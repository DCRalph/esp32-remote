#pragma once

#include <Direct.h>
#include <Wireless.h>

#include <cstdint>

// Point-to-point messages between the esp32-remote devices, as wireless_mesh
// DirectComms channels. Both ends include this header, so a link's ids and
// payloads cannot drift apart. Payloads are memcpy'd: keep them trivially copyable.
namespace RemoteProtocol
{
  // Feeds direct frames from Wireless into DirectComms through a packet-type hook,
  // leaving the transport's one receive callback to SyncManager. Wireless::end()
  // drops the hook, so call this after every Wireless begin.
  inline DirectComms &attachDirect()
  {
    DirectComms *direct = DirectComms::getInstance();
    Wireless *wireless = Wireless::getInstance();
    direct->setTransport(wireless);
    wireless->addOnReceiveFor(DirectFraming::kDirectPacketType,
                              [rx = direct->receiveCallback()](WirelessFrame *frame)
                              { rx(TransportAddress::fromMac(frame->mac), frame->packet); });
    return *direct;
  }

  // Box Thing v2 -> Box Thing Remote Relays. The board disarms itself when pings stop.
  namespace FireBox
  {
    struct Ping
    {
    };
    struct Pong
    {
    };
    struct Test
    {
    };
    struct Arm
    {
      uint8_t armed;
    };
    struct Fire
    {
      uint8_t relayMask; // bit n fires relay n + 1 for 100 ms
    };

    inline DirectRpc<Ping, Pong> ping() { return DirectComms::getInstance()->rpc<Ping, Pong>(0x0101); }
    inline DirectChannel<Test> test() { return DirectComms::getInstance()->channel<Test>(0x0102); }
    inline DirectChannel<Arm> arm() { return DirectComms::getInstance()->channel<Arm>(0x0103); }
    inline DirectChannel<Fire> fire() { return DirectComms::getInstance()->channel<Fire>(0x0104); }
  }

  // lcd 2, oled 3, oled 3 lvglAI -> Box Thing Car.
  namespace CarRelays
  {
    constexpr uint8_t kRelayCount = 6;

    struct Lock
    {
      uint8_t unlock; // 1 pulses the unlock relay, 0 the lock relay
    };
    struct RelaySet
    {
      uint8_t index; // 0-based, below kRelayCount
      uint8_t on;
    };
    struct RelaysGet
    {
    };
    struct Relays
    {
      uint8_t mask; // bit n is relay n + 1
    };
    struct Flash
    {
      uint8_t count;       // on/off cycles of relay 1
      uint16_t intervalMs; // per half cycle
    };

    inline DirectChannel<Lock> lock() { return DirectComms::getInstance()->channel<Lock>(0x0201); }
    inline DirectRpc<RelaySet, Relays> set() { return DirectComms::getInstance()->rpc<RelaySet, Relays>(0x0202); }
    inline DirectRpc<RelaysGet, Relays> get() { return DirectComms::getInstance()->rpc<RelaysGet, Relays>(0x0203); }
    inline DirectChannel<Flash> flash() { return DirectComms::getInstance()->channel<Flash>(0x0204); }
  }

  // Box Thing v2 -> Box Thing servo, about every 10 ms while the screen is open.
  namespace Servo
  {
    struct Command
    {
      uint8_t manual;
      uint8_t x; // degrees, 0-180
      uint8_t y;
    };

    inline DirectChannel<Command> command() { return DirectComms::getInstance()->channel<Command>(0x0301); }
  }

  // Box Thing v2 broadcasts its encoder; oled 3 shows it.
  namespace Encoder
  {
    struct State
    {
      int64_t detents;
      int8_t clicks;
    };

    inline DirectChannel<State> state() { return DirectComms::getInstance()->channel<State>(0x0401); }
  }

  // Test message from the Send screens; the template boards blink on it.
  namespace Hello
  {
    struct Message
    {
      char text[8]; // NUL padded
    };

    inline DirectChannel<Message> message() { return DirectComms::getInstance()->channel<Message>(0x0501); }
  }
}
