#pragma once

#include "config.h"

struct __attribute__((packed)) data_packet
{
  uint8_t type;
  uint8_t len;
  uint8_t data[200];
};

enum class PacketDirection
{
  SEND,
  RECV 
};

struct fullPacket
{
  uint8_t mac[6];
  PacketDirection direction;
  data_packet p;
};

class Wireless
{
private:
  bool setupDone = false;

  std::function<void(fullPacket *fp)> recvCb;

public:
  esp_now_send_status_t lastStatus = ESP_NOW_SEND_FAIL;

  Wireless();
  void setup();
  void unSetup();
  void loop();

  bool isSetupDone();

  void sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
  void recvCallback(const uint8_t *mac_addr, const uint8_t *data, int len);

  void setRecvCb(std::function<void(fullPacket *fp)> cb);

  // void enable();
  // void disable();

  int send(data_packet *p, u8_t *peer_addr);
  int send(u8_t *data, size_t len, u8_t *peer_addr);

  int send(fullPacket *fp);
};

extern Wireless wireless;