#pragma once

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <array>
#include <cstdint>
#include <functional>
#include <map>

// ============================================================
// Transport primitives (shared by Wireless + Mesh)
// ============================================================

struct TransportAddress
{
  std::array<uint8_t, 6> bytes{};

  static constexpr TransportAddress broadcast()
  {
    return TransportAddress{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
  }
};

struct __attribute__((packed)) TransportPacket
{
  uint16_t type = 0;
  uint16_t len = 0;
  uint8_t data[250]{};
};

class ITransport
{
public:
  using ReceiveCallback = std::function<void(const TransportAddress &source, const TransportPacket &packet)>;

  virtual ~ITransport() = default;
  virtual bool begin() = 0;
  virtual void end() = 0;
  virtual bool isReady() const = 0;
  virtual int sendPacket(const TransportPacket &packet, const TransportAddress &peer) = 0;
  virtual void setReceiveCallback(ReceiveCallback cb) = 0;
};

using data_packet = TransportPacket;

// ============================================================
// Packet wrappers used by existing sync code
// ============================================================

enum class PacketDirection
{
  SEND,
  RECV
};

struct fullPacket
{
  uint8_t mac[6];
  PacketDirection direction;
  data_packet p;
};

extern uint8_t BROADCAST_MAC[6];

// ============================================================
// ESP-NOW transport implementation
// ============================================================

class Wireless : public ITransport
{
private:
  bool setupDone = false;

  std::function<void(fullPacket *fp)> onReceiveOtherCb;
  std::map<uint16_t, std::function<void(fullPacket *fp)>> onReceiveForCallbacks;
  ReceiveCallback receiveCb;

public:
  esp_now_send_status_t lastStatus = ESP_NOW_SEND_FAIL;

  Wireless();
  void setup();   // Backward-compatible alias for begin()
  void unSetup(); // Backward-compatible alias for end()
  void loop();

  bool isSetupDone() const;

  void sendCallback(const uint8_t *mac_addr,
                    esp_now_send_status_t status);
  void recvCallback(const uint8_t *mac_addr, const uint8_t *data, uint16_t len);

  // Set the generic "other" callback.
  void setOnReceiveOther(std::function<void(fullPacket *fp)> cb);
  // Register a type-specific callback.
  void addOnReceiveFor(uint16_t type,
                       std::function<void(fullPacket *fp)> cb);
  // Remove a type-specific callback.
  void removeOnReceiveFor(uint16_t type);

  int send(const data_packet *p, const uint8_t *peer_addr);
  int send(const uint8_t *data, uint16_t len, const uint8_t *peer_addr);

  int send(fullPacket *fp);

  // ITransport
  bool begin() override;
  void end() override;
  bool isReady() const override;
  int sendPacket(const TransportPacket &packet, const TransportAddress &peer) override;
  void setReceiveCallback(ReceiveCallback cb) override;
};

extern Wireless wireless;