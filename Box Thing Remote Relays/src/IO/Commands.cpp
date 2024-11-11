#include "Commands.h"

static TaskHandle_t testProdcudureTaskHandle;
static TaskHandle_t fireTaskHandle;

static void sendAck(uint8_t type, uint8_t *data, uint8_t len, uint8_t dest[6])
{
  fullPacket fp;
  fp.direction = PacketDirection::SEND;
  fp.p.type = type;
  fp.p.len = len;
  memcpy(fp.p.data, data, len);

  memcpy(fp.mac, dest, 6);

  wireless.send(&fp);
}

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
  bool relay1Fire = ((bool *)pvParameters)[0];
  bool relay2Fire = ((bool *)pvParameters)[1];
  bool relay3Fire = ((bool *)pvParameters)[2];
  bool relay4Fire = ((bool *)pvParameters)[3];
  bool relay5Fire = ((bool *)pvParameters)[4];
  bool relay6Fire = ((bool *)pvParameters)[5];
  bool relay7Fire = ((bool *)pvParameters)[6];
  bool relay8Fire = ((bool *)pvParameters)[7];

  if (relay1Fire)
    relay1.On();
  if (relay2Fire)
    relay2.On();
  if (relay3Fire)
    relay3.On();
  if (relay4Fire)
    relay4.On();
  if (relay5Fire)
    relay5.On();
  if (relay6Fire)
    relay6.On();
  if (relay7Fire)
    relay7.On();
  if (relay8Fire)
    relay8.On();

  vTaskDelay(pdMS_TO_TICKS(100));

  if (relay1Fire)
    relay1.Off();
  if (relay2Fire)
    relay2.Off();
  if (relay3Fire)
    relay3.Off();
  if (relay4Fire)
    relay4.Off();
  if (relay5Fire)
    relay5.Off();
  if (relay6Fire)
    relay6.Off();
  if (relay7Fire)
    relay7.Off();
  if (relay8Fire)
    relay8.Off();

  fireTaskHandle = NULL;
  vTaskDelete(NULL);
}

void fireRelays(bool *relayToFire)
{

  if (armed.get() == ArmedState::DISARMED || armed.get() == ArmedState::LOCKED)
  {
    Serial.println("Not Armed");
    return;
  }

  if (fireTaskHandle == NULL)
    xTaskCreate(fireTask, "Fire Task", 4096, relayToFire, 1, &fireTaskHandle);
}

static void fireCmd(fullPacket *fp)
{
  if (armed.get() != ArmedState::ARMED)
  {
    Serial.println("Not Armed");
    return;
  }

  if (fp->p.len != 8)
  {
    Serial.println("Invalid FIRE command");
    return;
  }

  bool relay1 = fp->p.data[0] == 0x01;
  bool relay2 = fp->p.data[1] == 0x01;
  bool relay3 = fp->p.data[2] == 0x01;
  bool relay4 = fp->p.data[3] == 0x01;
  bool relay5 = fp->p.data[4] == 0x01;
  bool relay6 = fp->p.data[5] == 0x01;
  bool relay7 = fp->p.data[6] == 0x01;
  bool relay8 = fp->p.data[7] == 0x01;

  bool relayToFire[8] = {relay1, relay2, relay3, relay4, relay5, relay6, relay7, relay8};

  fireRelays(relayToFire);
}

int parseCommand(fullPacket *fp)
{

  if (fp->p.type == CMD_PING)
  {
    remoteConnected = true;
    lastRemotePing = millis();
    sendAck(CMD_PING, NULL, 0, fp->mac);
  }

  else if (fp->p.type == CMD_TEST)
  {
    runTestProcedure();
  }

  // ################### ARM ###################
  else if (fp->p.type == CMD_ARM)
  {
    armed.setRemote(true);
  }

  // ################### DISARM ###################
  else if (fp->p.type == CMD_DISARM)
  {
    armed.setRemote(false);
  }

  // ################### FIRE ###################
  else if (fp->p.type == CMD_FIRE)
  {
    fireCmd(fp);
  }

  // ################### UNKNOW COMMAND ###################
  else
  {
    Serial.println("######### Unknown Command #########");

    Serial.print("Type: ");
    Serial.print(fp->p.type);

    Serial.print(" - Len: ");
    Serial.println(fp->p.len);

    Serial.println("Data: ");
    for (int i = 0; i < fp->p.len; i++)
    {
      Serial.print(fp->p.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    Serial.println("######################");
  }
  return -1;
}
