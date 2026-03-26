#pragma once

#include "ScreenManager.h"
#include "Wireless.h"

namespace BoxThingEncoderScreenNamespace
{
void onRecvPacket(const TransportPacket &pkt);
}

extern const Screen2 BoxThingEncoderScreen;
