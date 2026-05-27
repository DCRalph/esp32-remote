#include "CompanionWaveform.h"

#include <cstring>
#include <mbedtls/base64.h>

static void copyTrunc(char *dest, size_t destSize, const char *src) {
  if (!dest || destSize == 0) return;
  if (!src) {
    dest[0] = '\0';
    return;
  }
  strncpy(dest, src, destSize - 1);
  dest[destSize - 1] = '\0';
}

bool companionBase64Decode(const char *input, uint8_t *output, size_t outputMax, size_t *written) {
  size_t outLen = 0;
  int rc = mbedtls_base64_decode(output, outputMax, &outLen, (const unsigned char *)input,
                                 strlen(input));
  if (rc != 0) {
    return false;
  }
  *written = outLen;
  return true;
}

void CompanionWaveform::reset() {
  if (state_.data) {
    free(state_.data);
    state_.data = nullptr;
  }
  memset(&state_, 0, sizeof(state_));
}

bool CompanionWaveform::beginTransfer(const char *transferId, uint16_t segmentCount,
                                      uint32_t windowStartMs, uint32_t windowEndMs,
                                      uint32_t segmentIndexStart) {
  reset();

  if (segmentCount == 0 || segmentCount > COMPANION_MAX_WAVEFORM_SEGMENTS) {
    return false;
  }

  const size_t needBytes = (size_t)segmentCount * 2;
  state_.data = (uint8_t *)malloc(needBytes);
  if (!state_.data) {
    return false;
  }

  state_.dataCapacity = needBytes;
  state_.dataLen = 0;
  state_.segmentCount = segmentCount;
  state_.windowStartMs = windowStartMs;
  state_.windowEndMs = windowEndMs;
  state_.segmentIndexStart = segmentIndexStart;
  copyTrunc(state_.transferId, sizeof(state_.transferId), transferId);
  state_.valid = false;
  return true;
}

bool CompanionWaveform::appendChunk(const uint8_t *bytes, size_t len) {
  if (!state_.data || state_.dataLen + len > state_.dataCapacity) {
    return false;
  }
  memcpy(state_.data + state_.dataLen, bytes, len);
  state_.dataLen += len;
  return true;
}

void CompanionWaveform::endTransfer(bool ok) {
  const size_t expected = (size_t)state_.segmentCount * 2;
  state_.valid = ok && state_.data && state_.dataLen >= expected;
  if (state_.valid && updateCb_) {
    updateCb_(state_);
  }
}
