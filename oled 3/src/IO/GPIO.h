#pragma once
#include "config.h"

class GpIO;
class GpIO_RGB;

extern GpIO led;

extern GpIO_RGB btnLed;

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
  GpIO();
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

class GpIO_RGB
{
private:
  GpIO R;
  GpIO G;
  GpIO B;

public:
  GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB);
  GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB, bool _activeState);

  void init();

  void Off();
  void SetColor(uint8_t _r, uint8_t _g, uint8_t _b);
  void SetColor(uint32_t color);
  void SetColor565(uint16_t color);
};