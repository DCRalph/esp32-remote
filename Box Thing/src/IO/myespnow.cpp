#include "myespnow.h"

MyEspnow myEspnow;

MyEspnow::MyEspnow()
{
}

void MyEspnow::init()
{
  // 74:4d:bd:7b:93:6c
  uint8_t peerMac[] = {0x74, 0x4d, 0xbd, 0x7b, 0x93, 0x6c};

  memcpy(this->peerMac, peerMac, 6);

  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(peerMac, ESP_NOW_ROLE_SLAVE, 0, NULL, 0);

  esp_now_register_send_cb([](uint8_t *mac_addr, uint8_t status)
                           { myEspnow.OnDataSent(mac_addr, status); });

  esp_now_register_recv_cb([](uint8_t *mac_addr, uint8_t *data, uint8_t len)
                           { myEspnow.OnDataRecv(mac_addr, data, len); });
}

void MyEspnow::setCallback(std::function<void()> _callback)
{
  callback = _callback;
}

int MyEspnow::send(String data)
{
  return send((uint8_t *)data.c_str());
}

int MyEspnow::send(uint8_t *data)
{
  return esp_now_send(peerMac, (uint8_t *)&data, sizeof(data));
}

void MyEspnow::OnDataSent(uint8_t *mac_addr, uint8_t status)
{
  lastStatus = status;
  Serial.print("Last Packet Send Status: ");
  if (status == 0)
  {
    Serial.println("Delivery Success");
  }
  else
  {
    Serial.println("Delivery Fail");
  }
}

void MyEspnow::OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len)
{
  Serial.print("Last Packet Recv from: ");
  for (int i = 0; i < 6; i++)
  {
    Serial.print(mac_addr[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
  Serial.print(" Data: ");
  for (int i = 0; i < len; i++)
  {
    Serial.print((char)data[i]);
  }
  Serial.println();
}