#pragma once

#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <SPI.h>

#include <unordered_map>

struct canFrame
{
  unsigned long id;
  uint8_t extFlag;
  uint8_t len;
  uint8_t data[8] __attribute__((aligned(8)));
};

enum class HeadLightsState
{
  OFF = 0x24,
  FOG = 0x25,
  LOWBEAM = 0x26,
  HIGHBEAM = 0x27,
};

// string map for HeadLightsState
const std::unordered_map<HeadLightsState, std::string> HeadLightsStateMap = {
    {HeadLightsState::OFF, "OFF"},
    {HeadLightsState::FOG, "FOG"},
    {HeadLightsState::LOWBEAM, "LOWBEAM"},
    {HeadLightsState::HIGHBEAM, "HIGHBEAM"},
};