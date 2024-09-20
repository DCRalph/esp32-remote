#include "Server.h"

AsyncWebServer server(80);

static void processSetRelays(AsyncWebServerRequest *request, uint8_t relay, bool state)
{
  if (state)
    relayMap[relay]->On();
  else
    relayMap[relay]->Off();

  JsonDocument doc;
  doc["type"] = "success";
  doc["message"] = "Relay " + String(relay + 1) + (state ? " ON" : " OFF");

  // Serialize the JSON document to a string
  String response;
  serializeJson(doc, response);

  // Send the JSON response with the appropriate content type
  request->send(200, "application/json", response);
}

static void processGetRelays(AsyncWebServerRequest *request, uint8_t relay)
{
  JsonDocument doc;
  doc["type"] = "success";
  doc["message"] = "Relay " + String(relay + 1) + " State";
  doc["state"] = relayMap[relay]->read();

  // Serialize the JSON document to a string
  String response;
  serializeJson(doc, response);

  // Send the JSON response with the appropriate content type
  request->send(200, "application/json", response);
}

void setupServer()
{
  server.begin();
}

void setupRoutes()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/css", index_css); });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/javascript", index_js); });

              server.on("/keypad.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/javascript", keypad_js); });

  server.on("/lock", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              lockDoor();
              // blinks = 4;
              // ledBlinkMs = millis();

              JsonDocument doc;

              doc["type"] = "success";
              doc["message"] = "Doors Locked";
              String response = doc.as<String>();

              request->send(200, "text/plain", response);
              //
            });

  server.on("/unlock", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              unlockDoor();
              // blinks = 4;
              // ledBlinkMs = millis();

              JsonDocument doc;
              doc["type"] = "success";
              doc["message"] = "Doors Unlocked";
              String response = doc.as<String>();

              request->send(200, "text/plain", response);
              //
            });

  server.on("/ron1", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 0, true); });

  server.on("/ron2", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 1, true); });

  server.on("/ron3", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 2, true); });

  server.on("/ron4", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 3, true); });

  server.on("/ron5", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 4, true); });

  server.on("/ron6", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 5, true); });

  server.on("/roff1", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 0, false); });

  server.on("/roff2", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 1, false); });

  server.on("/roff3", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 2, false); });

  server.on("/roff4", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 3, false); });

  server.on("/roff5", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 4, false); });

  server.on("/roff6", HTTP_POST, [](AsyncWebServerRequest *request)
            { processSetRelays(request, 5, false); });

  server.on("/rget1", HTTP_GET, [](AsyncWebServerRequest *request)
            { processGetRelays(request, 0); });

  server.on("/rget2", HTTP_GET, [](AsyncWebServerRequest *request)
            { processGetRelays(request, 1); });

  server.on("/rget3", HTTP_GET, [](AsyncWebServerRequest *request)
            { processGetRelays(request, 2); });

  server.on("/rget4", HTTP_GET, [](AsyncWebServerRequest *request)
            { processGetRelays(request, 3); });

  server.on("/rget5", HTTP_GET, [](AsyncWebServerRequest *request)
            { processGetRelays(request, 4); });

  server.on("/rget6", HTTP_GET, [](AsyncWebServerRequest *request)
            { processGetRelays(request, 5); });
}
