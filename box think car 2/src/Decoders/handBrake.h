#pragma once

#include "config.h"
#include "IO/canDecoder.h"

class HandBrakeDecoder : public CANFrameDecoder<HandBrakeState>
{
public:
    HandBrakeDecoder();

    void process(canFrame *frame) override;
};

HandBrakeDecoder::HandBrakeDecoder()
{
    decoderId = 0x164;
    result = HandBrakeState::OFF;
}

void HandBrakeDecoder::process(canFrame *frame)
{
    // Decode data from the frame
    // b00000x00 - x is the state
    uint8_t state = frame->data[0] & 0b00000100; // only the 3rd bit is used
    if (state == 0)
    {
        result = HandBrakeState::ON;
    }
    else
    {
        result = HandBrakeState::OFF;
    }
}