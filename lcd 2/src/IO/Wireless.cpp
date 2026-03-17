#include "Wireless.h"
#include <cstring>
#include <utility>

#ifndef DEBUG_ESP_NOW
#define DEBUG_ESP_NOW 0
#endif

#ifndef ESP_NOW_CHANNEL
#define ESP_NOW_CHANNEL 1
#endif

uint8_t BROADCAST_MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

Wireless::Wireless()
{
}

void Wireless::setup()
{
  begin();
}

void Wireless::unSetup()
{
  end();
}

void Wireless::loop()
{
  if (!setupDone)
    return;
}

bool Wireless::isSetupDone() const
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

#if DEBUG_ESP_NOW == 1
  Serial.println("########### Sent Packet ###########");
  Serial.printf("Sent to: %s\n", macStr);
  Serial.printf("Send Status: %s\n",
                status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("###################################");
#endif

  lastStatus = status;
}

void Wireless::recvCallback(const uint8_t *mac_addr, const uint8_t *data,
                            uint16_t len)
{
  if (len < sizeof(uint16_t) * 2)
  {
    return;
  }

  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

  const data_packet *p = reinterpret_cast<const data_packet *>(data);
  if (p->len > sizeof(p->data))
  {
    return;
  }
  if (receiveCb)
  {
    TransportAddress source{};
    memcpy(source.bytes.data(), mac_addr, ESP_NOW_ETH_ALEN);
    TransportPacket packet{};
    packet.type = p->type;
    packet.len = p->len;
    if (packet.len > 0)
    {
      memcpy(packet.data, p->data, packet.len);
    }
    receiveCb(source, packet);
  }

  if (len < static_cast<uint16_t>(sizeof(p->type) + sizeof(p->len) + p->len))
  {
    return;
  }

#if DEBUG_ESP_NOW == 1
  Serial.println("########### Received Packet ###########");
  Serial.printf("Recv from: %s\n", macStr);
  Serial.printf("Type: %d\n", p->type);
  Serial.printf("Len: %d\n", p->len);

  // Build the data string using pointer arithmetic for speed.
  char dataStr[256];
  int pos = 0;
  for (int i = 0; i < p->len && pos < (int)sizeof(dataStr); i++)
  {
    pos += snprintf(dataStr + pos, sizeof(dataStr) - pos, "%02X ", p->data[i]);
  }
  Serial.printf("Data: %s\n", dataStr);
  Serial.println("#######################################");
#endif

  fullPacket fp;
  memset(&fp, 0, sizeof(fp));
  memcpy(fp.mac, mac_addr, ESP_NOW_ETH_ALEN);
  fp.direction = PacketDirection::RECV;
  fp.p.type = p->type;
  fp.p.len = p->len;
  if (p->len > 0)
  {
    memcpy(fp.p.data, p->data, p->len);
  }

  // Call a type-specific callback if available, otherwise the generic one
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

void Wireless::addOnReceiveFor(uint16_t type,
                               std::function<void(fullPacket *fp)> cb)
{
  onReceiveForCallbacks[type] = cb;
}

void Wireless::removeOnReceiveFor(uint16_t type)
{
  onReceiveForCallbacks.erase(type);
}

int Wireless::send(const data_packet *p, const uint8_t *peer_addr)
{
  const uint16_t payloadLen = p->len > sizeof(p->data) ? sizeof(p->data) : p->len;
  const uint16_t frameLen = static_cast<uint16_t>(sizeof(p->type) + sizeof(p->len) + payloadLen);
  return send(reinterpret_cast<const uint8_t *>(p), frameLen, peer_addr);
}

int Wireless::send(const uint8_t *data, uint16_t len, const uint8_t *peer_addr)
{
  if (!setupDone)
  {
    Serial.println("ESP-NOW not initialized");
    return -1;
  }

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, peer_addr, ESP_NOW_ETH_ALEN);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;
  // Explicitly select the WiFi interface (typically WIFI_IF_STA)
  peerInfo.ifidx = WIFI_IF_STA;

#if DEBUG_ESP_NOW == 1
  Serial.println("######################");
  char peerStr[32];
  int pos = 0;
  for (int i = 0; i < ESP_NOW_ETH_ALEN; i++)
  {
    pos += snprintf(peerStr + pos, sizeof(peerStr) - pos, "%02X ",
                    peerInfo.peer_addr[i]);
  }
  Serial.printf("Peer info: %s\n", peerStr);
  Serial.printf("Channel: %d\n", peerInfo.channel);
  Serial.printf("Encrypt: %d\n", peerInfo.encrypt);
  uint16_t dataLen = data[1];
  Serial.printf("Data:\n");
  for (uint16_t i = 0; i < dataLen + 2; i++)
  {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println("\n######################");
#endif

  bool removePeerAfterSend = false;
  esp_err_t err = esp_now_add_peer(&peerInfo);
  if (err == ESP_OK)
  {
    removePeerAfterSend = true;
  }
  else if (err != ESP_ERR_ESPNOW_EXIST)
  {
    Serial.printf("Failed to add peer, error: %d\n", err);
    return -1;
  }
#if DEBUG_ESP_NOW == 1
  Serial.println("Peer added");
#endif

  err = esp_now_send(peerInfo.peer_addr, data, len);
  if (err != ESP_OK)
  {
    Serial.printf("Failed to send data, error: %d\n", err);
    if (removePeerAfterSend)
    {
      // Attempt to clean up even if sending fails.
      esp_now_del_peer(peer_addr);
    }
    return -1;
  }
#if DEBUG_ESP_NOW == 1
  Serial.println("Data sent");
#endif

  if (removePeerAfterSend)
  {
    err = esp_now_del_peer(peer_addr);
    if (err != ESP_OK)
    {
      Serial.printf("Failed to delete peer, error: %d\n", err);
      return -1;
    }
  }
#if DEBUG_ESP_NOW == 1
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

bool Wireless::begin()
{
  if (setupDone)
  {
    return true;
  }

#ifndef ESPNOW_NO_DISABLE_WIFI
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
#endif

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return false;
  }

  // Note: keep global callback trampoline for ESPNOW C callbacks.
  esp_now_register_send_cb([](const uint8_t *mac_addr,
                              esp_now_send_status_t status)
                           { wireless.sendCallback(mac_addr, status); });

  esp_now_register_recv_cb([](const uint8_t *mac_addr,
                              const uint8_t *data,
                              int len)
                           { wireless.recvCallback(mac_addr, data, static_cast<uint16_t>(len)); });

  setupDone = true;
  return true;
}

void Wireless::end()
{
  if (!setupDone)
  {
    return;
  }

  setupDone = false;
  esp_now_deinit();
  esp_now_unregister_recv_cb();
  esp_now_unregister_send_cb();
}

bool Wireless::isReady() const
{
  return setupDone;
}

int Wireless::sendPacket(const TransportPacket &packet, const TransportAddress &peer)
{
  return send(&packet, peer.bytes.data());
}

void Wireless::setReceiveCallback(ReceiveCallback cb)
{
  receiveCb = std::move(cb);
}

Wireless wireless;