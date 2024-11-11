#include "GPIO.h"

CRGB leds[NUM_LEDS];

GpIO led(LED_PIN, Output, HIGH);
GpIO armedSW(ARMED_PIN, InputPullup, LOW);

GpIO_RGB pwrLed(0, 1);
GpIO_RGB armedLed(1, 1);

GpIO relay1(RELAY_1_PIN, Output, LOW);
GpIO relay2(RELAY_2_PIN, Output, LOW);
GpIO relay3(RELAY_3_PIN, Output, LOW);
GpIO relay4(RELAY_4_PIN, Output, LOW);
GpIO relay5(RELAY_5_PIN, Output, LOW);
GpIO relay6(RELAY_6_PIN, Output, LOW);
GpIO relay7(RELAY_7_PIN, Output, LOW);
GpIO relay8(RELAY_8_PIN, Output, LOW);

GpIO *relayMap[8];

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

GpIO::GpIO()
{
  pin = -1;
  mode = Input;
  activeState = HIGH;
}

GpIO::GpIO(uint8_t _pin, PinMode _mode)
{
  pin = _pin;
  mode = _mode;
  activeState = HIGH;

  debounceEnabled = false;
  debounceTime = 0;
  lastDebounceTime = 0;
  lastStableValue = digitalRead(pin);
  lastReadValue = lastStableValue;
}

GpIO::GpIO(uint8_t _pin, PinMode _mode, bool _activeState)
{
  pin = _pin;
  mode = _mode;
  activeState = _activeState;

  debounceEnabled = false;
  debounceTime = 0;
  lastDebounceTime = 0;
  lastStableValue = digitalRead(pin);
  lastReadValue = lastStableValue;
}

