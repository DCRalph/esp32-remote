#pragma once

static inline void floatToBytes(float val, uint8_t *bytes)
{
  memcpy(bytes, &val, sizeof(val));
}

static inline float bytesToFloat(uint8_t *bytes)
{
  float f;
  memcpy(&f, bytes, sizeof(f));
  return f;
}