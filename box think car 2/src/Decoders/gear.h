#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class GearDecoder : public CANFrameDecoder<GearState>
{
public:
    // Constructor that takes a reference to CANFrameManager
    GearDecoder();

    // Override the process method to decode RPM data from the frame
    void process(canFrame *frame) override;
};

GearDecoder::GearDecoder()
{
    decoderId = 0x188;
    result = GearState::NEUTRAL;
}

void GearDecoder::process(canFrame *frame)
{
    uint8_t state = frame->data[1];

    result = static_cast<GearState>(state);
}
