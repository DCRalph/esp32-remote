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

  void print();
};

enum class HeadLightsState
{
// 0x20 - 32
// 0x21 - 33
// 0x22 - 34
// 0x23 - 35
  OFF = 0x00, // 36
  FOG = 0x01, // 37
  LOWBEAM = 0x02, // 38
  HIGHBEAM = 0x03, // 39
};

// string map for HeadLightsState
const std::unordered_map<HeadLightsState, std::string> HeadLightsStateMap = {
    {HeadLightsState::OFF, "OFF"},
    {HeadLightsState::FOG, "FOG"},
    {HeadLightsState::LOWBEAM, "LOWBEAM"},
    {HeadLightsState::HIGHBEAM, "HIGHBEAM"},
};

enum class HandBrakeState
{
  OFF = 0x00,
  ON = 0x01,
};

// string map for HandBrakeState
const std::unordered_map<HandBrakeState, std::string> HandBrakeStateMap = {
    {HandBrakeState::OFF, "OFF"},
    {HandBrakeState::ON, "ON"},
};

enum class GearState
{
  NEUTRAL = 0x00,
  FIRST = 0x01,
  SECOND = 0x02,
  THIRD = 0x03,
  FOURTH = 0x04,
  FIFTH = 0x05,
  REVERSE = 0x06,
};

// string map for GearState
const std::unordered_map<GearState, std::string> GearStateMap = {
    {GearState::NEUTRAL, "NEUTRAL"},
    {GearState::FIRST, "FIRST"},
    {GearState::SECOND, "SECOND"},
    {GearState::THIRD, "THIRD"},
    {GearState::FOURTH, "FOURTH"},
    {GearState::FIFTH, "FIFTH"},
    {GearState::REVERSE, "REVERSE"},
};

// shifter states
enum class ShifterState
{
  NONE = 0x00,
  PARK = 0x01,
  REVERSE = 0x02,
  NEUTRAL = 0x04,
  DRIVE = 0x08,
  DRIVE3 = 0x20,
  SECOND = 0x40,
  FIRST = 0x80,
};

// string map for ShifterState
const std::unordered_map<ShifterState, std::string> ShifterStateMap = {
    {ShifterState::NONE, "NONE"},
    {ShifterState::PARK, "PARK"},
    {ShifterState::REVERSE, "REVERSE"},
    {ShifterState::NEUTRAL, "NEUTRAL"},
    {ShifterState::DRIVE, "DRIVE"},
    {ShifterState::DRIVE3, "DRIVE3"},
    {ShifterState::SECOND, "SECOND"},
    {ShifterState::FIRST, "FIRST"},
};