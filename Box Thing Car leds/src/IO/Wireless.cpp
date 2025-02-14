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
  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
#endif

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb([](const uint8_t *mac_addr,
                              esp_now_send_status_t status)
                           { wireless.sendCallback(mac_addr, status); });

  esp_now_register_recv_cb([](const uint8_t *mac_addr,
                              const uint8_t *data, int len)
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

void Wireless::sendCallback(const uint8_t *mac_addr,
                            esp_now_send_status_t status)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

#ifdef DEBUG_ESP_NOW
  Serial.println("########### Sent Packet ###########");
  Serial.print("Sent to: ");
  Serial.println(macStr);

  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS
                     ? "Delivery Success"
                     : "Delivery Fail");
  Serial.println("###################################");
#endif

  lastStatus = status;
}

void Wireless::recvCallback(const uint8_t *mac_addr,
                            const uint8_t *data, int len)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

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

  // If a type-specific callback exists, run it and do not run the generic
  // callback.
  auto it = onReceiveForCallbacks.find(fp.p.type);
  if (it != onReceiveForCallbacks.end())
  {
    it->second(&fp);
  }
  else if (onReceiveOtherCb)
  {
    onReceiveOtherCb(&fp);
  }
}

void Wireless::setOnReceiveOther(std::function<void(fullPacket *fp)> cb)
{
  onReceiveOtherCb = cb;
}

void Wireless::addOnReceiveFor(
    uint8_t type, std::function<void(fullPacket *fp)> cb)
{
  onReceiveForCallbacks[type] = cb;
}

void Wireless::removeOnReceiveFor(uint8_t type)
{
  onReceiveForCallbacks.erase(type);
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

  Serial.println("here0");
  delay(200);

  esp_now_peer_info_t peerInfo = esp_now_peer_info_t();
  memcpy(peerInfo.peer_addr, peer_addr, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;

  #ifdef DEBUG_ESP_NOW
  Serial.println("######################");
  Serial.print("Peer info: ");
  for (int i = 0; i < 6; i++)
  {
    Serial.print(peerInfo.peer_addr[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Channel: ");
  Serial.println(peerInfo.channel);
  Serial.print("Encrypt: ");
  Serial.println(peerInfo.encrypt);
  Serial.println("######################");
#endif

  Serial.println("here1");
  delay(200);

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("######################");
  Serial.println("Peer added");
#endif

Serial.println("here2");
delay(200);

  if (esp_now_send(peerInfo.peer_addr, data, len) != ESP_OK)
  {
    Serial.println("Failed to send data");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Data sent");
#endif

Serial.println("here3");
delay(200);

  if (esp_now_del_peer(peer_addr) != ESP_OK)
  {
    Serial.println("Failed to delete peer");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Peer deleted");
  Serial.println("######################");
#endif

Serial.println("here4");
delay(200);

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
