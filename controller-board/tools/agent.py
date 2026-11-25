#!/usr/bin/env python3
"""Drives a controller-board unit over USB serial (README.md, "USB control").

One process holds the port and talks to the device; short client calls ask
it for screenshots, taps and commands, so the port is never reopened (which
would reset the unit) and the whole serial stream is logged throughout.
Adapted from bioreactor-controller's scripts/agent.py.

  python tools/agent.py serve --port /dev/cu.usbmodem101  # keep running
  python tools/agent.py shot shot.png                     # screen -> PNG
  python tools/agent.py tap 160 120                       # press + release
  python tools/agent.py screen touch                      # jump to a screen
  python tools/agent.py wait "Switched to" 10             # regex on the stream
  python tools/agent.py tail 40
"""

import argparse
import base64
import json
import re
import socket
import struct
import sys
import threading
import time
import zlib
from collections import deque
from datetime import datetime
from pathlib import Path

PROMPT = "> "
ENTER = "\r"  # the CLI executes on CR and ignores LF
HOST, PORT = "127.0.0.1", 7421
CRASH_MARKERS = ("Guru Meditation", "Stack canary", "abort() was called", "Backtrace:", "[FATAL]", "rst:0x")
SHOT_HEADER = re.compile(r"SHOT-BEGIN w=(\d+) h=(\d+) fmt=rle565 bytes=(\d+) crc=([0-9A-Fa-f]{8})")
BASE64_LINE = re.compile(r"^[A-Za-z0-9+/=]+$")
# Tasks that log their own stack headroom: "name: 5120 B stack free".
SELF_REPORT = re.compile(r"(\w+): (\d+) B stack free")
# The last line each command prints. The prompt alone is not enough: HWCDC's
# flush() discards its TX buffer when its connected heuristic blinks, and the
# console flushes right after writing "> ". Searched anywhere in a line, since
# the newline after the echo can be lost the same way.
ACKS = {
    "tap": r"TAP \d+,\d+", "press": r"PRESS \d+,\d+", "release": r"RELEASE",
    "screen": r"Switched to|Usage: screen", "version": r"Version:",
}


# ---------------------------------------------------------------- device side

