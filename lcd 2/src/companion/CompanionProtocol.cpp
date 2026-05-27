#include "CompanionProtocol.h"

#include <cstring>

#include "CompanionWaveform.h"

static void copyTrunc(char *dest, size_t destSize, const char *src) {
  if (!dest || destSize == 0) return;
  if (!src) {
    dest[0] = '\0';
    return;
  }
  strncpy(dest, src, destSize - 1);
  dest[destSize - 1] = '\0';
}

void CompanionProtocol::begin(void (*writeFn)(const char *line)) {
  writeFn_ = writeFn;
  resetSession();
}

void CompanionProtocol::resetSession() {
  hostReady_ = false;
  memset(&display_, 0, sizeof(display_));
  lineLen_ = 0;
  chunkMode_ = false;
  chunkBuffer_ = "";
  chunkExpected_ = 0;
  waveform_.reset();
}

void CompanionProtocol::writeEnvelope(const char *type, JsonObject payload) {
  if (!writeFn_) return;

  StaticJsonDocument<384> doc;
  doc["v"] = COMPANION_PROTOCOL_VERSION;
  doc["t"] = type;
  doc["payload"] = payload;

  char buffer[400];
  size_t len = serializeJson(doc, buffer, sizeof(buffer));
  if (len == 0 || len >= sizeof(buffer) - 2) return;
  buffer[len] = '\n';
  buffer[len + 1] = '\0';
  writeFn_(buffer);
}

void CompanionProtocol::sendHello(const char *deviceId, const char *fw) {
  StaticJsonDocument<192> doc;
  JsonObject payload = doc.to<JsonObject>();
  payload["deviceId"] = deviceId;
  payload["fw"] = fw;
  JsonArray caps = payload["capabilities"].to<JsonArray>();
  caps.add("status");
  caps.add("cmd");
  caps.add("ping");
  writeEnvelope("hello", payload);
}

void CompanionProtocol::sendCmd(const char *actionType, const char *configJson) {
  StaticJsonDocument<256> doc;
  JsonObject payload = doc.to<JsonObject>();
  payload["id"] = String((uint32_t)esp_random(), HEX);

  JsonObject action = payload["action"].to<JsonObject>();
  action["type"] = actionType;

  StaticJsonDocument<128> cfgDoc;
  if (configJson && configJson[0]) {
    deserializeJson(cfgDoc, configJson);
  }
  action["config"] = cfgDoc.as<JsonObject>();

  writeEnvelope("cmd", payload);
}

void CompanionProtocol::sendPing() {
  StaticJsonDocument<64> doc;
  JsonObject payload = doc.to<JsonObject>();
  payload["ts"] = (uint64_t)millis();
  writeEnvelope("ping", payload);
}

void CompanionProtocol::sendPageNext() { sendCmd("page-next"); }
void CompanionProtocol::sendPagePrev() { sendCmd("page-prev"); }
void CompanionProtocol::sendWaveformRefresh() { sendCmd("waveform.refresh"); }

void CompanionProtocol::applyStatus(JsonObject payload) {
  copyTrunc(display_.profileName, sizeof(display_.profileName), payload["profileName"] | "");
  copyTrunc(display_.pageName, sizeof(display_.pageName), payload["pageName"] | "");
  display_.pageIndex = payload["pageIndex"] | 0;
  display_.pageCount = payload["pageCount"] | 0;
  display_.controllerConnected = payload["controllerConnected"] | false;
  copyTrunc(display_.lastInput, sizeof(display_.lastInput), payload["lastInput"] | "");
  display_.brightness = payload["brightness"] | 0.0f;
  display_.blackout = payload["blackout"] | false;
  copyTrunc(display_.activeScene, sizeof(display_.activeScene), payload["activeScene"] | "");
  copyTrunc(display_.activeEffect, sizeof(display_.activeEffect), payload["activeEffect"] | "");
  display_.masterBpm = payload["masterBpm"] | 0.0f;
  display_.beatInMeasure = payload["beatInMeasure"] | 0;
  display_.bar = payload["bar"] | 0;
  display_.playheadMs = payload["playheadMs"] | 0;
  display_.trackLengthMs = payload["trackLengthMs"] | 0;
  display_.waveformWindowStartMs = payload["waveformWindowStartMs"] | 0;
  display_.waveformWindowEndMs = payload["waveformWindowEndMs"] | 0;
  display_.waveformSegmentIndexStart = payload["waveformSegmentIndexStart"] | 0;
  display_.ts = payload["ts"] | 0ULL;

  if (displayCb_) {
    displayCb_(display_);
  }
}

