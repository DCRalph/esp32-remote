#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class RPMDecoder : public CANFrameDecoder<int>
{
public:
    // Constructor that takes a reference to CANFrameManager
    RPMDecoder();

    // Override the process method to decode RPM data from the frame
    void process(const canFrame& frame) override;
};

RPMDecoder::RPMDecoder()
{
    decoderId = 0x1dc;
}

void RPMDecoder::process(const canFrame& frame)
{
    // Decode RPM data from the frame
    result = (frame.data[1] << 8) | frame.data[2];
}

