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
files, taskManager, timeProfiler, fatalHandler, display) and `wireless_mesh`. Use them
instead of hand-rolling the same thing here:

- Logging: `debugI/W/E/D` with a per-file `LOG_TAG`, never `Serial.print`.
- Tasks: `taskManager.createTaskPinnedToCore`, call `taskManager.noteHeartbeat()` in the loop.
- Timing and rates: `timeProfiler.start/stop/increment`, not ad hoc counters.
- Screens: `Screen2` descriptors, declared in `src/Screens/Screens.h`.

`dcr_display` is pinned to a commit on its `mono-panel-support` branch. Move it to the
default branch once that work merges upstream.

## Comments

The reader is a competent embedded C++ engineer.

- Three lines per comment block, maximum. Longer explanations go in a doc, with a
  one-line pointer from the code.
- Comment only what the code cannot say: hardware quirks, protocol constraints,
  deliberate deviations, locking and task affinity, units.
- No commented-out code, section banners, or change history. Git has it.
