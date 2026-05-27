#include "CompanionSse.h"

#include "CompanionClock.h"
#include "CompanionProtocol.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>

namespace
{
  WiFiClient s_tcpClient;
  HTTPClient s_http;
} // namespace

void CompanionSse::setContext(CompanionProtocol *protocol, CompanionClock *clock, int *lastStatusSeq,
                              String *lastWaveformTransferId)
{
  protocol_ = protocol;
  clock_ = clock;
  lastStatusSeq_ = lastStatusSeq;
  lastWaveformTransferId_ = lastWaveformTransferId;
}

void CompanionSse::resetParser()
{
  lineBuf_ = "";
  dataBuf_ = "";
}

void CompanionSse::disconnect()
{
  if (connected_)
  {
    s_http.end();
    connected_ = false;
  }
  resetParser();
}

bool CompanionSse::connect(const String &url, const String &bearerToken)
{
  disconnect();

  if (!protocol_ || bearerToken.length() == 0)
    return false;

  s_http.setTimeout(500);
  if (!s_http.begin(s_tcpClient, url))
    return false;

  s_http.addHeader("Accept", "text/event-stream");
  s_http.addHeader("Authorization", String("Bearer ") + bearerToken);
  s_http.setReuse(false);

  const int code = s_http.GET();
  if (code != 200)
  {
    s_http.end();
    return false;
  }

  WiFiClient *stream = s_http.getStreamPtr();
  if (!stream)
  {
    s_http.end();
    return false;
  }

  connected_ = true;
  resetParser();
  return true;
}

bool CompanionSse::applyPollPayload(const char *json, size_t len)
{
  DynamicJsonDocument doc(16384);
  if (deserializeJson(doc, json, len))
    return false;

  JsonObject data = doc.as<JsonObject>();
  if (data.isNull())
    return false;

  JsonObject sync = data["sync"].as<JsonObject>();
  if (!sync.isNull() && clock_)
  {
    clock_->onPollSync(sync["serverTimeMs"] | 0ULL, sync["serverMonoMs"] | 0.0, millis(), 0);
  }

  protocol_->applyPoll(data);

  JsonObject status = data["status"].as<JsonObject>();
  if (!status.isNull() && lastStatusSeq_)
  {
    *lastStatusSeq_ = status["seq"] | *lastStatusSeq_;
  }

  JsonObject waveform = data["waveform"].as<JsonObject>();
  if (!waveform.isNull() && lastWaveformTransferId_)
  {
    const char *tid = waveform["transferId"] | "";
    if (tid[0])
      *lastWaveformTransferId_ = tid;
  }

  protocol_->setHostReady(true);
  return true;
}

bool CompanionSse::deliverEvent()
{
  if (dataBuf_.length() == 0)
    return true;

  const bool ok = applyPollPayload(dataBuf_.c_str(), dataBuf_.length());
  dataBuf_ = "";
  return ok;
}

bool CompanionSse::pump()
{
  if (!connected_)
    return false;

  WiFiClient *stream = s_http.getStreamPtr();
  if (!stream || !stream->connected())
  {
    disconnect();
    return false;
  }

  while (stream->available())
  {
    const char c = (char)stream->read();

    if (c == '\r')
      continue;

    if (c == '\n')
    {
      if (lineBuf_.length() == 0)
      {
        if (!deliverEvent())
        {
          disconnect();
          return false;
        }
        continue;
      }

      if (lineBuf_.startsWith("data:"))
      {
        String value = lineBuf_.substring(5);
        value.trim();
        if (dataBuf_.length() + value.length() + 1 > kMaxDataBytes)
        {
          disconnect();
          return false;
        }
        if (dataBuf_.length() > 0)
          dataBuf_ += '\n';
        dataBuf_ += value;
      }
      else if (lineBuf_.startsWith(":"))
      {
        // comment / keepalive — ignore
      }

      lineBuf_ = "";
      continue;
    }

    if (lineBuf_.length() < 256)
      lineBuf_ += c;
  }

  return true;
}
