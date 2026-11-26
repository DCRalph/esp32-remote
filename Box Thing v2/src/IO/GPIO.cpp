#include "GPIO.h"

#undef LOG_TAG
#define LOG_TAG "GPIO"

GpIO led(LED_PIN, Output, HIGH);
GpIO latch(LATCH_PIN, Output);
GpIO batterySense(BATTERY_SENSE_PIN, Input);
GpIO MCPPowerGood(MCP_POWER_GOOD_PIN, Input, LOW);
GpIO MCPState1(MCP_STATE_1_PIN, Input, LOW);
GpIO MCPState2(MCP_STATE_2_PIN, Input, LOW);

// Active high on an external pull-down. Also the power button, so the press that
// turned the box on is still held when it is added, and never counts.
Controls::Button knobButton([]
                            {
  Controls::Button::Config c;
  c.pin = ENCODER_PIN_BUTTON;
  c.activeHigh = true;
  c.pull = Controls::Button::Pull::None;
  c.debounceMs = 20;
  c.multiClickMs = 200;
  c.longMs = 500;
  return c; }());

// Pull-downs on A and B. The PCNT glitch filter defaults to its maximum,
// which this knob's contact bounce needs.
Controls::Encoder knob([]
                       {
  Controls::Encoder::Config c;
  c.pinA = ENCODER_PIN_A;
  c.pinB = ENCODER_PIN_B;
  c.pull = Controls::Pull::Down;
  c.countsPerDetent = ENCODER_COUNTS_PER_DETENT;
  return c; }());

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
#ifdef DEBUG_GPIO
  debugD("%d as %s initializing...", pin, PinModeString(mode).c_str());
#endif
  pinMode(pin, mode);

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
  debugI("Configuring pins...");

  led.init();
  latch.init();
  batterySense.init();
  MCPPowerGood.init();
  MCPState1.init();
  MCPState2.init();

  latch.On();

  debugI("Pins configured");
  debugI("Encoder initializing...");

  Controls::add(knob);
  Controls::add(knobButton);

  debugI("Encoder initialized");
}
