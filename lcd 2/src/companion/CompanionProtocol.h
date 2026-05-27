#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "CompanionWaveform.h"

#define COMPANION_PROTOCOL_VERSION 1

struct CompanionDisplayState {
  char profileName[32];
  char pageName[48];
  uint8_t pageIndex;
  uint8_t pageCount;
  bool controllerConnected;
  char lastInput[24];
  float brightness;
  bool blackout;
  char activeScene[40];
  char activeEffect[40];
  float masterBpm;
  uint8_t beatInMeasure;
  uint16_t bar;
  uint32_t playheadMs;
  uint32_t trackLengthMs;
  uint32_t waveformWindowStartMs;
  uint32_t waveformWindowEndMs;
  uint32_t waveformSegmentIndexStart;
  uint64_t ts;
};

class CompanionProtocol {
public:
  void begin(void (*writeFn)(const char *line));
  void onLine(const char *line);
  void pushBytes(const uint8_t *data, size_t len);
  void sendHello(const char *deviceId, const char *fw);
  void sendCmd(const char *actionType, const char *configJson = "{}");
  void sendPing();
  void sendPageNext();
  void sendPagePrev();
  void sendWaveformRefresh();

  /** Apply HTTP poll JSON (`data` object from host). */
  void applyPoll(JsonObject data);
  void setHostReady(bool ready) { hostReady_ = ready; }

  bool isHostReady() const { return hostReady_; }
  const CompanionDisplayState &display() const { return display_; }
  const CompanionWaveform &waveform() const { return waveform_; }

  /** Clears host handshake and display/waveform state (e.g. on BLE disconnect). */
  void resetSession();

  using DisplayCallback = void (*)(const CompanionDisplayState &);
  void onDisplayUpdate(DisplayCallback cb) { displayCb_ = cb; }

  void onWaveformUpdate(CompanionWaveform::WaveformCallback cb) { waveform_.onUpdate(cb); }

private:
  void (*writeFn_)(const char *line) = nullptr;
  DisplayCallback displayCb_ = nullptr;
  bool hostReady_ = false;
  CompanionDisplayState display_{};
  CompanionWaveform waveform_;
  char lineBuffer_[520];
  size_t lineLen_ = 0;
  bool chunkMode_ = false;
  size_t chunkExpected_ = 0;
  String chunkBuffer_;
  uint8_t chunkDecodeBuf_[256];

  void writeEnvelope(const char *type, JsonObject payload);
  void handleEnvelope(const char *type, JsonObject payload);
  void applyStatus(JsonObject payload);
  void handleWaveformBegin(JsonObject payload);
  void handleWaveformChunk(JsonObject payload);
  void handleWaveformEnd(JsonObject payload);
  void feedByte(char c);
};
