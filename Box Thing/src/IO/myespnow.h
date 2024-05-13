#pragma once

#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>

struct __attribute__((packed)) packet
{
  uint8_t type;
  uint8_t len;
  uint8_t data[200];
};

class MyEspnow
{
private:
  uint8_t peerMac[6];

  std::function<void()> callback;

public:
  MyEspnow();

  void init();

  void setCallback(std::function<void()> _callback);

  int send(packet *p, u8_t *peer_addr);
  int send(u8_t *data, size_t len, u8_t *peer_addr);

  int send(String data);
  int send(uint8_t *data);

  uint8_t lastStatus;

  void OnDataSent(uint8_t *mac_addr, uint8_t status);
  void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len);
};

extern MyEspnow myEspnow;