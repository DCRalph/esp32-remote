// main.cpp

// #include "secrets.h"
#include "config.h"
#include "IO/Wireless.h"
#include "Application.h"

Application *app;

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  app = Application::getInstance();

  if (!app)
  {
    Serial.println("Failed to create Application instance.");
    return;
  }

  app->begin();
  // app->setTestMode(true);

  xTaskCreatePinnedToCore(
      [](void *pvParameters)
      {
        led.On();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        led.Off();

        vTaskDelete(NULL);
      },
      "startup_blink", 10000, NULL, 1, NULL, 1);
}

void loop()
{
  unsigned long currentTime = millis();
  wireless.loop();

  app->update();
}