class Device:
    """The serial side: one reader thread logs every line, keeps the recent
    ones for crash context, and notes crash markers and self-reported stack."""

    def __init__(self, port, baud, log_path):
        import serial

        self.ser = serial.Serial()
        self.ser.port, self.ser.baudrate, self.ser.timeout = port, baud, 0.02
        self.ser.dtr = False  # a DTR/RTS pulse on open resets the unit
        self.ser.rts = False
        self.ser.open()
        self.log = open(log_path, "a", encoding="utf-8")
        self.recent = deque(maxlen=600)
        self.pending = deque()
        self.crashes = []  # {"at", "line", "context": lines before it}
        self.self_reported = {}  # task -> least bytes free it logged
        self.lock = threading.Lock()
        self.partial = ""
        threading.Thread(target=self._read, daemon=True).start()

    def _read(self):
        while True:
            try:
                data = self.ser.read(65536)
            except Exception as error:  # the port vanished; keep the process alive
                self._note(f"!! serial read failed: {error}")
                time.sleep(1)
                continue
            if not data:
                continue
            text = self.partial + data.decode("utf-8", errors="replace")
            parts = text.split("\n")
            self.partial = parts.pop()
            for line in parts:
                self._note(line.rstrip("\r"))
            if self.partial.endswith(PROMPT):
                self._note(self.partial)
                self.partial = ""

    def _note(self, line):
        stamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.log.write(f"{stamp} {line}\n")
        self.log.flush()
        with self.lock:
            if any(marker in line for marker in CRASH_MARKERS):
                self.crashes.append({"at": stamp, "line": line, "context": [l for _, l in self.recent]})
            match = SELF_REPORT.search(line)
            if match:
                name, free = match[1], int(match[2])
                self.self_reported[name] = min(free, self.self_reported.get(name, free))
            self.recent.append((time.monotonic(), f"{stamp} {line}"))
            self.pending.append(line)

    def recent_lines(self, n=None):
        with self.lock:
            lines = [line for _, line in self.recent]
        return lines if n is None else lines[-n:]

    def drain(self):
        with self.lock:
            lines = list(self.pending)
            self.pending.clear()
        return lines

    def send(self, text):
        self.drain()
        self.ser.write((text + ENTER).encode())

    def command(self, text, timeout=15.0):
        """Output of one command, up to its acknowledgement line or the next prompt."""
        ack = ACKS.get(text.split()[0]) if text.strip() else None
        ack = re.compile(ack) if ack else None
        self.send(text)
        out, deadline = [], time.monotonic() + timeout
        while time.monotonic() < deadline:
            for line in self.drain():
                if line.endswith(PROMPT) and out:
                    return out
                if line.strip() == text and not out:
                    continue  # the CLI echoes what was typed
                out.append(line)
                if ack and ack.search(line):
                    return out
            time.sleep(0.02)
        return out

    def wait_for_prompt(self, timeout=90.0):
        """True once the CLI answers again, e.g. after a reboot."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            self.ser.write(ENTER.encode())
            for line in self.drain():
                if line.endswith(PROMPT):
                    return True
            time.sleep(1)
        return False

    def handshake(self):
        """Flushes anything typed before and returns the device's version line, or None."""
        for _ in range(10):
            self.ser.write(ENTER.encode())
            time.sleep(0.3)
            version = next((l for l in self.command("version", timeout=2) if l.startswith("Version:")), None)
            if version:
                return version
            time.sleep(1)
        return None

    def shot(self, timeout=60.0):
        """Decoded screen as (w, h, rgb565 list), or raises."""
        self.send("shot")
        header, payload, deadline = None, [], time.monotonic() + timeout
        while time.monotonic() < deadline:
            for line in self.drain():
                if header is None:
                    match = SHOT_HEADER.search(line)
                    if match:
                        header = match
                    elif line.startswith("SHOT-ERROR"):
                        raise RuntimeError(line)
                    continue
                if line.startswith("SHOT-END"):
                    w, h, size, crc = int(header[1]), int(header[2]), int(header[3]), int(header[4], 16)
                    raw = base64.b64decode("".join(payload))
                    if len(raw) != size or zlib.crc32(raw) != crc:
                        raise RuntimeError(f"shot corrupt: {len(raw)}/{size} bytes, crc {zlib.crc32(raw):08X} vs {crc:08X}")
                    return w, h, decode_rle565(raw, w * h)
                if BASE64_LINE.match(line.strip()):  # IDF log lines bypass the muted logger
                    payload.append(line.strip())
            time.sleep(0.02)
        raise RuntimeError("shot timed out")

    def shot_png(self, path):
        """Screen to a PNG file; retries once, since a log line from another
        task can still cut a frame."""
        try:
            w, h, pixels = self.shot()
        except RuntimeError as error:
            if "corrupt" not in str(error):
                raise
            w, h, pixels = self.shot()
        write_png(path, w, h, pixels)
        return w, h

    def wait(self, pattern, timeout, lookback=5.0):
        """First match on the stream; also searches the last `lookback` seconds,
        which the previous command's own output collection may have consumed."""
        regex, deadline = re.compile(pattern), time.monotonic() + timeout
        with self.lock:
            for seen_at, line in self.recent:
                if seen_at >= time.monotonic() - lookback and regex.search(line):
                    return line
        while time.monotonic() < deadline:
            for line in self.drain():
                if regex.search(line):
                    return line
            time.sleep(0.05)
        return None


def decode_rle565(raw, pixels):
    out = []
    for count, lo, hi in struct.iter_unpack("<BBB", raw):
        out.extend([lo | (hi << 8)] * count)
    if len(out) != pixels:
        raise RuntimeError(f"shot decodes to {len(out)} pixels, expected {pixels}")
    return out


def write_png(path, w, h, pixels):
    rows = bytearray()
    for y in range(h):
        rows.append(0)  # filter: none
        for p in pixels[y * w:(y + 1) * w]:
            r, g, b = (p >> 11) & 0x1F, (p >> 5) & 0x3F, p & 0x1F
            rows += bytes(((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)))

    def chunk(kind, body):
        return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body) & 0xFFFFFFFF)

    png = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(bytes(rows), 9)) + chunk(b"IEND", b"")
    Path(path).write_bytes(png)


# ---------------------------------------------------------------- server

