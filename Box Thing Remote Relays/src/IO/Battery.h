#pragma once

#include "config.h"
#include "IO/GPIO.h"

extern float batteryGetVoltage();
extern float batteryGetVoltageSmooth();
extern int batteryGetPercentage();
extern int batteryGetPercentageSmooth();
extern void batteryUpdate();

extern float battery2GetVoltage();
extern float battery2GetVoltageSmooth();
extern int battery2GetPercentage();
extern int battery2GetPercentageSmooth();
extern void battery2Update();