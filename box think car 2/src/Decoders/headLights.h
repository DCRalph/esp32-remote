#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class HeadLightsDecoder : public CANFrameDecoder<HeadLightsState>
{
public:
    // Constructor that takes a reference to CANFrameManager
    HeadLightsDecoder();

    // Override the process method to decode RPM data from the frame
    void process(canFrame *frame) override;
};

HeadLightsDecoder::HeadLightsDecoder()
{
    decoderId = 0x164;
    result = HeadLightsState::OFF;
}

void HeadLightsDecoder::process(canFrame *frame)
{
    // Decode RPM data from the frame
    uint8_t state = frame->data[0] & 0x03; // only the first 2 bits are used

    result = static_cast<HeadLightsState>(state);
}