def serve(opts):
    device = Device(opts.port, opts.baud, opts.log)
    version = device.handshake()
    if not version:
        sys.exit("no answer to `version` on the port; close pio device monitor and check the unit")
    print(version)
    print(f"agent server on {HOST}:{PORT}, device on {opts.port}, log {opts.log}")

    def handle(conn):
        with conn:
            request = json.loads(conn.makefile("r", encoding="utf-8").readline() or "{}")
            try:
                reply = dispatch(device, request)
            except Exception as error:
                reply = {"error": str(error)}
            reply["crashes"] = len(device.crashes)
            conn.sendall((json.dumps(reply) + "\n").encode())

    with socket.create_server((HOST, PORT)) as listener:
        while True:
            conn, _ = listener.accept()
            handle(conn)


def dispatch(device, request):
    op = request.get("op")
    if op == "cmd":
        return {"lines": device.command(request["text"], request.get("timeout", 15))}
    if op == "shot":
        started = time.monotonic()
        w, h = device.shot_png(request["path"])
        return {"path": request["path"], "width": w, "height": h, "ms": int((time.monotonic() - started) * 1000)}
    if op == "tail":
        return {"lines": device.recent_lines(request.get("n", 40))}
    if op == "wait":
        return {"matched": device.wait(request["pattern"], request.get("timeout", 10))}
    if op == "ping":
        return {"ok": True}
    raise ValueError(f"unknown op {op!r}")


# ---------------------------------------------------------------- client

def call(request):
    try:
        with socket.create_connection((HOST, PORT), timeout=120) as conn:
            conn.sendall((json.dumps(request) + "\n").encode())
            reply = json.loads(conn.makefile("r", encoding="utf-8").readline() or "{}")
    except ConnectionRefusedError:
        sys.exit("no agent server; start one: python tools/agent.py serve --port <port>")
    if "error" in reply:
        sys.exit(f"error: {reply['error']}")
    return reply


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="op", required=True)
    s = sub.add_parser("serve")
    s.add_argument("--port", required=True)
    s.add_argument("--baud", type=int, default=115200)
    s.add_argument("--log", default="agent.log")
    sub.add_parser("shot").add_argument("path", nargs="?", default="shot.png")
    for name in ("tap", "press"):
        p = sub.add_parser(name)
        p.add_argument("x", type=int)
        p.add_argument("y", type=int)
    sub.add_parser("release")
    c = sub.add_parser("cmd")
    c.add_argument("text")
    c.add_argument("--timeout", type=float, default=15)
    sub.add_parser("screen").add_argument("name")
    sub.add_parser("tail").add_argument("n", type=int, nargs="?", default=40)
    w = sub.add_parser("wait")
    w.add_argument("pattern")
    w.add_argument("timeout", type=float, nargs="?", default=10)
    sub.add_parser("ping")
    opts = parser.parse_args()

    if opts.op == "serve":
        return serve(opts)
    if opts.op == "shot":
        reply = call({"op": "shot", "path": str(Path(opts.path).resolve())})
        print(f"{reply['path']} {reply['width']}x{reply['height']} in {reply['ms']} ms")
    elif opts.op in ("tap", "press"):
        reply = call({"op": "cmd", "text": f"{opts.op} {opts.x} {opts.y}", "timeout": 5})
        print("\n".join(reply["lines"]))
    elif opts.op == "release":
        reply = call({"op": "cmd", "text": "release", "timeout": 5})
        print("\n".join(reply["lines"]))
    elif opts.op == "cmd":
        reply = call({"op": "cmd", "text": opts.text, "timeout": opts.timeout})
        print("\n".join(reply["lines"]))
    elif opts.op == "screen":
        reply = call({"op": "cmd", "text": f"screen {opts.name}", "timeout": 5})
        print("\n".join(reply["lines"]))
    elif opts.op == "tail":
        reply = call({"op": "tail", "n": opts.n})
        print("\n".join(reply["lines"]))
    elif opts.op == "wait":
        reply = call({"op": "wait", "pattern": opts.pattern, "timeout": opts.timeout})
        if reply["matched"] is None:
            sys.exit(f"no match for {opts.pattern!r} within {opts.timeout} s")
        print(reply["matched"])
    elif opts.op == "ping":
        reply = call({"op": "ping"})
    if reply.get("crashes"):
        print(f"!! {reply['crashes']} crash marker(s) seen on the serial stream; see agent.log", file=sys.stderr)


if __name__ == "__main__":
    main()