void GpIO::init()
{
  Serial.println("\t[GPIO] " + String(pin) + " as " + PinModeString(mode) + " Initializing...");
  pinMode(pin, mode);

  if (mode == Output)
    digitalWrite(pin, !activeState);

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

void GpIO::Write(bool _value)
{
  if (mode == Input)
    return;
  digitalWrite(pin, _value ^ !activeState);
}

void GpIO::PWM(uint8_t _value)
{
  if (mode == Input)
    return;
  analogWrite(pin, _value);
}

bool GpIO::read()
{
  if (debounceEnabled)
  {
    bool currentValue = digitalRead(pin);
    if (currentValue != lastReadValue)
    {
      lastDebounceTime = millis();
      lastReadValue = currentValue;
    }

    if ((millis() - lastDebounceTime) > debounceTime)
    {
      if (lastStableValue != lastReadValue)
      {
        lastStableValue = lastReadValue;
      }
    }

    return lastStableValue == activeState;
  }
  else
  {
    return digitalRead(pin) == activeState;
  }
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

void GpIO::enableDebounce(unsigned long _debounceTime)
{
  debounceEnabled = true;
  debounceTime = _debounceTime;
  lastDebounceTime = millis();
  lastStableValue = digitalRead(pin);
  lastReadValue = lastStableValue;
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

  armedSW.init();
  armedSW.enableDebounce(50);

  GpIO_RGB::initStrip();

  pwrLed.initStrip();
  armedLed.initStrip();

  relay1.init();
  relay2.init();
  relay3.init();
  relay4.init();
  relay5.init();
  relay6.init();
  relay7.init();
  relay8.init();

  relayMap[0] = &relay1;
  relayMap[1] = &relay2;
  relayMap[2] = &relay3;
  relayMap[3] = &relay4;
  relayMap[4] = &relay5;
  relayMap[5] = &relay6;
  relayMap[6] = &relay7;
  relayMap[7] = &relay8;

  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();
}

// #######################################################
// #######################################################
// ################### RGB LED CONTROL ###################
// #######################################################
// #######################################################

GpIO_RGB::GpIO_RGB(uint16_t _startIdx, uint16_t _numLeds)
{
  startIdx = _startIdx;
  numLeds = _numLeds;
}

void GpIO_RGB::initStrip()
{
  FastLED.addLeds<NEOPIXEL, RGB_STRIP_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

void GpIO_RGB::showStrip()
{
  FastLED.show();
}

void GpIO_RGB::_SetColor(uint8_t _r, uint8_t _g, uint8_t _b)
{
  for (uint16_t i = startIdx; i < startIdx + numLeds; ++i)
  {
    leds[i] = CRGB(_r, _g, _b);
  }
  // showStrip();
}

void GpIO_RGB::_SetColor(uint32_t color)
{
  _SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void GpIO_RGB::_UpdateModeHistory(RGB_MODE _oldMode)
{
  if (modeHistory.size() >= maxModeHistory)
    modeHistory.erase(modeHistory.begin());
  modeHistory.push_back(_oldMode);
}

void GpIO_RGB::_SetMode(RGB_MODE _newMode)
{
  mode = _newMode;

  ESP_LOGI("RGB", "Mode changed to %d", mode);

  if (mode == RGB_MODE::Rainbow)
    _StartRainbow();
  else
    _StopRainbow();

  if (mode == RGB_MODE::Pulsing)
    _StartPulsing();
  else
    _StopPulsing();

  if (mode == RGB_MODE::Blink)
    _StartBlink();

  if (mode == RGB_MODE::Manual)
    _SetColor(prevManualColor);
  else
    _SetColor(0, 0, 0);
}

void GpIO_RGB::_StartRainbow()
{
  if (rainbowHandle == NULL)
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
                              GpIO_RGB *rgb = (GpIO_RGB *)pvParameters;
                              while (true)
                              {
                                rgb->_Rainbow();
                              }
                              //
                            },
                            "Rainbow", 4096, this, 1, &rainbowHandle, FASTLED_CORE);
}

void GpIO_RGB::_StopRainbow()
{
  if (rainbowHandle != NULL)
  {
    vTaskDelete(rainbowHandle);
    rainbowHandle = NULL;
  }
}

void GpIO_RGB::_Rainbow()
{
  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(255 - i, i, 0); // Red to Green
    vTaskDelay(pdMS_TO_TICKS(rainbowSpeed));
  }
  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(0, 255 - i, i); // Green to Blue
    vTaskDelay(pdMS_TO_TICKS(rainbowSpeed));
  }
  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(i, 0, 255 - i); // Blue to Red
    vTaskDelay(pdMS_TO_TICKS(rainbowSpeed));
  }
}

void GpIO_RGB::_StartPulsing()
{
  // ESP_LOGI("RGB", "StartPulsing s");

  if (pulsingHandle == NULL)
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
                              GpIO_RGB *rgb = (GpIO_RGB *)pvParameters;
                              while (true)
                              {
                                // ESP_LOGI("RGB", "Pulsing s");
                                rgb->_Pulsing();
                                // ESP_LOGI("RGB", "Pulsing e");
                              }
                              //
                            },
                            "Pulsing", 4096, this, 1, &pulsingHandle, FASTLED_CORE);
  // ESP_LOGI("RGB", "StartPulsing e");
}

void GpIO_RGB::_StopPulsing()
{
  if (pulsingHandle != NULL)
  {
    vTaskDelete(pulsingHandle);
    pulsingHandle = NULL;
  }
}

void GpIO_RGB::_Pulsing()
{
  uint8_t r = (pulsingColor >> 16) & 0xFF;
  uint8_t g = (pulsingColor >> 8) & 0xFF;
  uint8_t b = pulsingColor & 0xFF;

  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor((r * i / 255), (g * i / 255), (b * i / 255));
    vTaskDelay(pdMS_TO_TICKS(pulsingSpeed));
  }
  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(r - (r * i / 255), g - (g * i / 255), b - (b * i / 255));
    vTaskDelay(pdMS_TO_TICKS(pulsingSpeed));
  }
}

