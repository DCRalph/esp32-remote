#include "GPIO.h"

GpIO led(LED_PIN, Output, HIGH);

GpIO_RGB btnLed(BTN_LED_R_PIN, BTN_LED_G_PIN, BTN_LED_B_PIN, LOW);

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
}

GpIO::GpIO(uint8_t _pin, PinMode _mode, bool _activeState)
{
  pin = _pin;
  mode = _mode;
  activeState = _activeState;
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

void GpIO::Write(uint8_t _value)
{
  if (mode == Input)
    return;
  digitalWrite(pin, _value);
}

void GpIO::PWM(uint8_t _value)
{
  if (mode != Output)
    return;
  if (activeState == LOW)
    _value = 255 - _value;
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

  btnLed.init();

  Serial.println("\t[INFO] [IO] Pins configured.");
  Serial.println();
}

GpIO_RGB::GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB)
{
  R = GpIO(_pinR, Output);
  G = GpIO(_pinG, Output);
  B = GpIO(_pinB, Output);

  R.Off();
  G.Off();
  B.Off();
}

GpIO_RGB::GpIO_RGB(uint8_t _pinR, uint8_t _pinG, uint8_t _pinB, bool _activeState)
{
  R = GpIO(_pinR, Output, _activeState);
  G = GpIO(_pinG, Output, _activeState);
  B = GpIO(_pinB, Output, _activeState);

  R.Off();
  G.Off();
  B.Off();
}

void GpIO_RGB::_SetColor(uint8_t _r, uint8_t _g, uint8_t _b)
{
  R.PWM(_r);
  G.PWM(_g);
  B.PWM(_b);

  ESP_LOGI("RGB", "Color set to %d %d %d", _r, _g, _b);
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
    StartRainbow();
  else
    StopRainbow();

  if (mode == RGB_MODE::Pulsing)
    StartPulsing();
  else
    StopPulsing();

  if (mode == RGB_MODE::Blink)
    StartBlink();

  // #### RGB_MODE::Manual ####
  if (mode == RGB_MODE::Manual)
    _SetColor(prevManualColor);
  else
    _SetColor(0, 0, 0);
}

void GpIO_RGB::StartRainbow()
{
  if (rainbowHandle == NULL)
    xTaskCreate([](void *pvParameters)
                {
                  GpIO_RGB *rgb = (GpIO_RGB *)pvParameters;
                  while (true)
                  {
                    rgb->Rainbow();
                  }
                  //
                },
                "Rainbow", 4096, this, 1, &rainbowHandle);
}

void GpIO_RGB::StopRainbow()
{
  if (rainbowHandle != NULL)
  {
    vTaskDelete(rainbowHandle);
    rainbowHandle = NULL;
  }
}

void GpIO_RGB::Rainbow()
{
  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(255 - i, i, 0);                // Red to Green
    vTaskDelay(pdMS_TO_TICKS(rainbowSpeed)); // Give time between color transitions
  }

  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(0, 255 - i, i);                // Green to Blue
    vTaskDelay(pdMS_TO_TICKS(rainbowSpeed)); // Delay to prevent too fast transition
  }

  for (uint8_t i = 0; i < 255; i++)
  {
    _SetColor(i, 0, 255 - i);                // Blue to Red
    vTaskDelay(pdMS_TO_TICKS(rainbowSpeed)); // Smooth transition
  }
}

void GpIO_RGB::StartPulsing()
{
  if (pulsingHandle == NULL)
    xTaskCreate([](void *pvParameters)
                {
                  GpIO_RGB *rgb = (GpIO_RGB *)pvParameters;
                  while (true)
                  {
                    rgb->Pulsing();
                  }
                  //
                },
                "Pulsing", 4096, this, 1, &pulsingHandle);
}

void GpIO_RGB::StopPulsing()
{
  if (pulsingHandle != NULL)
  {
    vTaskDelete(pulsingHandle);
    pulsingHandle = NULL;
  }
}

void GpIO_RGB::Pulsing()
{
  uint8_t r = (pulsingColor >> 16) & 0xFF;
  uint8_t g = (pulsingColor >> 8) & 0xFF;
  uint8_t b = pulsingColor & 0xFF;

  // Fade from the original color to black
  for (uint8_t i = 0; i < 255; i++)
  {
    uint8_t red = r - (r * i / 255);
    uint8_t green = g - (g * i / 255);
    uint8_t blue = b - (b * i / 255);

    // ESP_LOGI("RGB", "Pulsing down: %d %d %d", red, green, blue);

    _SetColor(red, green, blue);
    vTaskDelay(pdMS_TO_TICKS(pulsingSpeed)); // Delay for smooth fading
  }

  // Fade from black back to the original color
  for (uint8_t i = 0; i < 255; i++)
  {
    uint8_t red = (r * i / 255);
    uint8_t green = (g * i / 255);
    uint8_t blue = (b * i / 255);

    // ESP_LOGI("RGB", "Pulsing up: %d %d %d", red, green, blue);

    _SetColor(red, green, blue);
    vTaskDelay(pdMS_TO_TICKS(pulsingSpeed)); // Delay for smooth fading
  }
}

void GpIO_RGB::StartBlink()
{
  if (blinkHandle == NULL)
    xTaskCreate([](void *pvParameters)
                {
                  GpIO_RGB *rgb = (GpIO_RGB *)pvParameters;

                  vTaskDelay(pdMS_TO_TICKS(5));

                  rgb->Blink();

                  rgb->SetPrevMode();

                  rgb->blinkHandle = NULL;

                  vTaskDelete(NULL);
                  //
                },
                "Blink", 4096, this, 1, &blinkHandle);
}

void GpIO_RGB::StopBlink()
{
  if (blinkHandle != NULL)
  {
    vTaskDelete(blinkHandle);
    blinkHandle = NULL;
  }
}

void GpIO_RGB::Blink()
{
  for (uint8_t i = 0; i < blinkCount; i++)
  {
    _SetColor(blinkColor);
    vTaskDelay(pdMS_TO_TICKS(blinkSpeed)); // Delay for on time

    _SetColor(0, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(blinkSpeed)); // Delay for off time
  }
}

void GpIO_RGB::init()
{
  R.init();
  G.init();
  B.init();
}
// ################################
// ############ Manual ############
// ################################

void GpIO_RGB::SetColor(uint8_t _r, uint8_t _g, uint8_t _b)
{
  if (mode != RGB_MODE::Manual)
  {
    ESP_LOGI("RGB", "SetColor() called while not in Manual mode");
    return;
  }

  prevManualColor = (_r << 16) | (_g << 8) | _b;

  ESP_LOGI("RGB", "Color set to %d %d %d", _r, _g, _b);

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

// ################################
// ########### RGB_MODE ###########
// ################################

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

  ESP_LOGI("RGB", "Mode changed to %d", _newMode);
  _UpdateModeHistory(mode);
  _SetMode(_newMode);
}

void GpIO_RGB::SetPrevMode()
{
  if (modeHistory.size() == 0)
  {
    ESP_LOGI("RGB", "SetPrevMode() called with no history");
    mode = RGB_MODE::Manual;
    return;
  }

  ESP_LOGI("RGB", "[PREV] Mode changed to %d", modeHistory.back());

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
