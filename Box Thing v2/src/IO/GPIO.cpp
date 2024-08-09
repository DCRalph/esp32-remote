#include "GPIO.h"

GpIO led(LED_PIN, Output, HIGH);
GpIO latch(LATCH_PIN, Output);
GpIO switchPin(SWITCH_PIN, InputPullup, LOW);
GpIO encoderButton(ENCODER_PIN_BUTTON, Input);
GpIO batterySense(BATTERY_SENSE_PIN, Input);

ClickButton ClickButtonEnc(ENCODER_PIN_BUTTON, HIGH);

RotaryEncoder encoder = RotaryEncoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_BUTTON);

void IRAM_ATTR encoderISR()
{
  encoder.readAB();
}

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
  latch.init();
  switchPin.init();
  encoderButton.init();

  // led.On();
  latch.On();

  ClickButtonEnc.debounceTime = 20;
  ClickButtonEnc.multiclickTime = 200;
  ClickButtonEnc.longClickTime = 500;

  // attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
  attachInterrupt(ENCODER_PIN_A, encoderISR, CHANGE);

  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();

  Serial.println("\t[Encoder] Initializing...");
  encoder.begin();
  Serial.println("\t[Encoder] Initialized");
}