void CompanionProtocol::handleWaveformBegin(JsonObject payload) {
  const char *transferId = payload["transferId"] | "";
  const uint16_t segmentCount = payload["segmentCount"] | 0;
  const uint32_t windowStartMs = payload["windowStartMs"] | 0;
  const uint32_t windowEndMs = payload["windowEndMs"] | 0;
  const uint32_t segmentIndexStart = payload["segmentIndexStart"] | 0;
  waveform_.beginTransfer(transferId, segmentCount, windowStartMs, windowEndMs, segmentIndexStart);
}

void CompanionProtocol::handleWaveformChunk(JsonObject payload) {
  const char *b64 = payload["data"] | "";
  if (!b64[0]) return;
  size_t written = 0;
  if (!companionBase64Decode(b64, chunkDecodeBuf_, sizeof(chunkDecodeBuf_), &written)) {
    return;
  }
  waveform_.appendChunk(chunkDecodeBuf_, written);
}

void CompanionProtocol::handleWaveformEnd(JsonObject payload) {
  const bool ok = payload["ok"] | false;
  waveform_.endTransfer(ok);
}

void CompanionProtocol::applyPoll(JsonObject data) {
  JsonObject sync = data["sync"].as<JsonObject>();
  if (!sync.isNull()) {
    (void)sync;
  }

  JsonObject status = data["status"].as<JsonObject>();
  if (!status.isNull()) {
    JsonObject payload = status["payload"].as<JsonObject>();
    if (!payload.isNull()) {
      applyStatus(payload);
      hostReady_ = true;
    }
  }

  JsonObject waveform = data["waveform"].as<JsonObject>();
  if (!waveform.isNull()) {
    JsonObject begin = waveform["begin"].as<JsonObject>();
    if (!begin.isNull()) {
      handleWaveformBegin(begin);
    }
    JsonArray chunks = waveform["chunks"].as<JsonArray>();
    for (JsonObject chunk : chunks) {
      handleWaveformChunk(chunk);
    }
    JsonObject end = waveform["end"].as<JsonObject>();
    if (!end.isNull()) {
      handleWaveformEnd(end);
    }
  }
}

void CompanionProtocol::handleEnvelope(const char *type, JsonObject payload) {
  if (strcmp(type, "hello.ack") == 0) {
    hostReady_ = true;
#if DEBUG
    Serial.println("[CompanionBLE] hello.ack — host ready");
#endif
    return;
  }
  if (strcmp(type, "status") == 0) {
    applyStatus(payload);
    return;
  }
  if (strcmp(type, "cmd.result") == 0) {
    return;
  }
  if (strcmp(type, "pong") == 0) {
    return;
  }
  if (strcmp(type, "waveform.begin") == 0) {
    handleWaveformBegin(payload);
    return;
  }
  if (strcmp(type, "waveform.chunk") == 0) {
    handleWaveformChunk(payload);
    return;
  }
  if (strcmp(type, "waveform.end") == 0) {
    handleWaveformEnd(payload);
    return;
  }
}

void CompanionProtocol::feedByte(char c) {
  if (chunkMode_) {
    chunkBuffer_ += c;
    if (chunkBuffer_.length() >= chunkExpected_) {
      String line = chunkBuffer_;
      chunkMode_ = false;
      chunkBuffer_ = "";
      onLine(line.c_str());
    }
    return;
  }

  if (c == '\n') {
    if (lineLen_ == 0) return;
    lineBuffer_[lineLen_] = '\0';

    if (strncmp(lineBuffer_, "C:", 2) == 0) {
      const char *colon = strchr(lineBuffer_ + 2, ':');
      if (colon) {
        chunkExpected_ = (size_t)atoi(lineBuffer_ + 2);
        chunkMode_ = true;
        chunkBuffer_ = String(colon + 1);
        lineLen_ = 0;
        if (chunkBuffer_.length() >= chunkExpected_) {
          String line = chunkBuffer_;
          chunkMode_ = false;
          chunkBuffer_ = "";
          onLine(line.c_str());
        }
        return;
      }
    }

    onLine(lineBuffer_);
    lineLen_ = 0;
    return;
  }

  if (lineLen_ < sizeof(lineBuffer_) - 1) {
    lineBuffer_[lineLen_++] = c;
  }
}

void CompanionProtocol::pushBytes(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    feedByte((char)data[i]);
  }
}

void CompanionProtocol::onLine(const char *line) {
  DynamicJsonDocument doc(2048);
  if (deserializeJson(doc, line)) {
#if DEBUG
    Serial.printf("[CompanionBLE] JSON parse fail: %.80s\n", line);
#endif
    return;
  }

  int version = doc["v"] | 0;
  if (version != COMPANION_PROTOCOL_VERSION) {
#if DEBUG
    Serial.printf("[CompanionBLE] bad protocol v=%d\n", version);
#endif
    return;
  }

  const char *type = doc["t"] | "";
#if DEBUG
  Serial.printf("[CompanionBLE] envelope: %s\n", type);
#endif
  JsonObject payload = doc["payload"].as<JsonObject>();
  handleEnvelope(type, payload);
}
