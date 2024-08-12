#include <Arduino.h>
#include "secrets.h"

#include "config.h"

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

#include "html.h"
#include "IO/Wireless.h"

#include "IO/Commands.h"

#define CHANNEL 1

AsyncWebServer server(80);

void lockDoors()
{
  // digitalWrite(RELAY1, 0);
  led.On();
  delay(300);
  // digitalWrite(RELAY1, 1);
  led.Off();

  Serial.println("Doors Locked");
}

void unlockDoors()
{
  // digitalWrite(RELAY2, 0);
  led.On();
  delay(300);
  // digitalWrite(RELAY2, 1);
  led.Off();

  Serial.println("Doors Unlocked");
}

void espNowRecvCb(fullPacket *fp)
{
  fullPacket resp;
  memcpy(resp.mac, fp->mac, 6);
  resp.direction = PacketDirection::SEND;
  resp.p.type = 0x11;
  resp.p.len = 1;
  resp.p.data[0] = 0;

  wireless.send(&resp);

  parseCommand(fp);

  led.On();
  delay(500);
  led.Off();
  //
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);

  GpIO::initIO();
  // WiFi.disconnect();

  WiFi.softAP(WIFI_SSID, WIFI_PASS, CHANNEL, 1);
  WiFi.softAPConfig(IPAddress(10, 104, 19, 1), IPAddress(10, 104, 19, 1), IPAddress(255, 255, 255, 0));

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/lock", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              digitalWrite(15, 1);
              lockDoors();
              digitalWrite(15, 0);

              JsonDocument doc;

              doc["type"] = "success";
              doc["message"] = "Doors Locked";
              String response = doc.as<String>();

              request->send(200, "text/plain", response);
              //
            });

  server.on("/unlock", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              digitalWrite(15, 1);
              unlockDoors();
              digitalWrite(15, 0);

              JsonDocument doc;
              doc["type"] = "success";
              doc["message"] = "Doors Unlocked";
              String response = doc.as<String>();

              request->send(200, "text/plain", response);
              //
            });

  server.begin();

  wireless.setup();

  wireless.setRecvCb(espNowRecvCb);

  led.On();
  delay(500);
  led.Off();
}

void loop()
{
  // uint8_t baseMac[6];
  // esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  // if (ret == ESP_OK)
  // {
  //   Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
  //                 baseMac[0], baseMac[1], baseMac[2],
  //                 baseMac[3], baseMac[4], baseMac[5]);
  // }
  // else
  // {
  //   Serial.println("Failed to read MAC address");
  // }

  // delay(1000);
}
