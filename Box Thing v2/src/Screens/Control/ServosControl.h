#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"
#include "IO/ScreenManager.h"
#include "IO/Menu.h"

struct ServoDataStruct {
  uint8_t manual;
  uint8_t servoX;
  uint8_t servoY;
};

class ServoControlScreen : public Screen {
public:
  ServoControlScreen(String _name);

  // MAC address to transmit to: 30:30:F9:2A:05:20
  uint8_t peer_addr[6] = {0x30, 0x30, 0xF9, 0x2A, 0x05, 0x20};

  uint64_t lastSend = 0;

  // Variables for servo control
  bool manual = false;
  uint8_t servoX = 90; // Default mid-point
  uint8_t servoY = 90; // Default mid-point

  Menu menu = Menu();

  MenuItemBack backItem;

  // Menu items for user interaction (if needed)
  MenuItemToggle manualToggle = MenuItemToggle("Manual", &manual, true);
  MenuItemNumber<uint8_t> servoXItem =  MenuItemNumber<uint8_t>("X", &servoX, 0, 180, 10);
  MenuItemNumber<uint8_t> servoYItem =  MenuItemNumber<uint8_t>("Y", &servoY, 0, 180, 10);

  void draw() override;
  void update() override;
  void onEnter() override;
  void onExit() override;
};

ServoControlScreen::ServoControlScreen(String _name) : Screen(_name) {

  // Add menu items to the menu
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&manualToggle);
  menu.addMenuItem(&servoXItem);
  menu.addMenuItem(&servoYItem);

}

void ServoControlScreen::draw() {
  menu.draw();
}

void ServoControlScreen::update() {

  menu.update();

  if (millis() - lastSend > 10) {
    lastSend = millis();

    // Create payload with current values
    ServoDataStruct servoData;
    servoData.manual = manual ? 1 : 0;
    servoData.servoX = servoX;
    servoData.servoY = servoY;

    // Prepare a fullPacket for transmission
    fullPacket fp;
    memcpy(fp.mac, peer_addr, 6);
    fp.direction = PacketDirection::SEND;
    fp.p.type = 0xB1; // Use a unique type identifier for servo data
    fp.p.len = sizeof(ServoDataStruct);

    // Copy servo data into packet payload
    memcpy(fp.p.data, &servoData, sizeof(ServoDataStruct));

    // Send the packet
    wireless.send(&fp);
  }
}

void ServoControlScreen::onEnter() {
  // Initialize or reset values as needed when entering the screen.
  manual = false;
  servoX = 90;
  servoY = 90;
}

void ServoControlScreen::onExit() {
  // Cleanup if necessary when exiting the screen.
}
