#include "Commands.h"

#include <RemoteProtocol.h>

static TaskHandle_t testProdcudureTaskHandle;
static TaskHandle_t fireTaskHandle;

static void testProdcudureTask(void *pvParameters)
{
  pwrLed.SetMode(RGB_MODE::OVERRIDE);
  armedLed.SetMode(RGB_MODE::OVERRIDE);

  uint16_t delay = 200;

  for (int i = 0; i < 10; i++)
  {

    pwrLed.SetColorOveride(0, 0, 0);
    armedLed.SetColorOveride(0, 0, 0);
    lcd.lcd.noBacklight();

    vTaskDelay(pdMS_TO_TICKS(delay));

    pwrLed.SetColorOveride(0, 0, 255);
    armedLed.SetColorOveride(0, 0, 255);
    lcd.lcd.backlight();

    vTaskDelay(pdMS_TO_TICKS(delay));
  }

  pwrLed.SetPrevMode();
  armedLed.SetPrevMode();

  testProdcudureTaskHandle = NULL;
  vTaskDelete(NULL);
}

void runTestProcedure()
{
  if (testProdcudureTaskHandle == NULL)
    xTaskCreate(testProdcudureTask, "Test Procedure Task", 4096, NULL, 1, &testProdcudureTaskHandle);
}

static void fireTask(void *pvParameters)
{
  const uint8_t relayMask = static_cast<uint8_t>(reinterpret_cast<uintptr_t>(pvParameters));

  for (int i = 0; i < 8; i++)
    if (relayMask & (1u << i))
      relayMap[i]->On();

  vTaskDelay(pdMS_TO_TICKS(100));

  for (int i = 0; i < 8; i++)
    if (relayMask & (1u << i))
      relayMap[i]->Off();

  fireTaskHandle = NULL;
  vTaskDelete(NULL);
}

void fireRelays(uint8_t relayMask)
{
  if (armed.get() == ArmedState::DISARMED || armed.get() == ArmedState::LOCKED)
  {
    Serial.println("Not Armed");
    return;
  }

  if (fireTaskHandle == NULL)
    xTaskCreate(fireTask, "Fire Task", 4096, reinterpret_cast<void *>(static_cast<uintptr_t>(relayMask)), 1, &fireTaskHandle);
}

void registerCommands()
{
  using namespace RemoteProtocol::FireBox;

  ping().onRequest([](const TransportAddress &, const Ping &, Pong &)
                   {
                     remoteConnected = true;
                     lastRemotePing = millis();
                     return true; });

  test().onReceive([](const TransportAddress &, const Test &)
                   { runTestProcedure(); });

  arm().onReceive([](const TransportAddress &, const Arm &msg)
                  { armed.setRemote(msg.armed != 0); });

  // The remote must be armed too, not only the switch (READY).
  fire().onReceive([](const TransportAddress &, const Fire &msg)
                   {
                     if (armed.get() == ArmedState::ARMED)
                       fireRelays(msg.relayMask);
                     else
                       Serial.println("Not Armed"); });
}
