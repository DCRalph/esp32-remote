#include "config.h"
#include "IO/Wireless.h"

uint64_t ledBlinkMs = 0;
uint8_t blinks = 0;

void espNowRecvCb(fullPacket *fp)
{

  blinks = 4;
  ledBlinkMs = millis();
}

void setup()
{
  Serial.begin(115200);

  delay(200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  wireless.setup();
  wireless.setOnReceiveOther(espNowRecvCb);
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
    else
      Serial.println("[INFO] [SERIAL] " + input);
  }

  Serial.println("Hello World");
  delay(1000);
}
