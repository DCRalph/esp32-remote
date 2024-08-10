#include <Arduino.h>
#include "secrets.h"

#include <WiFi.h>
#include <esp_wifi.h>

#include <esp_now.h>

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

#include "html.h"

#define CHANNEL 1
#define RELAY1 6
#define RELAY2 8

AsyncWebServer server(80);

struct __attribute__((packed)) packet
{
  uint8_t type;
  uint8_t len;
  uint8_t data[200];
};
packet p;

void lockDoors()
{
  digitalWrite(RELAY1, 0);
  delay(300);
  digitalWrite(RELAY1, 1);

  Serial.println("Doors Locked");
}

void unlockDoors()
{
  digitalWrite(RELAY2, 0);
  delay(300);
  digitalWrite(RELAY2, 1);

  Serial.println("Doors Unlocked");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: ");
  Serial.println(macStr);
  Serial.print("Last Packet Recv Data: ");
  Serial.println(*data);
  Serial.println("");

  memcpy(&p, data, sizeof(packet));

  String dataS = "";

  for (int i = 0; i < p.len; i++)
  {
    dataS += (char)p.data[i];
  }

  Serial.print("Data: ");
  Serial.println("type: " + String(p.type) + " len: " + String(p.len));
  Serial.println("Data Hex: ");
  for (int i = 0; i < p.len; i++)
  {
    Serial.print(p.data[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");
  Serial.println("Data ASCII: ");
  Serial.println(dataS);

  digitalWrite(15, 1);

  if (p.type == 11)
  {
    if (p.data[0] == 0)
    {
      // digitalWrite(RELAY1, 0);
      // delay(300);
      // digitalWrite(RELAY1, 1);
      lockDoors();
    }
    else if (p.data[0] == 1)
    {
      // digitalWrite(RELAY2, 0);
      // delay(300);
      // digitalWrite(RELAY2, 1);
      unlockDoors();
    }
  }

  digitalWrite(15, 0);
}

void setup()
{
  Serial.begin(115200);

  pinMode(15, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, 1);
  digitalWrite(RELAY2, 1);

  digitalWrite(15, 1);
  delay(3000);
  digitalWrite(15, 0);

  WiFi.mode(WIFI_AP);
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

  // esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  // esp_wifi_set_promiscuous(false);

  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
