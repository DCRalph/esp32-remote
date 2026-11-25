#include "Commands.h"

#include <RemoteProtocol.h>

#include <atomic>

namespace
{
  enum DoorRequest : uint8_t
  {
    DoorNone,
    DoorLock,
    DoorUnlock,
  };

  // Written by any task (the web server too), consumed only by doorLoop.
  std::atomic<uint8_t> doorRequest{DoorNone};
  uint32_t doorPulseStartMs = 0;
  bool doorPulsing = false;
}

void lockDoor() { doorRequest.store(DoorLock); }
void unlockDoor() { doorRequest.store(DoorUnlock); }

void doorLoop()
{
  const uint8_t request = doorRequest.exchange(DoorNone);
  if (request != DoorNone)
  {
    relay7.Off();
    relay8.Off();
    (request == DoorLock ? relay7 : relay8).On();
    doorPulseStartMs = millis();
    doorPulsing = true;
  }
  else if (doorPulsing && millis() - doorPulseStartMs >= 500)
  {
    relay7.Off();
    relay8.Off();
    doorPulsing = false;
  }
}

namespace
{
  using namespace RemoteProtocol::CarRelays;

  GpIO *const relays[kRelayCount] = {&relay1, &relay2, &relay3, &relay4, &relay5, &relay6};
  bool *const busy[kRelayCount] = {&relay1Busy, &relay2Busy, &relay3Busy, &relay4Busy, &relay5Busy, &relay6Busy};

  Relays readRelays()
  {
    Relays out{};
    for (uint8_t i = 0; i < kRelayCount; i++)
      if (relays[i]->read())
        out.mask |= 1u << i;
    return out;
  }
}

void registerCommands(void (*onCommand)())
{
  lock().onReceive([onCommand](const TransportAddress &, const Lock &msg)
                   {
                     Serial.println(msg.unlock ? "Unlocking Doors" : "Locking Doors");
                     if (msg.unlock)
                       unlockDoor();
                     else
                       lockDoor();
                     onCommand(); });

  // A busy relay (flashing) keeps its state; the reply always carries the real one.
  set().onRequest([onCommand](const TransportAddress &, const RelaySet &req, Relays &resp)
                  {
                    if (req.index < kRelayCount && !*busy[req.index])
                    {
                      Serial.printf("Relay %u %s\n", req.index + 1, req.on ? "ON" : "OFF");
                      if (req.on)
                        relays[req.index]->On();
                      else
                        relays[req.index]->Off();
                    }
                    resp = readRelays();
                    onCommand();
                    return true; });

  get().onRequest([onCommand](const TransportAddress &, const RelaysGet &, Relays &resp)
                  {
                    resp = readRelays();
                    onCommand();
                    return true; });

  flash().onReceive([onCommand](const TransportAddress &, const Flash &msg)
                    {
                      if (relay1Busy)
                      {
                        Serial.println("Relay 1 Busy");
                        return;
                      }
                      Serial.println("Relay 1 Flash");
                      relay1Busy = true;
                      relay1FlashCount = msg.count;
                      relay1FlashDelay = msg.intervalMs;
                      onCommand(); });
}
