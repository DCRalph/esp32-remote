#include "Wireless.h"

Wireless::Wireless()
{
}

void Wireless::setup()
{
#ifndef ESPNOW_NO_DISABLE_WIFI
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
#endif

#ifndef ESPNOW_NO_DISABLE_WIFI
  esp_wifi_set_promiscuous(true);
#endif
  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
#ifndef ESPNOW_NO_DISABLE_WIFI
  esp_wifi_set_promiscuous(false);
#endif

  delay(100);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status)
                           { wireless.sendCallback(mac_addr, status); });

  esp_now_register_recv_cb([](const uint8_t *mac_addr, const uint8_t *data, int len)
                           { wireless.recvCallback(mac_addr, data, len); });

  setupDone = true;
}

void Wireless::unSetup()
{
  setupDone = false;
  esp_now_deinit();

  esp_now_unregister_recv_cb();
  esp_now_unregister_send_cb();
}

void Wireless::loop()
{
  if (!setupDone)
    return;
}

bool Wireless::isSetupDone()
{
  return setupDone;
}

void Wireless::sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

#ifdef DEBUG_ESP_NOW
  Serial.println("########### Sent Packet ###########");
  Serial.print("Sent to: ");
  Serial.println(macStr);

  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("###################################");
#endif

  lastStatus = status;
}

void Wireless::recvCallback(const uint8_t *mac_addr, const uint8_t *data, int len)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  data_packet *p = (data_packet *)data;

#ifdef DEBUG_ESP_NOW
  Serial.println("########### Received Packet ###########");
  Serial.print("Recv from: ");
  Serial.println(macStr);

  Serial.print("Type: ");
  Serial.println(p->type);

  Serial.print("Len: ");
  Serial.println(p->len);

  Serial.print("Data: ");
  for (int i = 0; i < p->len; i++)
  {
    Serial.print(p->data[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("#######################################");
#endif

  fullPacket fp;
  memcpy(fp.mac, mac_addr, 6);
  fp.direction = PacketDirection::RECV;
  memcpy(&fp.p, p, sizeof(data_packet));

  if (recvCb != nullptr)
    recvCb(&fp);
}

void Wireless::setRecvCb(std::function<void(fullPacket *fp)> cb)
{
  recvCb = cb;
}

int Wireless::send(data_packet *p, u8_t *peer_addr)
{
  return send((u8_t *)p, sizeof(data_packet), peer_addr);
}

int Wireless::send(u8_t *data, size_t len, u8_t *peer_addr)
{
  if (!setupDone)
  {
    Serial.println("ESP-NOW not initialized");
    return -1;
  }

  esp_now_peer_info_t peerInfo = esp_now_peer_info_t();

  memcpy(peerInfo.peer_addr, peer_addr, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("######################");
  Serial.println("Peer added");
#endif

  if (esp_now_send(peerInfo.peer_addr, data, len) != ESP_OK)
  {
    Serial.println("Failed to send data");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Data sent");
#endif

  if (esp_now_del_peer(peer_addr) != ESP_OK)
  {
    Serial.println("Failed to delete peer");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Peer deleted");
  Serial.println("######################");
#endif

  return 0;
}

int Wireless::send(fullPacket *fp)
{
  if (fp->direction == PacketDirection::SEND)
  {
    return send(&fp->p, fp->mac);
  }
  else
  {
    Serial.println("Cannot send a receive packet");
    return -1;
  }
}

Wireless wireless;