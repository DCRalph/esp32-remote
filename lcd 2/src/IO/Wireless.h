#pragma once

#include "config.h"
#include "ScreenManager.h"

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

  void sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
  void recvCallback(const uint8_t *mac_addr, const uint8_t *data, int len);

  // void enable();
  // void disable();

  int send(packet *p, u8_t *peer_addr);
  int send(u8_t *data, size_t len, u8_t *peer_addr);
};

extern Wireless wireless;