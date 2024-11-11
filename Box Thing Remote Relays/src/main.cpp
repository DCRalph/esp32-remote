
#include "secrets.h"
#include "config.h"
#include "IO/Wireless.h"
#include "IO/Commands.h"
#include "IO/LCD.h"

#include "Screens/HomeScreen.h"

bool lastArmedSW = false;
uint64_t lastLedShow = 0;

void espNowRecvCb(fullPacket *fp)
{
  int ret = parseCommand(fp);
}

void armedStateCB(ArmedState state)
{
  Serial.println("[INFO] [ARMED] State changed to " + ArmedStateMap.at(state));

  switch (state)
  {
  case ArmedState::ARMED:
    armedLed.SetMode(RGB_MODE::Manual);
    armedLed.SetColor(0, 255, 0);
    break;
  case ArmedState::READY:
    // armedLed.SetColor(255, 255, 0);
    armedLed.SetPulsingColor(255, 255, 0);
    armedLed.SetMode(RGB_MODE::Pulsing);
    break;
  case ArmedState::DISARMED:
    armedLed.SetMode(RGB_MODE::Manual);
    armedLed.SetColor(0, 0, 0);
    break;
  case ArmedState::LOCKED:
    armedLed.SetMode(RGB_MODE::Manual);
    armedLed.SetColor(255, 0, 0);
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();
  lcd.init();

  armed.setOnStateChange(armedStateCB);

  wireless.setup();
  wireless.setRecvCb(espNowRecvCb);

  pwrLed.SetColor(255, 0, 0);

  lcd.setScreen(&HomeScreen);
}

void loop()
{
  if (armedSW.read() != lastArmedSW)
  {
    lastArmedSW = armedSW.read();
    armed.setSW(armedSW.read());
  }

  if(millis() - lastRemotePing > 1000)
  {
    lastRemotePing = millis();
    remoteConnected = false;
    armed.setRemote(false);
  }

  armed.update();

  lcd.loop();

  if (millis() - lastLedShow > 10)
  {
    lastLedShow = millis();
    GpIO_RGB::showStrip();
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

    else if (input == "getall")
    {
      for (auto relay : relayMap)
      {
        Serial.print("Relay ");
        Serial.print(relay->getPin());
        Serial.print(" - ");
        Serial.println(relay->read());
      }
    }

    else if (input.startsWith("set"))
    {
      String pin = input.substring(3, 4);
      String value = input.substring(5, 6);
      int pinInt = pin.toInt();
      int valueInt = value.toInt();
      relayMap[pinInt]->Write(valueInt);
    }

    else if (input == "getarmed")
    {
      Serial.println(armed.getStateString());
    }

    else if (input == "test")
    {
      runTestProcedure();
    }

    else if (input == "fire")
    {
      bool relayToFire[8] = {true, true, true, true, true, true, true, true};
      fireRelays(relayToFire);
    }

    else
      Serial.println("[INFO] [SERIAL] " + input);
  }
}
