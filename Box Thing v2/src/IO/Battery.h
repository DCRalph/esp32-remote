#pragma once

#include "config.h"
#include "IO/GPIO.h"

extern float batteryGetVoltage();
extern float batteryGetVoltageSmooth();
extern int batteryGetPercentage();
extern int batteryGetPercentageSmooth();
extern void batteryUpdate();