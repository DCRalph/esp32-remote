#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <functional>

#define ADDRESS_LENGTH 6

// ============================================================
// Transport primitives (shared by Wireless + Mesh)
// ============================================================

struct TransportAddress
{
  std::array<uint8_t, ADDRESS_LENGTH> bytes{};

  bool isFrom(const uint8_t *mac) const
  {
    return memcmp(bytes.data(), mac, bytes.size()) == 0;
  }

  static TransportAddress fromMac(const uint8_t *mac)
  {
    TransportAddress addr{};
    if (mac != nullptr)
    {
      memcpy(addr.bytes.data(), mac, ADDRESS_LENGTH);
    }
    return addr;
  }

  uint8_t *getMac()
  {
    return bytes.data();
  }

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
  virtual void loop() = 0;
  virtual bool isReady() const = 0;
  virtual int sendPacket(const TransportPacket &packet, const TransportAddress &peer) = 0;
  virtual void setReceiveCallback(ReceiveCallback cb) = 0;
};