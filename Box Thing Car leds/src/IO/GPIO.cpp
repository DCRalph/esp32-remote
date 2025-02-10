#include "GPIO.h"

GpIO led(LED_PIN, Output, HIGH);

String GpIO::PinModeString(PinMode mode)
{
  switch (mode)
  {
  case Input:
    return "Input";
  case Output:
    return "Output";
  case InputPullup:
    return "InputPullup";
  default:
    return "Unknown";
  }
}

GpIO::GpIO(uint8_t _pin, PinMode _mode)
{
  pin = _pin;
  mode = _mode;
  activeState = HIGH;
}

GpIO::GpIO(uint8_t _pin, PinMode _mode, bool _activeState)
{
  pin = _pin;
  mode = _mode;
  activeState = _activeState;
}

void GpIO::init()
{
  Serial.println("\t[GPIO] " + String(pin) + " as " + PinModeString(mode) + " Initializing...");
  pinMode(pin, mode);

  if (mode == Output)
    digitalWrite(pin, !activeState);

  // Serial.println("\t[GPIO] Initialized");
}

void GpIO::SetMode(PinMode _mode)
{
  mode = _mode;
  pinMode(pin, mode);
}

void GpIO::SetActiveState(bool _activeState)
{
  activeState = _activeState;
}

void GpIO::Write(uint8_t _value)
{
  if (mode == Input)
    return;
  digitalWrite(pin, _value);
}

void GpIO::PWM(uint8_t _value)
{
  if (mode == Input)
    return;
  analogWrite(pin, _value);
}

bool GpIO::read()
{
  return digitalRead(pin) == activeState;
}

int GpIO::analogRead()
{
  return ::analogRead(pin);
}

void GpIO::Toggle()
{
  if (mode == Input)
    return;
  digitalWrite(pin, !digitalRead(pin));
}

void GpIO::On()
{
  if (mode == Input)
    return;
  digitalWrite(pin, activeState);
}

void GpIO::Off()
{
  if (mode == Input)
    return;
  digitalWrite(pin, !activeState);
}

uint8_t GpIO::getPin()
{
  return pin;
}

PinMode GpIO::getMode()
{
  return mode;
}

void GpIO::initIO()
{
  Serial.println("\t[INFO] [IO] Configuring pins...");

  led.init();


  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();
}
