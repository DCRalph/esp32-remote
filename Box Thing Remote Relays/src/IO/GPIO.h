#pragma once
#include "config.h"
#include <vector>
#include <FastLED.h>

#define FASTLED_CORE 1

extern CRGB leds[NUM_LEDS];

class GpIO;
class GpIO_RGB;

extern GpIO led;
extern GpIO armedSW;

extern GpIO_RGB pwrLed;
extern GpIO_RGB armedLed;

extern GpIO batterySense;
extern GpIO battery2Sense;

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
  GpIO();
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

// #######################################################
// #######################################################
// ################### RGB LED CONTROL ###################
// #######################################################
// #######################################################

enum class RGB_MODE
{
  Manual,
  OVERRIDE,
  Rainbow,
  Pulsing,
  Blink
};

class GpIO_RGB
{
private:
  uint16_t startIdx;
  uint16_t numLeds;

  RGB_MODE mode = RGB_MODE::Manual;
  uint8_t maxModeHistory = 10;
  std::vector<RGB_MODE> modeHistory;

  uint32_t prevManualColor = 0;

  void _SetColor(uint8_t _r, uint8_t _g, uint8_t _b);
  void _SetColor(uint32_t color);
  void _UpdateModeHistory(RGB_MODE _oldMode);
  void _SetMode(RGB_MODE _mode);

  xTaskHandle rainbowHandle = NULL;
  uint8_t rainbowSpeed = 5;

  void _StartRainbow();
  void _StopRainbow();
  void _Rainbow();

  xTaskHandle pulsingHandle = NULL;
  uint8_t pulsingSpeed = 2;
  uint32_t pulsingColor = 0;

  void _StartPulsing();
  void _StopPulsing();
  void _Pulsing();

  xTaskHandle blinkHandle = NULL;
  uint8_t blinkSpeed = 2;
  uint8_t blinkCount = 0;
  uint32_t blinkColor = 0;

  void _StartBlink();
  void _StopBlink();
  void _Blink();

public:
  GpIO_RGB(uint16_t _startIdx, uint16_t _numLeds);

  static void initStrip();
  static void showStrip();

  void Off();
  void SetColor(uint8_t _r, uint8_t _g, uint8_t _b);
  void SetColor(uint32_t color);
  void SetColor565(uint16_t color);

  void SetColorOveride(uint8_t _r, uint8_t _g, uint8_t _b);
  void SetColorOveride(uint32_t color);

  void SetMode(RGB_MODE _mode);
  void SetPrevMode();
  RGB_MODE GetMode();

  void SetPulsingColor(uint32_t _color);
  void SetPulsingColor(uint8_t _r, uint8_t _g, uint8_t _b);

  void Blink(uint32_t _color, uint8_t _speed, uint8_t _count);
};