#pragma once

#include <esp_now.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <WiFi.h>
#include <array>
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>

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

private:
  std::atomic<esp_now_send_status_t> lastStatus_{ESP_NOW_SEND_FAIL};
};

extern Wireless wireless;