#pragma once

#include <Arduino.h>

/** RTT-aware clock sync from HTTP poll `sync` + playhead anchor fields. */
class CompanionClock {
public:
  void onPollSync(uint64_t serverTimeMs, double serverMonoMs, uint32_t localMillis, uint32_t rttMs);

  uint64_t serverTimeMsNow() const;
  uint32_t extrapolatedPlayheadMs(uint32_t playheadMs, double playheadCapturedAtMonoMs) const;

private:
  bool valid_ = false;
  int64_t clockOffsetMs_ = 0;
  double hostMonoAtSync_ = 0;
  uint32_t localMillisAtSync_ = 0;
};
