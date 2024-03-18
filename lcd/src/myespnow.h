#pragma once

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

class MyEspnow
{
private:
  uint8_t peerMac[6];
  esp_now_peer_info_t peerInfo;

  std::function<void()> callback;

public:
  MyEspnow();

  void init();

  void setCallback(std::function<void()> _callback);

  void send(String data);
  void send(uint8_t *data, int len);

  void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
};

extern MyEspnow myEspnow;