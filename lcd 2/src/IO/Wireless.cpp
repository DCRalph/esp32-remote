#include "Wireless.h"
#include <cstring>
#include <utility>

#ifndef DEBUG_ESP_NOW
#define DEBUG_ESP_NOW 0
#endif

#ifndef ESP_NOW_CHANNEL
#define ESP_NOW_CHANNEL 1
#endif

namespace
{
  bool isBroadcastMac(const uint8_t *mac)
  {
    static constexpr uint8_t kBroadcastMac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    return memcmp(mac, kBroadcastMac, ESP_NOW_ETH_ALEN) == 0;
  }
} // namespace


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
  {
    return;
  }
  if (incomingQueue_ == nullptr)
  {
    return;
  }

  WirelessFrame frame{};
  while (xQueueReceive(incomingQueue_, &frame, 0) == pdTRUE)
  {
    if (receiveCb)
    {
      TransportAddress source = TransportAddress::fromMac(frame.mac);
      receiveCb(source, frame.packet);
    }

    // Call a type-specific callback if available, otherwise the generic one.
    auto it = onReceiveForCallbacks.find(frame.packet.type);
    if (it != onReceiveForCallbacks.end())
    {
      it->second(&frame);
    }
    else if (onReceiveOtherCb)
    {
      onReceiveOtherCb(&frame);
    }
  }
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

  lastStatus_.store(status, std::memory_order_relaxed);
}

void Wireless::recvCallback(const uint8_t *mac_addr, const uint8_t *data,
                            uint16_t len)
{
  if (incomingQueue_ == nullptr || len < sizeof(uint16_t) * 2)
  {
    return;
  }

  const TransportPacket *p = reinterpret_cast<const TransportPacket *>(data);
  if (p->len > sizeof(p->data))
  {
    return;
  }
  if (len < static_cast<uint16_t>(sizeof(p->type) + sizeof(p->len) + p->len))
  {
    return;
  }

  WirelessFrame frame{};
  memcpy(frame.mac, mac_addr, ESP_NOW_ETH_ALEN);
  frame.direction = PacketDirection::RECV;
  frame.packet.type = p->type;
  frame.packet.len = p->len;
  if (p->len > 0)
  {
    memcpy(frame.packet.data, p->data, p->len);
  }

  if (xQueueSendToBack(incomingQueue_, &frame, 0) != pdTRUE)
  {
    droppedRxFrames_.fetch_add(1, std::memory_order_relaxed);
  }
}

void Wireless::setOnReceiveOther(std::function<void(WirelessFrame *frame)> cb)
{
  onReceiveOtherCb = cb;
}

void Wireless::addOnReceiveFor(uint16_t type,
                               std::function<void(WirelessFrame *frame)> cb)
{
  onReceiveForCallbacks[type] = cb;
}

void Wireless::removeOnReceiveFor(uint16_t type)
{
  onReceiveForCallbacks.erase(type);
}

int Wireless::send(const TransportPacket *p, const uint8_t *peer_addr)
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

  const bool isBroadcast = isBroadcastMac(peer_addr);
  bool removePeerAfterSend = !isBroadcast;
  if (!isBroadcast)
  {
    if (!esp_now_is_peer_exist(peer_addr))
    {
      esp_err_t addErr = esp_now_add_peer(&peerInfo);
      if (addErr != ESP_OK && addErr != ESP_ERR_ESPNOW_EXIST)
      {
        Serial.printf("Failed to add peer, error: %d\n", addErr);
        return -1;
      }
    }
  }
#if DEBUG_ESP_NOW == 1
  Serial.println("Peer added");
#endif

  esp_err_t err = esp_now_send(peerInfo.peer_addr, data, len);
  if (err != ESP_OK)
  {
    Serial.printf("Failed to send data, error: %d\n", err);
    if (removePeerAfterSend)
    {
      esp_now_del_peer(peer_addr);
    }
    return -1;
  }
#if DEBUG_ESP_NOW == 1
  Serial.println("Data sent");
#endif

  if (removePeerAfterSend)
  {
    esp_now_del_peer(peer_addr);
  }

#if DEBUG_ESP_NOW == 1
  Serial.println("######################");
#endif

  return 0;
}

int Wireless::send(WirelessFrame *frame)
{
  if (frame->direction == PacketDirection::SEND)
  {
    return send(&frame->packet, frame->mac);
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

  if (incomingQueue_ == nullptr)
  {
    incomingQueue_ = xQueueCreate(kRxQueueDepth, sizeof(WirelessFrame));
    if (incomingQueue_ == nullptr)
    {
      Serial.println("Failed to create ESP-NOW RX queue");
      esp_now_deinit();
      return false;
    }
  }
  droppedRxFrames_.store(0, std::memory_order_relaxed);

  // Note: keep global callback trampoline for ESPNOW C callbacks.
  esp_now_register_send_cb([](const uint8_t *mac_addr,
                              esp_now_send_status_t status)
                           { wireless.sendCallback(mac_addr, status); });

  esp_now_register_recv_cb([](const uint8_t *mac_addr,
                              const uint8_t *data,
                              int len)
                           { wireless.recvCallback(mac_addr, data, static_cast<uint16_t>(len)); });

  esp_now_peer_info_t broadcastPeer{};
  memcpy(broadcastPeer.peer_addr, TransportAddress::broadcast().bytes.data(), ADDRESS_LENGTH);
  broadcastPeer.channel = ESP_NOW_CHANNEL;
  broadcastPeer.encrypt = false;
  broadcastPeer.ifidx = WIFI_IF_STA;
  esp_err_t broadcastAddErr = esp_now_add_peer(&broadcastPeer);
  broadcastPeerConfigured_ = (broadcastAddErr == ESP_OK || broadcastAddErr == ESP_ERR_ESPNOW_EXIST);
  if (!broadcastPeerConfigured_)
  {
    Serial.printf("Failed to add broadcast peer, error: %d\n", broadcastAddErr);
  }

  setupDone = true;
  return true;
}

void Wireless::end()
{
  if (!setupDone)
  {
    return;
  }
  if (broadcastPeerConfigured_)
  {
    esp_now_del_peer(TransportAddress::broadcast().bytes.data());
    broadcastPeerConfigured_ = false;
  }
  esp_now_unregister_recv_cb();
  esp_now_unregister_send_cb();
  esp_now_deinit();
  if (incomingQueue_ != nullptr)
  {
    vQueueDelete(incomingQueue_);
    incomingQueue_ = nullptr;
  }
  setupDone = false;
}

bool Wireless::isReady() const
{
  
  return setupDone;
}

esp_now_send_status_t Wireless::getLastStatus() const
{
  return lastStatus_.load(std::memory_order_relaxed);
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