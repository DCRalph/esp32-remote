#pragma once

#include <esp_now.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <WiFi.h>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <type_traits>

#include "ITransport.h"

// ============================================================
// Wireless-layer frame (MAC + direction + payload)
// ============================================================

enum class PacketDirection
{
  SEND,
  RECV
};

struct WirelessFrame
{
  uint8_t mac[ESP_NOW_ETH_ALEN];
  PacketDirection direction;
  TransportPacket packet;
};

// ============================================================
// ESP-NOW transport implementation
// ============================================================

class Wireless : public ITransport
{
private:
  static constexpr uint8_t kRxQueueDepth = 16;
  bool setupDone = false;
  bool broadcastPeerConfigured_ = false;
  QueueHandle_t incomingQueue_ = nullptr;
  std::atomic<uint32_t> droppedRxFrames_{0};

  std::function<void(WirelessFrame *frame)> onReceiveOtherCb;
  std::map<uint16_t, std::function<void(WirelessFrame *frame)>> onReceiveForCallbacks;
  ReceiveCallback receiveCb;

public:
  Wireless();
  void setup();   // Backward-compatible alias for begin()
  void unSetup(); // Backward-compatible alias for end()
  void loop() override;

  bool isSetupDone() const;

  void sendCallback(const uint8_t *mac_addr,
                    esp_now_send_status_t status);
  void recvCallback(const uint8_t *mac_addr, const uint8_t *data, uint16_t len);

  // Set the generic "other" callback.
  void setOnReceiveOther(std::function<void(WirelessFrame *frame)> cb);
  // Register a type-specific callback.
  void addOnReceiveFor(uint16_t type,
                       std::function<void(WirelessFrame *frame)> cb);
  // Remove a type-specific callback.
  void removeOnReceiveFor(uint16_t type);

  int send(const TransportPacket *p, const uint8_t *peer_addr);
  int send(const uint8_t *data, uint16_t len, const uint8_t *peer_addr);

  int send(WirelessFrame *frame);

  // ITransport
  bool begin() override;
  void end() override;
  bool isReady() const override;
  esp_now_send_status_t getLastStatus() const;
  int sendPacket(const TransportPacket &packet, const TransportAddress &peer) override;
  void setReceiveCallback(ReceiveCallback cb) override;

  // --- Typed property/event helpers (trivially copyable structs, max 250-byte payload) ---

  template <typename T>
  int sendProperty(uint16_t key, const T &value, const uint8_t *peer_addr)
  {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Wireless property type must be trivially copyable");
    static_assert(sizeof(T) <= sizeof(TransportPacket{}.data),
                  "Wireless property payload exceeds TransportPacket::data");
    TransportPacket pkt{};
    pkt.type = key;
    pkt.len = static_cast<uint16_t>(sizeof(T));
    if (pkt.len > 0)
    {
      std::memcpy(pkt.data, &value, sizeof(T));
    }
    return send(&pkt, peer_addr);
  }

  template <typename T>
  int sendProperty(uint16_t key, const T &value, const TransportAddress &peer)
  {
    return sendProperty(key, value, peer.bytes.data());
  }

  template <typename T>
  int sendEvent(uint16_t channel, const T &value, const uint8_t *peer_addr)
  {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Wireless event payload type must be trivially copyable");
    static_assert(sizeof(T) <= sizeof(TransportPacket{}.data),
                  "Wireless event payload exceeds TransportPacket::data");
    TransportPacket pkt{};
    pkt.type = channel;
    pkt.len = static_cast<uint16_t>(sizeof(T));
    if (pkt.len > 0)
    {
      std::memcpy(pkt.data, &value, sizeof(T));
    }
    return send(&pkt, peer_addr);
  }

  template <typename T>
  int sendEvent(uint16_t channel, const T &value, const TransportAddress &peer)
  {
    return sendEvent(channel, value, peer.bytes.data());
  }

