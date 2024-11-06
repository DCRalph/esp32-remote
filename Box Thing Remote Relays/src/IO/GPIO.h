#pragma once
#include "config.h"

class GpIO;

extern GpIO led;
extern GpIO armed;

extern GpIO relay1;
extern GpIO relay2;
extern GpIO relay3;
extern GpIO relay4;
extern GpIO relay5;
extern GpIO relay6;
extern GpIO relay7;
extern GpIO relay8;

extern GpIO *relayMap[8];

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

  bool debounceEnabled;
  unsigned long debounceTime;
  unsigned long lastDebounceTime;
  bool lastStableValue;
  bool lastReadValue;

  String PinModeString(PinMode mode);

public:
  GpIO(uint8_t _pin, PinMode _mode);
  GpIO(uint8_t _pin, PinMode _mode, bool _activeState);

  void init();

  void SetMode(PinMode _mode);
  void SetActiveState(bool _activeState);

  void Write(bool _value);
  void PWM(uint8_t _value);

  bool read();
  int analogRead();

  void Toggle();
  void On();
  void Off();

  void enableDebounce(unsigned long debounceTime);

  uint8_t getPin();
  PinMode getMode();

  static void initIO();
};