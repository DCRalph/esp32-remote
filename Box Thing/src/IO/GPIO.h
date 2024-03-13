#pragma once
#include "config.h"

#include "IO/EncoderHelper.h"
#include <ClickButton.h>

class GPIO;

extern GPIO led;
extern GPIO latch;
extern GPIO switchPin;
extern GPIO encoderButton;

extern EncoderHelper encoder;

extern ClickButton ClickButtonEnc;

static void IRAM_ATTR encoderISR()
{
  encoder.encoder.readAB();
}

enum PinMode
{
  Input = INPUT,
  Output = OUTPUT,
  InputPullup = INPUT_PULLUP
};

class GPIO
{
private:
  int pin;
  PinMode mode;
  bool activeState;

  String PinModeString(PinMode mode);

public:
  GPIO(int _pin, PinMode _mode);
  GPIO(int _pin, PinMode _mode, bool _activeState);

  void init();

  void SetMode(PinMode _mode);
  void SetActiveState(bool _activeState);

  void Write(int _value);
  void PWM(int _value);

  bool read();
  int analogRead();

  void Toggle();
  void On();
  void Off();

  int getPin();
  PinMode getMode();

  static void initIO();
};
