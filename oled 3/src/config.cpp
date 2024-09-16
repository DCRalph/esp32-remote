#include "config.h"

static const char *TAG = "Config";

Preferences preferences;
esp_sleep_wakeup_cause_t wakeup_reason;

bool globalRelay1 = false;
bool globalRelay2 = false;
bool globalRelay3 = false;
bool globalRelay4 = false;
bool globalRelay5 = false;
bool globalRelay6 = false;

void initConfig()
{
  Serial.begin(BAUD_RATE);
  // Serial.println("[INFO] [CONFIG] Starting...");
  ESP_LOGI(TAG, "Starting...");

  // Serial.println("[INFO] [CONFIG] Preferences...");
  ESP_LOGI(TAG, "Preferences...");
  preferences.begin("p", false);

  long bootCount = preferences.getLong("bootCount", 0);
  bootCount++;
  preferences.putLong("bootCount", bootCount);
}

void configureDeepSleep()
{
  // Create a bitmask for GPIO12 and GPIO13
  uint64_t wakeupMask = (1ULL << BTN_UP_PIN) | (1ULL << BTN_DOWN_PIN);

  rtc_gpio_pullup_en(BTN_UP_GPIO);
  rtc_gpio_pullup_en(BTN_DOWN_GPIO);

  esp_sleep_enable_ext1_wakeup(wakeupMask, ESP_EXT1_WAKEUP_ALL_LOW);
}

void deepSleepSetup()
{

  wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
  {
    bool button1Pressed = (digitalRead(BTN_UP_PIN) == LOW);
    bool button2Pressed = (digitalRead(BTN_DOWN_PIN) == LOW);

    // Check if both buttons are pressed
    do
    {
      button1Pressed = (digitalRead(BTN_UP_PIN) == LOW);
      button2Pressed = (digitalRead(BTN_DOWN_PIN) == LOW);

      if (!button1Pressed && !button2Pressed)
        esp_deep_sleep_start();

      if (button1Pressed && button2Pressed)
        break;

    } while (button1Pressed || button2Pressed);

    do
    {
      button1Pressed = (digitalRead(BTN_UP_PIN) == LOW);
      button2Pressed = (digitalRead(BTN_DOWN_PIN) == LOW);
    } while (button1Pressed || button2Pressed);
  }
  else
  {
    esp_deep_sleep_start();
  }
}