  int sendEvent(uint16_t channel, const uint8_t *peer_addr)
  {
    TransportPacket pkt{};
    pkt.type = channel;
    pkt.len = 0;
    return send(&pkt, peer_addr);
  }

  int sendEvent(uint16_t channel, const TransportAddress &peer)
  {
    return sendEvent(channel, peer.bytes.data());
  }

  template <typename T>
  static bool decodeProperty(uint16_t expectedKey, const TransportPacket &pkt, T &out)
  {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Wireless property type must be trivially copyable");
    if (pkt.type != expectedKey || pkt.len != sizeof(T))
    {
      return false;
    }
    if (sizeof(T) > 0)
    {
      std::memcpy(&out, pkt.data, sizeof(T));
    }
    return true;
  }

  template <typename T>
  static bool decodeProperty(uint16_t expectedKey, const WirelessFrame &frame, T &out)
  {
    return decodeProperty(expectedKey, frame.packet, out);
  }

  template <typename T>
  static bool decodeProperty(uint16_t expectedKey, const WirelessFrame *frame, T &out)
  {
    return frame != nullptr && decodeProperty(expectedKey, frame->packet, out);
  }

  template <typename T>
  static bool decodeEvent(uint16_t expectedChannel, const TransportPacket &pkt, T &out)
  {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Wireless event payload type must be trivially copyable");
    if (pkt.type != expectedChannel || pkt.len != sizeof(T))
    {
      return false;
    }
    if (sizeof(T) > 0)
    {
      std::memcpy(&out, pkt.data, sizeof(T));
    }
    return true;
  }

  template <typename T>
  static bool decodeEvent(uint16_t expectedChannel, const WirelessFrame &frame, T &out)
  {
    return decodeEvent(expectedChannel, frame.packet, out);
  }

  template <typename T>
  static bool decodeEvent(uint16_t expectedChannel, const WirelessFrame *frame, T &out)
  {
    return frame != nullptr && decodeEvent(expectedChannel, frame->packet, out);
  }

  static bool decodeEvent(uint16_t expectedChannel, const TransportPacket &pkt)
  {
    return pkt.type == expectedChannel && pkt.len == 0;
  }

  static bool decodeEvent(uint16_t expectedChannel, const WirelessFrame &frame)
  {
    return decodeEvent(expectedChannel, frame.packet);
  }

  static bool decodeEvent(uint16_t expectedChannel, const WirelessFrame *frame)
  {
    return frame != nullptr && decodeEvent(expectedChannel, frame->packet);
  }

  template <typename T>
  void addOnReceiveProperty(uint16_t key, std::function<void(const uint8_t *mac, const T &)> cb)
  {
    if (!cb)
    {
      return;
    }
    addOnReceiveFor(key, [cb = std::move(cb), key](WirelessFrame *frame)
                     {
                       T value{};
                       if (!decodeProperty(key, frame->packet, value))
                       {
                         return;
                       }
                       cb(frame->mac, value);
                     });
  }

  template <typename T>
  void addOnReceiveEvent(uint16_t channel, std::function<void(const uint8_t *mac, const T &)> cb)
  {
    if (!cb)
    {
      return;
    }
    addOnReceiveFor(channel, [cb = std::move(cb), channel](WirelessFrame *frame)
                      {
                        T value{};
                        if (!decodeEvent(channel, frame->packet, value))
                        {
                          return;
                        }
                        cb(frame->mac, value);
                      });
  }

  void addOnReceiveEvent(uint16_t channel, std::function<void(const uint8_t *mac)> cb)
  {
    if (!cb)
    {
      return;
    }
    addOnReceiveFor(channel, [cb = std::move(cb), channel](WirelessFrame *frame)
                     {
                       if (!decodeEvent(channel, frame->packet))
                       {
                         return;
                       }
                       cb(frame->mac);
                     });
  }

private:
  std::atomic<esp_now_send_status_t> lastStatus_{ESP_NOW_SEND_FAIL};
};

extern Wireless wireless;