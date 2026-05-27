#include "CompanionHttp.h"



#include "CompanionClock.h"

#include "CompanionSse.h"

#include "config.h"



#include <ESPmDNS.h>

#include <WiFi.h>

#include <dcr_HttpRequest.h>

#include <dcr_NetLink.h>



#define FIRMWARE_VERSION "1.0.0"

// #define DEFAULT_HOST "prolink-lights.local"

#define DEFAULT_HOST "10.0.0.51"

#define DEFAULT_PORT 3000

#define POLL_INTERVAL_MS 150

#define SSE_RECONNECT_MIN_MS 1000

#define SSE_RECONNECT_MAX_MS 5000



static CompanionProtocol s_protocol;

static CompanionClock s_clock;

static CompanionSse s_sse;

static bool s_active = false;

static CompanionPairState s_state = CompanionPairState::WifiDown;

static String s_baseUrl;

static String s_token;

static String s_deviceId;

static char s_macString[18] = "00:00:00:00:00:00";

static uint32_t lastPollMs = 0;

static uint32_t lastSseAttemptMs = 0;

static uint32_t sseReconnectBackoffMs = SSE_RECONNECT_MIN_MS;

static int lastStatusSeq = 0;

static String lastWaveformTransferId;



static String authHeaders() {

  if (s_token.length() == 0) return "";

  return String("Authorization: Bearer ") + s_token + "\n";

}



static void loadCredentials() {

  preferences.begin("companion", true);

  s_token = preferences.getString("token", "");

  s_baseUrl = preferences.getString("baseUrl", "");

  preferences.end();

}



static void saveCredentials() {

  preferences.begin("companion", false);

  preferences.putString("token", s_token);

  preferences.putString("baseUrl", s_baseUrl);

  preferences.end();

}



static void buildDeviceId() {

  uint8_t mac[6];

  WiFi.macAddress(mac);

  snprintf(s_macString, sizeof(s_macString), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3],

           mac[4], mac[5]);

  s_deviceId = String("display-") + s_macString;

}



static bool applyPollData(JsonObject data, uint32_t rttMs) {

  if (data.isNull()) return false;



  JsonObject sync = data["sync"].as<JsonObject>();

  if (!sync.isNull()) {

    s_clock.onPollSync(

        sync["serverTimeMs"] | 0ULL,

        sync["serverMonoMs"] | 0.0,

        millis(),

        rttMs);

  }



  s_protocol.applyPoll(data);



  JsonObject status = data["status"].as<JsonObject>();

  if (!status.isNull()) {

    lastStatusSeq = status["seq"] | lastStatusSeq;

  }



  JsonObject waveform = data["waveform"].as<JsonObject>();

  if (!waveform.isNull()) {

    const char *tid = waveform["transferId"] | "";

    if (tid[0]) lastWaveformTransferId = tid;

  }



  s_protocol.setHostReady(true);

  s_state = CompanionPairState::Ready;

  return true;

}



static String buildEventsUrl() {

  String url = s_baseUrl + "/api/companion/v1/events?sinceStatusSeq=" + String(lastStatusSeq);

  if (lastWaveformTransferId.length() > 0) {

    url += "&sinceWaveformTransferId=" + lastWaveformTransferId;

  }

  return url;

}



static bool tryConnectSse() {

  if (!CompanionHttp::ensureBaseUrl()) return false;

  return s_sse.connect(buildEventsUrl(), s_token);

}



bool CompanionHttp::ensureBaseUrl() {

  if (s_baseUrl.length() > 0) return true;



  if (!netLink.isConnected()) {

    s_state = CompanionPairState::WifiDown;

    return false;

  }



  s_state = CompanionPairState::Discovering;



  IPAddress resolved;

  if (WiFi.hostByName(DEFAULT_HOST, resolved)) {

    s_baseUrl = String("http://") + resolved.toString() + ":" + String(DEFAULT_PORT);

    saveCredentials();

    return true;

  }



  int n = MDNS.queryService("prolink-lights", "tcp");

  for (int i = 0; i < n; i++) {

    if (MDNS.hostname(i)) {

      s_baseUrl = String("http://") + MDNS.hostname(i) + ":" + String(MDNS.port(i));

      saveCredentials();

      return true;

    }

  }



  s_baseUrl = String("http://") + DEFAULT_HOST + ":" + String(DEFAULT_PORT);

  return true;

}



bool CompanionHttp::registerClient() {

  if (!ensureBaseUrl()) return false;



  StaticJsonDocument<256> body;

  body["clientId"] = s_deviceId;

  body["clientType"] = "display";

  body["firmware"] = FIRMWARE_VERSION;

  JsonArray caps = body["capabilities"].to<JsonArray>();

  caps.add("status");

  caps.add("cmd");



  String payload;

  serializeJson(body, payload);



  HTTP::HttpResponse res = HTTP::post(

      s_baseUrl + "/api/v1/auth/register",

      payload,

      "application/json",

      "",

      10000);



  if (!res.success || res.statusCode < 200 || res.statusCode >= 300) {

    s_state = CompanionPairState::PendingApproval;

    return false;

  }



  DynamicJsonDocument doc(1024);

  if (deserializeJson(doc, res.payload)) return false;



  JsonObject data = doc["data"].as<JsonObject>();

  const char *status = data["status"] | "pending";

  if (strcmp(status, "approved") == 0) {

    const char *token = data["token"] | "";

    if (token[0]) {

      s_token = token;

      saveCredentials();

    }

    s_state = CompanionPairState::Ready;

    s_protocol.setHostReady(true);

    return s_token.length() > 0;

  }



  if (strcmp(status, "revoked") == 0) {

    s_token = "";

    saveCredentials();

    s_state = CompanionPairState::PendingApproval;

    return false;

  }



  s_state = CompanionPairState::PendingApproval;

  return false;

}



