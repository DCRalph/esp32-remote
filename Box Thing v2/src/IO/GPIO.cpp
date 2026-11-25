#include "GPIO.h"

#undef LOG_TAG
#define LOG_TAG "GPIO"

GpIO led(LED_PIN, Output, HIGH);
GpIO latch(LATCH_PIN, Output);
GpIO encoderButton(ENCODER_PIN_BUTTON, Input);
GpIO batterySense(BATTERY_SENSE_PIN, Input);
GpIO MCPPowerGood(MCP_POWER_GOOD_PIN, Input, LOW);
GpIO MCPState1(MCP_STATE_1_PIN, Input, LOW);
GpIO MCPState2(MCP_STATE_2_PIN, Input, LOW);

ClickButton ClickButtonEnc(ENCODER_PIN_BUTTON, HIGH);

ESP32Encoder encoder;

namespace
{
  int64_t detentPosition = 0; // hysteresis-tracked position, in detents
  int64_t takenDetents = 0;   // position already handed out by encoderTakeDetents()
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
  encoderButton.init();
  batterySense.init();
  MCPPowerGood.init();
  MCPState1.init();
  MCPState2.init();

  latch.On();

  ClickButtonEnc.debounceTime = 20;
  ClickButtonEnc.multiclickTime = 200;
  ClickButtonEnc.longClickTime = 500;

  debugI("Pins configured");
  debugI("Encoder initializing...");

  encoder.attachFullQuad(ENCODER_PIN_A, ENCODER_PIN_B);
  // Longest PCNT glitch filter (1023 APB cycles, ~12.8 us); the default 250 passes contact bounce.
  encoder.setFilter(1023);
  encoderReset();

  debugI("Encoder initialized");
}

// A detent counts only once the shaft is ENCODER_COUNTS_PER_DETENT - 1 counts
// past the last one, so bounce or a knob resting near a boundary cannot flicker.
int64_t encoderDetents()
{
  constexpr int64_t cpd = ENCODER_COUNTS_PER_DETENT;
  const int64_t count = encoder.getCount();
  while (count >= detentPosition * cpd + (cpd - 1))
    detentPosition++;
  while (count <= detentPosition * cpd - (cpd - 1))
    detentPosition--;
  return detentPosition;
}

int32_t encoderTakeDetents()
{
  const int64_t position = encoderDetents();
  const int32_t delta = static_cast<int32_t>(position - takenDetents);
  takenDetents = position;
  if (delta != 0)
    lastInteract = millis();
  return delta;
}

void encoderReset()
{
  encoder.clearCount();
  detentPosition = takenDetents = 0;
}
