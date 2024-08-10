#pragma once

#include "config.h"
#include <espnow.h>
#include <ESP8266WiFi.h>

#define ESP_NOW_CHANNEL 1

struct __attribute__((packed)) packet
{
  uint8_t type;
  uint8_t len;
  uint8_t data[200];
};

class Wireless
{
private:
  bool setupDone = false;

public:
  Wireless();
  void setup();
  void unSetup();
  void loop();

  bool isSetupDone();

  void sendCallback(uint8_t *mac_addr, uint8_t status);
  uint8_t lastStatus;

  int send(packet *p, u8_t *peer_addr);
  int send(u8_t *data, size_t len, u8_t *peer_addr);
};

extern Wireless wireless;