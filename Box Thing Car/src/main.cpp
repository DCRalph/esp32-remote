#include <Arduino.h>
#include "secrets.h"

#include "config.h"

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

#include "html.h"
#include "IO/Wireless.h"

#include "IO/Commands.h"

#define CHANNEL 1

uint64_t ledBlinkMs = 0;
uint64_t relay1FlashMs = 0;

uint8_t blinks = 0;

AsyncWebServer server(80);

void espNowRecvCb(fullPacket *fp)
{
  int ret = parseCommand(fp);

  blinks = ret == -1 ? 2 : 4;

  ledBlinkMs = millis();

  // void *pvParameters = &ret;

  // if (blinkLedHandle != NULL)
  // {
  //   Serial.println("Deleting blinkLedHandle before creating new task");
  //   vTaskDelete(blinkLedHandle);
  // }

  // xTaskCreate([](void *pvParameters)
  //             {
  //               int ret = *(int *)pvParameters;
  //               int blinks = ret == -1 ? 1 : 2;
  //               for (int i = 0; i < blinks * 2; i++)
  //               {
  //                 led.Toggle();
  //                 delay(100);
  //               }

  //               vTaskDelete(NULL);
  //               //
  //             },
  //             "blink", 1000, pvParameters, 1, &blinkLedHandle);

  //
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  WiFi.softAP(WIFI_SSID, WIFI_PASS, CHANNEL, 1);
  WiFi.softAPConfig(IPAddress(10, 104, 19, 1), IPAddress(10, 104, 19, 1), IPAddress(255, 255, 255, 0));

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/lock", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              lockDoor();
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

  if (blinks > 0)
  {
    if (millis() - ledBlinkMs > 100)
    {
      ledBlinkMs = millis();
      led.Toggle();
      blinks--;
    }

    if (blinks == 0)
      led.Off();
  }

  if (relay1FlashCount > 0)
  {
    if (millis() - relay1FlashMs > relay1FlashDelay)
    {
      relay1FlashMs = millis();
      relay1.Toggle();
      if (relay1.read() == 0)
        relay1FlashCount--;
    }

    if (relay1FlashCount == 0)
    {
      relay1.Off();
      relay1Busy = false;
    }
  }

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
