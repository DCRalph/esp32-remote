#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class RPMDecoder : public CANFrameDecoder<int>
{
public:
    // Constructor that takes a reference to CANFrameManager
    RPMDecoder();

    // Override the process method to decode RPM data from the frame
    void process(canFrame *frame) override;
};

RPMDecoder::RPMDecoder()
{
    decoderId = 0x1dc;
    result = 0;
}

void RPMDecoder::process(canFrame *frame)
{
    // Decode RPM data from the frame
    result = (frame->data[1] << 8) | frame->data[2];
}
