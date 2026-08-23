#pragma once

#include <Menu.h>
#include <Wireless.h>

#include "config.h"
#include "Screens/Screens.h"

struct ServoDataStruct
{
  uint8_t manual;
  uint8_t servoX;
  uint8_t servoY;
};

namespace
{
  // MAC address to transmit to: 30:30:F9:2A:05:20
  uint8_t gServoPeerAddr[6] = {0x30, 0x30, 0xF9, 0x2A, 0x05, 0x20};

  uint64_t gServoLastSend = 0;

  bool gServoManual = false;
  uint8_t gServoX = 90; // Default mid-point
  uint8_t gServoY = 90; // Default mid-point

  Menu servoMenu;

  MenuItemBack servoBackItem;
  MenuItemToggle servoManualToggle("Manual", &gServoManual, true);
  MenuItemNumber<uint8_t> servoXItem("X", &gServoX, 0, 180, 10);
  MenuItemNumber<uint8_t> servoYItem("Y", &gServoY, 0, 180, 10);

  [[maybe_unused]] const bool servoMenuBuilt = []()
  {
    servoMenu.addMenuItem(&servoBackItem);
    servoMenu.addMenuItem(&servoManualToggle);
    servoMenu.addMenuItem(&servoXItem);
    servoMenu.addMenuItem(&servoYItem);
    return true;
  }();

  void servoDraw()
  {
    servoMenu.draw();
  }

  void servoUpdate()
  {
    servoMenu.update();

    if (millis() - gServoLastSend <= 10)
      return;

    gServoLastSend = millis();

    const ServoDataStruct servoData{
        static_cast<uint8_t>(gServoManual ? 1 : 0),
        gServoX,
        gServoY,
    };

    wireless.sendTyped(0xB1, servoData, gServoPeerAddr);
  }

  void servoOnEnter()
  {
    gServoManual = false;
    gServoX = 90;
    gServoY = 90;
  }
}

const Screen2 ServoControlScreen2 = {
    .name = "Servos",
    .draw = servoDraw,
    .update = servoUpdate,
    .onEnter = servoOnEnter,
    .onExit = nullptr,
};