void GpIO_RGB::_StartBlink()
{
  if (blinkHandle == NULL)
    xTaskCreatePinnedToCore([](void *pvParameters)
                            {
                              GpIO_RGB *rgb = (GpIO_RGB *)pvParameters;

                              vTaskDelay(pdMS_TO_TICKS(5));

                              rgb->_Blink();

                              rgb->SetPrevMode();

                              rgb->blinkHandle = NULL;

                              vTaskDelete(NULL);
                              //
                            },
                            "Blink", 4096, this, 1, &blinkHandle, FASTLED_CORE);
}

void GpIO_RGB::_StopBlink()
{
  if (blinkHandle != NULL)
  {
    vTaskDelete(blinkHandle);
    blinkHandle = NULL;
  }
}

void GpIO_RGB::_Blink()
{
  for (uint8_t i = 0; i < blinkCount; i++)
  {
    _SetColor(blinkColor);
    vTaskDelay(pdMS_TO_TICKS(blinkSpeed));
    _SetColor(0, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkSpeed));
  }
}

// ########### PUBLIC METHODS ###########

void GpIO_RGB::Off()
{
  if (mode != RGB_MODE::Manual)
  {
    ESP_LOGI("RGB", "Off() called while not in Manual mode");
    return;
  }
  prevManualColor = 0;
  _SetColor(0, 0, 0);
}

void GpIO_RGB::SetColor(uint8_t _r, uint8_t _g, uint8_t _b)
{
  if (mode != RGB_MODE::Manual)
  {
    ESP_LOGI("RGB", "SetColor() called while not in Manual mode");
    return;
  }
  prevManualColor = (_r << 16) | (_g << 8) | _b;
  _SetColor(_r, _g, _b);
}

void GpIO_RGB::SetColor(uint32_t color)
{
  SetColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void GpIO_RGB::SetColor565(uint16_t color)
{
  SetColor((color >> 8) & 0xF8, (color >> 3) & 0xFC, (color << 3) & 0xF8);
}

void GpIO_RGB::SetColorOveride(uint8_t _r, uint8_t _g, uint8_t _b)
{
  if (mode != RGB_MODE::OVERRIDE)
  {
    ESP_LOGI("RGB", "SetColorOveride() called while not in Overide mode");
    return;
  }

  _SetColor(_r, _g, _b);
}

void GpIO_RGB::SetColorOveride(uint32_t color)
{
  SetColorOveride((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void GpIO_RGB::SetMode(RGB_MODE _newMode)
{
  if (mode == _newMode)
  {
    ESP_LOGI("RGB", "SetMode() called with the same mode");
    return;
  }

  if (mode == RGB_MODE::Blink)
  {
    ESP_LOGI("RGB", "Not changed because current mode is Blink");
    return;
  }

  if (mode == RGB_MODE::OVERRIDE)
  {
    ESP_LOGI("RGB", "Not changed because current mode is Override");
    return;
  }

  _UpdateModeHistory(mode);
  _SetMode(_newMode);
}

void GpIO_RGB::SetPrevMode()
{
  if (modeHistory.empty())
  {
    ESP_LOGI("RGB", "SetPrevMode() called with no history");
    mode = RGB_MODE::Manual;
    return;
  }
  _SetMode(modeHistory.back());
  modeHistory.pop_back();
}

RGB_MODE GpIO_RGB::GetMode()
{
  return mode;
}

void GpIO_RGB::SetPulsingColor(uint32_t _color)
{
  pulsingColor = _color;
}

void GpIO_RGB::SetPulsingColor(uint8_t _r, uint8_t _g, uint8_t _b)
{
  pulsingColor = (_r << 16) | (_g << 8) | _b;
}

void GpIO_RGB::Blink(uint32_t _color, uint8_t _speed, uint8_t _count)
{
  if (mode == RGB_MODE::Blink)
  {
    ESP_LOGI("RGB", "Blink() called while already in Blink mode");
    return;
  }
  blinkColor = _color;
  blinkSpeed = _speed;
  blinkCount = _count;

  _UpdateModeHistory(mode);
  _SetMode(RGB_MODE::Blink);
}