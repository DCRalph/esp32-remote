
#include "secrets.h"
#include "config.h"
#include <RemoteProtocol.h>
#include <Wireless.h>

#include "IO/Commands.h"
#include "IO/LCD.h"
#include "IO/Battery.h"
#include "Timer.h"

#include "Screens/HomeScreen.h"

bool lastArmedSW = false;
uint64_t lastLedShow = 0;

unsigned long batteryLoopMs = 0;

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
  Serial.setTimeout(10); // readString() otherwise blocks the loop, and the link, for 1 s

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();
  lcd.init();

  armed.setOnStateChange(armedStateCB);

  Wireless::getInstance()->setup();
  RemoteProtocol::attachDirect();
  registerCommands();

  pwrLed.SetColor(255, 0, 0);

  lcd.setScreen(&HomeScreen);
}

uint32_t loopsPerSecond = 0;
uint64_t lastLoopPerSecondMs = 0;

void loop()
{
  // Before draining frames: a ping queued during a stalled loop must not hide a lost link.
  if (millis() - lastRemotePing > 1000)
  {
    lastRemotePing = millis();
    remoteConnected = false;
    armed.setRemote(false);
  }
  armed.update();

  // A frame is at most one drain interval old; past 300 ms treat the queue as stale.
  static uint32_t lastDrainMs = millis();
  rxStale = millis() - lastDrainMs > 300;
  lastDrainMs = millis();
  DirectComms::getInstance()->loop();
  Wireless::getInstance()->loop();

  loopsPerSecond++;

  if (millis() - lastLoopPerSecondMs > 1000)
  {
    lastLoopPerSecondMs = millis();
    Serial.println("Loops per second: " + String(loopsPerSecond));
    loopsPerSecond = 0;
  }

  if (millis() - batteryLoopMs > 500)
  {
    batteryLoopMs = millis();

    // Timer battery1Timer("Battery 1", Timer::TimerAction::AUTOSTART);
    batteryUpdate();
    // battery1Timer.print();

    // Timer battery2Timer("Battery 2", Timer::TimerAction::AUTOSTART);
    battery2Update();
    // battery2Timer.print();

#ifdef DEBUG_BATTERY
    // print pretty box with battery info
    Serial.println("---------------------------------");
    Serial.println("Battery 1:");
    Serial.print("Voltage: ");
    Serial.print(batteryGetVoltageSmooth());
    Serial.println("V");
    Serial.print("Percentage: ");
    Serial.print(batteryGetPercentageSmooth());
    Serial.println("%");
    Serial.println("---------------------------------");
    Serial.println("Battery 2:");
    Serial.print("Voltage: ");
    Serial.print(battery2GetVoltageSmooth());
    Serial.println("V");
    Serial.print("Percentage: ");
    Serial.print(battery2GetPercentageSmooth());
    Serial.println("%");
    Serial.println("---------------------------------");
#endif
  }

  if (armedSW.read() != lastArmedSW)
  {
    lastArmedSW = armedSW.read();
    armed.setSW(armedSW.read());
  }


  armed.update();

  // Timer lcdTimer("LCD", Timer::TimerAction::AUTOSTART);
  lcd.loop();
  // lcdTimer.print();

  if (millis() - lastLedShow > 5)
  {
    // lastLedShow = millis();
    // Timer ledTimer("LED", Timer::TimerAction::AUTOSTART);
    // GpIO_RGB::showStrip();
    // ledTimer.print();
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
      fireRelays(0xFF);
    }

    else
      Serial.println("[INFO] [SERIAL] " + input);
  }
}
