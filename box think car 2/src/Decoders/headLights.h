#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class HeadLightsDecoder : public CANFrameDecoder<HeadLightsState>
{
public:
    // Constructor that takes a reference to CANFrameManager
    HeadLightsDecoder();

    // Override the process method to decode RPM data from the frame
    void process(const canFrame &frame) override;
};
