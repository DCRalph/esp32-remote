#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class ShifterPosDecoder : public CANFrameDecoder<int>
{
public:
    // Constructor that takes a reference to CANFrameManager
    ShifterPosDecoder();

    // Override the process method to decode RPM data from the frame
    void process(const canFrame& frame) override;
};

ShifterPosDecoder::ShifterPosDecoder()
{
    decoderId = 0x188;
}

void ShifterPosDecoder::process(const canFrame& frame)
{
    // Decode RPM data from the frame
    result = 0;
}

