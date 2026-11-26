# Box Thing v2

Handheld ESP32-S3 remote: 128x64 SH1106 OLED, rotary encoder, DRV2605 haptics, 1S LiPo.
PlatformIO, env `esp32s3`.

## Building

```
pio run -e esp32s3
```

Never run two builds at once: they share `.pio/build` and corrupt it.

## Libraries

Shared infrastructure comes from the `dcr_*` libraries (logger, netLink, httpClient,
files, taskManager, timeProfiler, fatalHandler, display, input) and `wireless_mesh`. Use them
instead of hand-rolling the same thing here:

- Logging: `debugI/W/E/D` with a per-file `LOG_TAG`, never `Serial.print`.
- Tasks: `taskManager.createTaskPinnedToCore`, call `taskManager.noteHeartbeat()` in the loop.
- Timing and rates: `timeProfiler.start/stop/increment`, not ad hoc counters.
- Screens: `Screen2` descriptors, declared in `src/Screens/Screens.h`.

- Input: `knob` and `knobButton` (dcr_input, `IO/GPIO`) are sampled by `Controls::update()`
  at the top of `loop()`; each reading holds for that frame. Menus, popups and widgets read
  them through the `EncoderInput` registered with `Inputs::addSource`.
- Fonts: `display.setTextSize(U8G2_TEXT_*)` picks a face from `IO/BoxThingDisplay`.

## Comments

The reader is a competent embedded C++ engineer.

- Three lines per comment block, maximum. Longer explanations go in a doc, with a
  one-line pointer from the code.
- Comment only what the code cannot say: hardware quirks, protocol constraints,
  deliberate deviations, locking and task affinity, units.
- No commented-out code, section banners, or change history. Git has it.
