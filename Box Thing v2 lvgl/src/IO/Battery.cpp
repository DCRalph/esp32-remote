#include "Battery.h"

#define minVoltage 3.2f
#define maxVoltage 4.2f

static float batteryVoltage = 1.0f;
static float batteryVoltageSmooth = 1.0f;

float batteryGetVoltage()
{
  return batteryVoltage;
}

float batteryGetVoltageSmooth()
{
  return batteryVoltageSmooth;
}

int batteryGetPercentage()
{
  float percentage = ((constrain(batteryVoltage, minVoltage, maxVoltage) - minVoltage) / (maxVoltage - minVoltage)) * 100.0f;

  return static_cast<int>(percentage);
}

int batteryGetPercentageSmooth()
{
  float percentage = ((constrain(batteryVoltageSmooth, minVoltage, maxVoltage) - minVoltage) / (maxVoltage - minVoltage)) * 100.0f;

  return static_cast<int>(percentage);
}

void batteryUpdate()
{
  int adcValue = batterySense.analogRead();
  float adcVoltage = (adcValue / 4095.0) * BATTERY_SENSE_VCC;

  batteryVoltage = (adcVoltage * (BATTERY_SENSE_R1 + BATTERY_SENSE_R2) / BATTERY_SENSE_R2) + BATTERY_SENSE_OFFSET;

  if (batteryVoltageSmooth == 1.0f)
  {
    batteryVoltageSmooth = batteryVoltage;
  }
  else
  {
    batteryVoltageSmooth = batteryVoltageSmooth * 0.95 + batteryVoltage * 0.05;
  }
}