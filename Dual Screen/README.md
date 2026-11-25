# Dual Screen

Two-screen handheld on dcr_display: the LilyGO T-Display S3's own 320x170 LCD
(two buttons) plus an ILI9341 320x240 SPI panel with XPT2046 touch. The base is
the hardware layer of prolink-lights' `esp32-companion`, without the
prolink-specific link and booth code.

## Wiring

| ILI9341 | GPIO | | Touch | GPIO |
| --- | --- | --- | --- | --- |
| CS | 10 | | T_CS | 1 |
| RST | 3 | | T_CLK | 12 (shared) |
| DC | 2 | | T_DIN | 11 (shared) |
| MOSI | 11 | | T_DO | 13 (shared) |
| SCLK | 12 | | | |
| MISO | 13 | | | |

## How it fits together

- `display` (LCD) and `touchDisplay` (ILI9341) are separate dcr_display displays,
  each with its own screen manager, menu sizing and input: the buttons drive the
  LCD, touch drives the panel. `Displays::render()` draws both from `loop()`.
- The ILI9341 is buffered (150 KB frame in PSRAM), so it redraws without flicker.
- A demo is a pair of screens. Its LCD half opens the touch half on enter and
  sends the touch panel home on exit, so either screen can start it.

## Buttons

One click moves, a double click selects. A triple click leaves screens that
have no menu.

## What to try

- **Shared controls:** the LCD menu and the touch lamp, slider and colour
  buttons edit the same values.
- **Sketch pad:** draw on the panel; the LCD mirrors it and picks the pen.
- **Cross popups:** each screen raises a popup on the other, answered with the
  other's input.
- **Type to LCD:** the touch keyboard types onto the LCD.
- **Dashboard:** live stats on the panel; the LCD picks the panel's theme.
- **UI test (LCD)** on the LCD home and **UI test (touch)** on the touch home run
  dcr_display's test screens with each screen's own input. Use one at a time:
  they share state.
- **Settings > Touch cal:** steps on the LCD, targets on the panel.

## Build

```
pio run
```

Factory Wi-Fi credentials go in `src/secrets.h` (copy `src/secrets.h.example`).
