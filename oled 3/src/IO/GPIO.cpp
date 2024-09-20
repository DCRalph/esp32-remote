#include "GPIO.h"

GpIO led(LED_PIN, Output, HIGH);

GpIO_RGB btnLed(BTN_LED_R_PIN, BTN_LED_G_PIN, BTN_LED_B_PIN, LOW);

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

GpIO::GpIO()
{
  pin = -1;
  mode = Input;
  activeState = HIGH;
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
  if (mode != Output)
    return;
  if (activeState == LOW)
    _value = 255 - _value;
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

  btnLed.init();

  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();
}

GpIO_RGB::GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB)
{
  R = GpIO(_pinR, Output);
  G = GpIO(_pinG, Output);
  B = GpIO(_pinB, Output);

  R.Off();
  G.Off();
  B.Off();
}

GpIO_RGB::GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB, bool _activeState)
{
  R = GpIO(_pinR, Output, _activeState);
  G = GpIO(_pinG, Output, _activeState);
  B = GpIO(_pinB, Output, _activeState);

  R.Off();
  G.Off();
  B.Off();
}

void GpIO_RGB::init()
{
  R.init();
  G.init();
  B.init();
}

void GpIO_RGB::Off()
{
  R.PWM(0);
  G.PWM(0);
  B.PWM(0);
}

void GpIO_RGB::SetColor(uint8_t _r, uint8_t _g, uint8_t _b)
{
  R.PWM(_r);
  G.PWM(_g);
  B.PWM(_b);
}

void GpIO_RGB::SetColor(uint32_t color)
{
  SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void GpIO_RGB::SetColor565(uint16_t color)
{
  SetColor((color >> 8) & 0xF8, (color >> 3) & 0xFC, (color << 3) & 0xF8);
}
