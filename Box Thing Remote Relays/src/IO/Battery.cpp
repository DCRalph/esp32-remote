#include "Battery.h"

#define minVoltage 6.4f
#define maxVoltage 8.4f

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

#ifdef DEBUG_BATTERY
  Serial.printf("1. ADC: %d VCC: %f\n", adcValue, adcVoltage);
#endif

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

// #####################################
// #####################################
// #####################################
// #####################################

#define minVoltage2 12.8f
#define maxVoltage2 16.8f

static float battery2Voltage = 1.0f;
static float battery2VoltageSmooth = 1.0f;

float battery2GetVoltage()
{
  return battery2Voltage;
}

float battery2GetVoltageSmooth()
{
  return battery2VoltageSmooth;
}

int battery2GetPercentage()
{
  float percentage = ((constrain(battery2Voltage, minVoltage2, maxVoltage2) - minVoltage2) / (maxVoltage2 - minVoltage2)) * 100.0f;

  return static_cast<int>(percentage);
}

int battery2GetPercentageSmooth()
{
  float percentage = ((constrain(battery2VoltageSmooth, minVoltage2, maxVoltage2) - minVoltage2) / (maxVoltage2 - minVoltage2)) * 100.0f;

  return static_cast<int>(percentage);
}

void battery2Update()
{
  int adcValue = battery2Sense.analogRead();
  float adcVoltage = (adcValue / 4095.0) * BATTERY2_SENSE_VCC;

#ifdef DEBUG_BATTERY
  Serial.printf("2. ADC: %d VCC: %f\n", adcValue, adcVoltage);
#endif

  battery2Voltage = (adcVoltage * (BATTERY2_SENSE_R1 + BATTERY2_SENSE_R2) / BATTERY2_SENSE_R2) + BATTERY2_SENSE_OFFSET;

  if (battery2VoltageSmooth == 1.0f)
  {
    battery2VoltageSmooth = battery2Voltage;
  }
  else
  {
    battery2VoltageSmooth = battery2VoltageSmooth * 0.95 + battery2Voltage * 0.05;
  }
}