#pragma once
#include "config.h"

#include <ESP32Encoder.h>
#include <ClickButton.h>
#include <FunctionalInterrupt.h>

class GpIO;

extern GpIO led;
extern GpIO latch;
extern GpIO encoderButton;
extern GpIO batterySense;
extern GpIO MCPPowerGood;
extern GpIO MCPState1;
extern GpIO MCPState2;

extern ESP32Encoder encoder;

extern ClickButton ClickButtonEnc;

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

/** Detents turned since the previous call, signed. Counts are never cleared, so none are lost. */
int32_t encoderTakeDetents();
/** Detent position since the last reset. */
int64_t encoderDetents();
/** Zero the position and drop any turns not yet taken. */
void encoderReset();