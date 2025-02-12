// main.cpp


// #include "secrets.h"
#include "config.h"
#include "IO/Wireless.h"
#include "Application.h"


Application* app;

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

  led.On();
  delay(500);
  led.Off();
}

void loop()
{
  unsigned long currentTime = millis();
  wireless.loop();

  app->update();

}
