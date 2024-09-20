#pragma once
#include "config.h"
#include <vector>

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

enum class RGB_MODE
{
  Manual,
  Rainbow,
  Pulsing,
  Blink
};

class GpIO_RGB
{
private:
  GpIO R;
  GpIO G;
  GpIO B;

  RGB_MODE mode = RGB_MODE::Manual;
  uint8_t maxModeHistory = 10;
  std::vector<RGB_MODE> modeHistory;

  uint32_t prevManualColor = 0;

  void _SetColor(uint8_t _r, uint8_t _g, uint8_t _b);
  void _SetColor(uint32_t color);
  void _SetMode(RGB_MODE _mode);

  xTaskHandle rainbowHandle = NULL;
  uint8_t rainbowSpeed = 5;

  void StartRainbow();
  void StopRainbow();
  void Rainbow();

  xTaskHandle pulsingHandle = NULL;
  uint8_t pulsingSpeed = 2;
  uint32_t pulsingColor = 0;

  void StartPulsing();
  void StopPulsing();
  void Pulsing();

  xTaskHandle blinkHandle = NULL;
  uint8_t blinkSpeed = 2;
  uint8_t blinkCount = 0;
  uint32_t blinkColor = 0;

  void StartBlink();
  void StopBlink();
  void Blink();

public:
  GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB);
  GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB, bool _activeState);

  void init();

  void Off();
  void SetColor(uint8_t _r, uint8_t _g, uint8_t _b);
  void SetColor(uint32_t color);
  void SetColor565(uint16_t color);

  void SetMode(RGB_MODE _mode);
  void SetPrevMode();
  RGB_MODE GetMode();

  void SetPulsingColor(uint32_t _color);

  void Blink(uint32_t _color, uint8_t _speed, uint8_t _count);
};