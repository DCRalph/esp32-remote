#include "GPIO.h"

GpIO led(LED_PIN, Output, HIGH);
GpIO latch(LATCH_PIN, Output);
GpIO batterySense(BATTERY_SENSE_PIN, Input);
GpIO MCPPowerGood(MCP_POWER_GOOD_PIN, Input, LOW);
GpIO MCPState1(MCP_STATE_1_PIN, Input, LOW);
GpIO MCPState2(MCP_STATE_2_PIN, Input, LOW);

Controls::Button knobButton([]
{
  Controls::Button::Config c;
  c.pin = ENCODER_PIN_BUTTON;
  c.activeHigh = true;
  c.pull = Controls::Button::Pull::None;
  c.debounceMs = 20;
  c.multiClickMs = 200;
  c.longMs = 500;
  return c;
}());

// Pull-downs on A and B, as ESP32Encoder's default set them.
Controls::Encoder knob([]
{
  Controls::Encoder::Config c;
  c.pinA = ENCODER_PIN_A;
  c.pinB = ENCODER_PIN_B;
  c.pull = Controls::Pull::Down;
  return c;
}());

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
  Serial.println("\t[GPIO] " + String(pin) + " as " + PinModeString(mode) + " Initializing...");
#endif
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
  batterySense.init();
  MCPPowerGood.init();
  MCPState1.init();
  MCPState2.init();

  // led.On();
  latch.On();

#ifdef DEBUG_GPIO
  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();
#endif

#ifdef DEBUG_GPIO
  Serial.println("\t[Encoder] Initializing...");
#endif

  Controls::add(knob);
  Controls::add(knobButton);

  Serial.println("\t[Encoder] Initialized");
}
