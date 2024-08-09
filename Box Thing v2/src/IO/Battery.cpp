#include "Battery.h"

static float batteryVoltage = 0.0f;

float batteryGetVoltage()
{
  return batteryVoltage;
}

int batteryGetPercentage()
{
  const float minVoltage = 3.2f;
  const float maxVoltage = 4.2f;

  // Clamp the voltage to be within the min and max range
  if (batteryVoltage < minVoltage)
  {
    return 0;
  }
  else if (batteryVoltage > maxVoltage)
  {
    return 100;
  }

  float percentage = ((batteryVoltage - minVoltage) / (maxVoltage - minVoltage)) * 100.0f;

  return static_cast<int>(percentage);
}

void batteryUpdate()
{
  int adcValue = batterySense.analogRead();
  float adcVoltage = (adcValue / 4095.0) * BATTERY_SENSE_VCC;

  batteryVoltage = (adcVoltage * (BATTERY_SENSE_R1 + BATTERY_SENSE_R2) / BATTERY_SENSE_R2) + BATTERY_SENSE_OFFSET;
}