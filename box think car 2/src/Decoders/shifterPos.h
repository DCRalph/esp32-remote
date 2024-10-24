#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class ShifterPosDecoder : public CANFrameDecoder<ShifterState>
{
public:
    // Constructor that takes a reference to CANFrameManager
    ShifterPosDecoder();

    // Override the process method to decode RPM data from the frame
    void process(canFrame *frame) override;
};

ShifterPosDecoder::ShifterPosDecoder()
{
    decoderId = 0x188;
    result = ShifterState::PARK;
}

void ShifterPosDecoder::process(canFrame *frame)
{
    uint8_t state = frame->data[3];

    result = static_cast<ShifterState>(state);
}
