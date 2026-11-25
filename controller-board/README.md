# controller-board

A touch test bench for the bioreactor-controller board: its ESP32-S3 (16 MB, PSRAM),
ILI9341 320x240 panel and XPT2046 touch, driven by dcr_display. It exercises the
library on the real panel and touch controller, and nothing else on the board.

## Flashing replaces the bioreactor firmware

It installs into the bioreactor's app slot. Reflash bioreactor-controller to put the
device back. Its data survives:

- The partition table is the bioreactor's, so NVS and LittleFS stay where it expects them.
- NVS is opened read-only, for the hardware version and device type.
- LittleFS is mounted without format-on-fail, only to read the touch calibration,
  and unmounted again. Calibrating here keeps the result in RAM; nothing is written.
- dcr_files is left out on purpose: its `Files::init()` formats LittleFS if a mount fails.

The heater coprocessor, pumps, RFID and battery chips are not driven at all.

## Hardware

| | GPIO | |
| --- | --- | --- |
| TFT CS / DC / RST | 10 / 7 / 6 | ILI9341, 70 MHz SPI |
| SCLK / MOSI / MISO | 12 / 11 / 13 | shared by panel and touch |
| Touch CS | 16 | XPT2046 through TFT_eSPI, 2.5 MHz |
| Backlight | 38 | active high on hardware v0, v1, v4; low on v2, v3 |
| Power latch | 5 | held high from boot |
| Power button | 15 | active high; hold 2 s to switch off |

Pins and the TFT_eSPI setup are build flags in `platformio.ini`, copied from
bioreactor-controller's `lib/TFT_eSPI/User_Setup.h` and `src/config.h`. The screen
is turned 180 degrees from the bioreactor (`ROTATE_180` in `src/config.h`): rotation
3, or 1 on the 8-way unit. The bioreactor's saved touch calibration is flipped to
match, so touch lines up without recalibrating.

## Screens

- **UI test**: every dcr_display feature (menus, widgets, gestures, popups,
  keyboard, animation, drawing) on this panel.
- **Touch test**: draw with a finger. Shows raw controller x, y and pressure next
  to the filtered pointer, with a crosshair that should sit under the finger.
- **Calibrate touch**: the bioreactor's four-corner TFT_eSPI calibration.
- **Device**: what the unit reported at boot, frame rate and memory.
- **Power off**.

Touch filtering matches the bioreactor: a press starts above raw pressure 600,
holds above 20, and survives 5 empty polls; touch is also sampled mid-frame so
short taps register. Serial logs frame timing once a second.

## USB control

The USB serial port takes the bioreactor's agent commands, so the UI can be
driven and checked from a computer. A line runs on CR or LF; each command ends
with a `> ` prompt.

| Command | Answer |
| --- | --- |
| `tap <x> <y>` | `TAP x,y`; a press and release over the next two frames |
| `press <x> <y>` / `release` | `PRESS x,y` / `RELEASE`; a held touch, for long presses and drags |
| `shot` | the screen as `SHOT-BEGIN w= h= fmt=rle565 bytes= crc=`, base64 lines, `SHOT-END` |
| `screen <home\|ui\|touch\|device>` | `Switched to <name>` |
| `version` | `Version: controller-board <build time>` |

Coordinates are screen pixels (320 x 240, origin top left), so a point read off a
shot is the point to tap. Injected touches go through the same filtering and
gestures as a finger. `shot` sends runs of `[count][low][high]` RGB565 and a
CRC-32, written straight to the USB FIFO with logging muted.

`tools/agent.py` (from bioreactor-controller's `scripts/agent.py`) holds the port
so it is opened once (reopening resets the board), logs the stream, and answers
short calls:

```
python tools/agent.py serve --port /dev/cu.usbmodem101   # leave running
python tools/agent.py shot shot.png
python tools/agent.py tap 160 120
python tools/agent.py screen touch
```

It needs `pyserial`.

## Build

```
pio run -e controller
pio run -e controller -t upload
```
