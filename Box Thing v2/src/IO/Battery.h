#pragma once

#include "config.h"
#include "IO/GPIO.h"

extern float batteryGetVoltage();
extern int batteryGetPercentage();
extern void batteryUpdate();