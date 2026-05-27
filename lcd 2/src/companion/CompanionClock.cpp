#include "CompanionClock.h"

void CompanionClock::onPollSync(uint64_t serverTimeMs, double serverMonoMs, uint32_t localMillis, uint32_t rttMs) {
  const uint32_t midpoint = localMillis - rttMs / 2;
  clockOffsetMs_ = (int64_t)serverTimeMs - (int64_t)midpoint;
  hostMonoAtSync_ = serverMonoMs;
  localMillisAtSync_ = localMillis;
  valid_ = true;
}

uint64_t CompanionClock::serverTimeMsNow() const {
  if (!valid_) return (uint64_t)millis();
  return (uint64_t)((int64_t)millis() + clockOffsetMs_);
}

uint32_t CompanionClock::extrapolatedPlayheadMs(uint32_t playheadMs, double playheadCapturedAtMonoMs) const {
  if (!valid_ || playheadCapturedAtMonoMs <= 0) return playheadMs;
  const double deltaMono = (double)millis() - (double)localMillisAtSync_ + (hostMonoAtSync_ - playheadCapturedAtMonoMs);
  const int64_t next = (int64_t)playheadMs + (int64_t)deltaMono;
  return next < 0 ? 0 : (uint32_t)next;
}
