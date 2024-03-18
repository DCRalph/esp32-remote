#include "myespnow.h"

MyEspnow::MyEspnow()
{
}

void MyEspnow::init()
{
  uint8_t peerMac[] = {0x24, 0x0A, 0xC4, 0x00, 0x5E, 0x3A};
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status)
                           { myEspnow.OnDataSent(mac_addr, status); });

  esp_now_register_recv_cb([](const uint8_t *mac_addr, const uint8_t *data, int data_len)
                           { myEspnow.OnDataRecv(mac_addr, data, data_len); });
}

void MyEspnow::setCallback(std::function<void()> _callback)
{
  callback = _callback;
}

void MyEspnow::send(String data)
{
  send((uint8_t *)data.c_str(), data.length());
}

void MyEspnow::send(uint8_t *data, int len)
{
  esp_now_send(peerMac, data, len);
}

void MyEspnow::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("Last Packet Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Delivery Success");
  }
  else
  {
    Serial.println("Delivery Fail");
  }
}

void MyEspnow::OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  Serial.print("Last Packet Recv from: ");
  for (int i = 0; i < 6; i++)
  {
    Serial.print(mac_addr[i], HEX);
    if (i < 5)
      Serial.print(":");
  }
  Serial.print(" Data: ");
  for (int i = 0; i < data_len; i++)
  {
    Serial.print((char)data[i]);
  }
  Serial.println();
}

MyEspnow myEspnow;