bool CompanionHttp::pollOnce() {

  if (s_token.length() == 0) return registerClient();



  String url = s_baseUrl + "/api/companion/v1/poll?sinceStatusSeq=" + String(lastStatusSeq);

  if (lastWaveformTransferId.length() > 0) {

    url += "&sinceWaveformTransferId=" + lastWaveformTransferId;

  }



  const uint32_t t0 = millis();

  HTTP::HttpResponse res = HTTP::get(url, authHeaders(), 10000);

  const uint32_t rtt = millis() - t0;



  if (!res.success || res.statusCode == 401) {

    s_token = "";

    saveCredentials();

    s_protocol.resetSession();

    s_sse.disconnect();

    s_state = CompanionPairState::PendingApproval;

    return false;

  }



  if (res.statusCode < 200 || res.statusCode >= 300) return false;



  DynamicJsonDocument doc(16384);

  if (deserializeJson(doc, res.payload)) return false;



  JsonObject data = doc["data"].as<JsonObject>();

  return applyPollData(data, rtt);

}



void CompanionHttp::begin() {

  if (s_active) return;

  s_active = true;

  loadCredentials();

  buildDeviceId();

  s_protocol.resetSession();

  s_protocol.setHostReady(false);

  lastStatusSeq = 0;

  lastWaveformTransferId = "";

  lastPollMs = 0;

  lastSseAttemptMs = 0;

  sseReconnectBackoffMs = SSE_RECONNECT_MIN_MS;

  s_sse.setContext(&s_protocol, &s_clock, &lastStatusSeq, &lastWaveformTransferId);

  s_sse.disconnect();



  if (!netLink.isConnected()) {

    netLink.on();

    netLink.connect();

  }



  MDNS.begin("prolink-display");



  if (s_token.length() > 0 && s_baseUrl.length() > 0) {

    s_state = CompanionPairState::Ready;

  } else {

    s_state = CompanionPairState::WifiDown;

  }

}



void CompanionHttp::shutdown() {

  if (!s_active) return;

  s_active = false;

  s_sse.disconnect();

  s_protocol.resetSession();

  s_state = CompanionPairState::WifiDown;

  MDNS.end();

}



bool CompanionHttp::isActive() { return s_active; }



void CompanionHttp::loop() {

  if (!s_active) return;



  if (!netLink.isConnected()) {

    s_state = CompanionPairState::WifiDown;

    s_sse.disconnect();

    return;

  }



  const uint32_t now = millis();



  if (s_token.length() == 0) {

    if (now - lastPollMs >= POLL_INTERVAL_MS) {

      lastPollMs = now;

      registerClient();

    }

    return;

  }



  if (s_sse.isConnected()) {

    if (!s_sse.pump()) {

      sseReconnectBackoffMs = min(sseReconnectBackoffMs * 2, (uint32_t)SSE_RECONNECT_MAX_MS);

      lastSseAttemptMs = now;

    }

    return;

  }



  if (now - lastSseAttemptMs >= sseReconnectBackoffMs) {

    lastSseAttemptMs = now;

    if (tryConnectSse()) {

      sseReconnectBackoffMs = SSE_RECONNECT_MIN_MS;

      return;

    }

    sseReconnectBackoffMs = min(sseReconnectBackoffMs * 2, (uint32_t)SSE_RECONNECT_MAX_MS);

  }



  if (now - lastPollMs < POLL_INTERVAL_MS) return;

  lastPollMs = now;

  pollOnce();

}



CompanionProtocol &CompanionHttp::protocol() { return s_protocol; }



CompanionPairState CompanionHttp::getPairState() {

  if (!s_active) return CompanionPairState::WifiDown;

  if (!netLink.isConnected()) return CompanionPairState::WifiDown;

  if (s_token.length() == 0) return s_state;

  if (s_protocol.isHostReady()) return CompanionPairState::Ready;

  return s_state;

}



bool CompanionHttp::isConnected() {

  return s_protocol.isHostReady();

}



const char *CompanionHttp::getDeviceName() { return "Prolink Display"; }



const char *CompanionHttp::getMacString() { return s_macString; }



void CompanionHttp::restartPairing() {

  s_token = "";

  s_baseUrl = "";

  saveCredentials();

  s_protocol.resetSession();

  s_sse.disconnect();

  lastStatusSeq = 0;

  lastWaveformTransferId = "";

  lastSseAttemptMs = 0;

  sseReconnectBackoffMs = SSE_RECONNECT_MIN_MS;

  s_state = CompanionPairState::PendingApproval;

  registerClient();

}



static void postCmd(const char *actionType) {

  if (s_token.length() == 0 || s_baseUrl.length() == 0) return;



  StaticJsonDocument<192> body;

  body["id"] = String((uint32_t)esp_random(), HEX);

  JsonObject action = body["action"].to<JsonObject>();

  action["type"] = actionType;

  action["config"] = JsonObject();



  String payload;

  serializeJson(body, payload);

  HTTP::post(s_baseUrl + "/api/companion/v1/cmd", payload, "application/json", authHeaders(), 5000);

}



void CompanionHttp::sendPageNext() { postCmd("page-next"); }

void CompanionHttp::sendPagePrev() { postCmd("page-prev"); }

void CompanionHttp::sendWaveformRefresh() { postCmd("waveform.refresh"); }

