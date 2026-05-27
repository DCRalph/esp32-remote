#pragma once

#include <Arduino.h>
#include <stdint.h>

#define COMPANION_MAX_WAVEFORM_SEGMENTS 4096

struct CompanionWaveformState {
  bool valid;
  char transferId[24];
  uint32_t windowStartMs;
  uint32_t windowEndMs;
  uint32_t segmentIndexStart;
  uint16_t segmentCount;
  uint8_t *data;
  size_t dataLen;
  size_t dataCapacity;
};

bool companionBase64Decode(const char *input, uint8_t *output, size_t outputMax, size_t *written);

class CompanionWaveform {
public:
  void reset();
  bool beginTransfer(const char *transferId, uint16_t segmentCount, uint32_t windowStartMs,
                     uint32_t windowEndMs, uint32_t segmentIndexStart);
  bool appendChunk(const uint8_t *bytes, size_t len);
  void endTransfer(bool ok);
  const CompanionWaveformState &state() const { return state_; }

  using WaveformCallback = void (*)(const CompanionWaveformState &);
  void onUpdate(WaveformCallback cb) { updateCb_ = cb; }

private:
  CompanionWaveformState state_{};
  WaveformCallback updateCb_ = nullptr;
};
