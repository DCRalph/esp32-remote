
#include "secrets.h"

#include "config.h"
#include "IO/Server.h"

#include "IO/Wireless.h"

#include "IO/Commands.h"

uint64_t ledBlinkMs = 0;
uint64_t relay1FlashMs = 0;

uint8_t blinks = 0;

void espNowRecvCb(fullPacket *fp)
{
  int ret = parseCommand(fp);

  blinks = ret == -1 ? 2 : 4;

  ledBlinkMs = millis();
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  WiFi.softAP(WIFI_SSID, WIFI_PASS, CHANNEL, 0);
  WiFi.softAPConfig(IPAddress(10, 104, 19, 1), IPAddress(10, 104, 19, 1), IPAddress(255, 255, 255, 0));

  wireless.setup();
  wireless.setRecvCb(espNowRecvCb);

  setupRoutes();
  setupServer();

  led.On();
  delay(500);
  led.Off();
}

void loop()
{

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
    return;
  }

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

  if (Serial.available() > 0)
  {
    String input = Serial.readString();
    while (Serial.available() > 0)
      input += Serial.readString();
    if (input == "reboot")
      ESP.restart();
    else if (input == "getip")
      Serial.println(WiFi.localIP());
    else if (input == "getmac")
      Serial.println(WiFi.macAddress());
    else if (input == "lock")
      lockDoor();
    else if (input == "unlock")
      unlockDoor();
    else
      Serial.println("[INFO] [SERIAL] " + input);
  }
}
