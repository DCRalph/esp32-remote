#pragma once
#include "config.h"

#include <RotaryEncoder.h>
#include <ClickButton.h>
#include <FunctionalInterrupt.h>

class GpIO;

extern GpIO led;
extern GpIO latch;
extern GpIO switchPin;
extern GpIO encoderButton;
extern GpIO batterySense;

extern RotaryEncoder encoder;

extern ClickButton ClickButtonEnc;

void IRAM_ATTR encoderISR();

enum PinMode
{
  Input = INPUT,
  Output = OUTPUT,
  InputPullup = INPUT_PULLUP
};

class GpIO
{
private:
  uint8_t pin;
  PinMode mode;
  bool activeState;

  String PinModeString(PinMode mode);

public:
  GpIO(uint8_t _pin, PinMode _mode);
  GpIO(uint8_t _pin, PinMode _mode, bool _activeState);

  void init();

  void SetMode(PinMode _mode);
  void SetActiveState(bool _activeState);

  void Write(uint8_t _value);
  void PWM(uint8_t _value);

  bool read();
  int analogRead();

  void Toggle();
  void On();
  void Off();

  uint8_t getPin();
  PinMode getMode();

  static void initIO();
};
