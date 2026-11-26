# controller-board

A touch test bench for the bioreactor-controller board: its ESP32-S3 (16 MB, PSRAM),
ILI9341 320x240 panel and XPT2046 touch, driven by dcr_display. It exercises the
library on the real panel and touch controller, and nothing else on the board.

## Flashing replaces the bioreactor firmware

It installs into the bioreactor's app slot. Reflash bioreactor-controller to put the
device back. Its data survives:

- The partition table is the bioreactor's, so NVS and LittleFS stay where it expects them.
- The bioreactor's NVS (`prefs`) is opened read-only, for the hardware version and
  device type. This project writes only its own namespace, `controller`, for the
  touch calibration.
- LittleFS is mounted without format-on-fail, only to read the bioreactor's touch
  calibration, and unmounted again.
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
| BOOT button | 0 | active low; only read by the Buttons screen |

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
- **Calibrate touch**: Dual Screen's calibrator. Tap a marker in each corner,
  holding still until you lift, then test it with a crosshair. The result is
  saved to NVS and loaded on every boot. Until then the bioreactor's own
  calibration file is used, else generic XPT2046 values. The calibrator also
  works out whether the controller's raw axes are swapped or reversed for this
  panel, and a saved calibration only applies at the rotation it was taken in.
- **Backlight**: a brightness slider, 5-100 %. The backlight pin is driven by
  20 kHz PWM with the hardware version's polarity. The level is not saved, so
  it is back to 100 % after a restart.
- **Buttons**: the power and BOOT buttons through dcr_input. Shows whether each
  is pressed, how long it has been held, and its last click sequence (clicks, or
  clicks then a long press). BOOT repeats every 200 ms while held past its long
  press; holding power for 2 s still switches the board off.
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
| `screen <home\|ui\|touch\|device\|cal>` | `Switched to <name>` |
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
