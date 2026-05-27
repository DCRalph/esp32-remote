#pragma once

#include <Arduino.h>

class CompanionClock;
class CompanionProtocol;

/** Long-lived SSE reader for companion display updates. */
class CompanionSse {
public:
  void setContext(CompanionProtocol *protocol, CompanionClock *clock, int *lastStatusSeq,
                  String *lastWaveformTransferId);

  bool connect(const String &url, const String &bearerToken);
  void disconnect();
  bool isConnected() const { return connected_; }

  /** Non-blocking read/parse; returns false when the stream is dead. */
  bool pump();

private:
  void resetParser();
  bool deliverEvent();
  bool applyPollPayload(const char *json, size_t len);

  CompanionProtocol *protocol_ = nullptr;
  CompanionClock *clock_ = nullptr;
  int *lastStatusSeq_ = nullptr;
  String *lastWaveformTransferId_ = nullptr;

  bool connected_ = false;
  String lineBuf_;
  String dataBuf_;
  static constexpr size_t kMaxDataBytes = 24576;
};
