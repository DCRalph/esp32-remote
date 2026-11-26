#include "GPIO.h"

GPIO led(LED_PIN, Output, LOW);
GPIO latch(LATCH_PIN, Output);
GPIO switchPin(SWITCH_PIN, InputPullup, LOW);

// GPIO16 has no internal pull-up.
Controls::Button knobButton([]
{
  Controls::Button::Config c;
  c.pin = ENCODER_PIN_BUTTON;
  c.pull = Controls::Button::Pull::None;
  c.debounceMs = 20;
  c.multiClickMs = 200;
  c.longMs = 500;
  return c;
}());

// The ESP8266 has no PCNT, so dcr_input decodes this on pin-change interrupts.
Controls::Encoder knob(ENCODER_PIN_A, ENCODER_PIN_B);

String GPIO::PinModeString(PinMode mode)
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

GPIO::GPIO(int _pin, PinMode _mode)
{
  pin = _pin;
  mode = _mode;
  activeState = HIGH;
}

GPIO::GPIO(int _pin, PinMode _mode, bool _activeState)
{
  pin = _pin;
  mode = _mode;
  activeState = _activeState;
}

void GPIO::init()
{
  Serial.println("\t[GPIO] " + String(pin) + " as " + PinModeString(mode) + " Initializing...");
  pinMode(pin, mode);

  // Serial.println("\t[GPIO] Initialized");
}

void GPIO::SetMode(PinMode _mode)
{
  mode = _mode;
  pinMode(pin, mode);
}

void GPIO::SetActiveState(bool _activeState)
{
  activeState = _activeState;
}

void GPIO::Write(int _value)
{
  if (mode == Input)
    return;
  digitalWrite(pin, _value);
}

void GPIO::PWM(int _value)
{
  if (mode == Input)
    return;
  analogWrite(pin, _value);
}

bool GPIO::read()
{
  return digitalRead(pin) == activeState;
}

int GPIO::analogRead()
{
  return ::analogRead(pin);
}

void GPIO::Toggle()
{
  if (mode == Input)
    return;
  digitalWrite(pin, !digitalRead(pin));
}

void GPIO::On()
{
  if (mode == Input)
    return;
  digitalWrite(pin, activeState);
}

void GPIO::Off()
{
  if (mode == Input)
    return;
  digitalWrite(pin, !activeState);
}

int GPIO::getPin()
{
  return pin;
}

PinMode GPIO::getMode()
{
  return mode;
}

void GPIO::initIO()
{
  Serial.println("\t[INFO] [IO] Configuring pins...");

  led.init();
  latch.init();
  switchPin.init();

  led.On();
  latch.On();

  Controls::add(knob);
  Controls::add(knobButton);

  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();
}
