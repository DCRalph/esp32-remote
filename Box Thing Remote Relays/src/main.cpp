
#include "secrets.h"
#include "config.h"
#include "IO/Wireless.h"
#include "IO/Commands.h"
#include "IO/LCD.h"

#include "Screens/HomeScreen.h"

uint64_t ledBlinkMs = 0;
uint8_t blinks = 0;

bool lastArmed = false;

void lcdTask(void *pvParameters)
{
  for (;;)
  {
    lcd.loop();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// lcd task handle
TaskHandle_t lcdTaskHandle = NULL;

void espNowRecvCb(fullPacket *fp)
{
  int ret = parseCommand(fp);

  blinks = ret == -1 ? 2 : 4;

  ledBlinkMs = millis();
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();
  lcd.init();

  wireless.setup();
  wireless.setRecvCb(espNowRecvCb);

  led.On();
  delay(500);
  led.Off();

  xTaskCreatePinnedToCore(
      lcdTask,   /* Task function. */
      "lcdTask", /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &lcdTaskHandle, 0);
}

void loop()
{

  if (armed.read() != lastArmed)
  {
    lastArmed = armed.read();

    Serial.print("Armed: ");
    if (lastArmed)
      Serial.println("YES");
    else
      Serial.println("NO");
    relay8.Write(lastArmed);

    if (lastArmed)
      lcd.setScreen(&HomeScreen);
    else
      lcd.setScreen(nullptr);
  }

  if (blinks > 0)
  {
    if (millis() - ledBlinkMs > 100)
    {
      ledBlinkMs = millis();
      led.Toggle();
      blinks--;
    }

    if (blinks == 0)
      led.Off();
  }

  if (Serial.available() > 0)
  {
    String input = Serial.readString();
    while (Serial.available() > 0)
      input += Serial.readString();
    if (input == "reboot")
      ESP.restart();
    else if (input == "getip")
      Serial.println(WiFi.localIP());
    else if (input == "getmac")
      Serial.println(WiFi.macAddress());

    else if (input == "getall")
    {
      for (auto relay : relayMap)
      {
        Serial.print("Relay ");
        Serial.print(relay->getPin());
        Serial.print(" - ");
        Serial.println(relay->read());
      }
    }

    else if (input.startsWith("set"))
    {
      String pin = input.substring(3, 4);
      String value = input.substring(5, 6);
      int pinInt = pin.toInt();
      int valueInt = value.toInt();
      relayMap[pinInt]->Write(valueInt);
    }

    else if (input == "getarmed")
    {
      Serial.println(armed.read());
    }

    else if (input == "disarmed")
    {
      armed.Off();
    }

    else
      Serial.println("[INFO] [SERIAL] " + input);
  }
}
