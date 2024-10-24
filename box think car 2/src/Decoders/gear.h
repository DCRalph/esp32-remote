#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class GearDecoder : public CANFrameDecoder<int>
{
public:
    // Constructor that takes a reference to CANFrameManager
    GearDecoder();

    // Override the process method to decode RPM data from the frame
    void process(const canFrame& frame) override;
};

GearDecoder::GearDecoder()
{
    decoderId = 0x188;
}

void GearDecoder::process(const canFrame& frame)
{
    // Decode RPM data from the frame
    result = 0;
}

