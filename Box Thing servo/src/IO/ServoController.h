#pragma once

#include <Arduino.h>

extern void initServos();
extern void loopServos();
extern void servoSetX(uint8_t val);
extern void servoSetY(uint8_t val);
extern void servoSetMan(bool man);
