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
bioreactor-controller's `lib/TFT_eSPI/User_Setup.h` and `src/config.h`. Rotation
is 1, or 3 on the 8-way unit, as the bioreactor chooses from its device type.

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

## Build

```
pio run -e controller
pio run -e controller -t upload
```
