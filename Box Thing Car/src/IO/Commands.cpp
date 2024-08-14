#include "Commands.h"

void lockDoor()
{
  // xTaskCreate([](void *pvParameters)
  //             {
  //               relay7.On();
  //               vTaskDelay(500 / portTICK_PERIOD_MS);
  //               relay7.Off();
  //               vTaskDelete(NULL);
  //               //
  //             },
  //             "lockDoor", 1000, &lockDoorHandle, 1, NULL);

  relay7.On();
  delay(500);
  relay7.Off();
}

void unlockDoor()
{
  // xTaskCreate([](void *pvParameters)
  //             {
  //               relay8.On();
  //               vTaskDelay(500 / portTICK_PERIOD_MS);
  //               relay8.Off();
  //               vTaskDelete(NULL);
  //               //
  //             },
  //             "unlockDoor", 1000, &unlockDoorHandle, 1, NULL);

  relay8.On();
  delay(500);
  relay8.Off();
}

static int parseForRelay(fullPacket *fp, GpIO *relay, bool *busy, uint8_t get, uint8_t set)
{
  if (fp->p.type == set)
  {
    if (*busy)
    {
      Serial.println("Relay Busy Pin " + String(relay->getPin()));
      return -1;
    }

    if (fp->p.data[0] == 0x01)
    {
      Serial.println("Relay ON Pin " + String(relay->getPin()));
      relay->On();
    }
    else
    {
      Serial.println("Relay OFF Pin " + String(relay->getPin()));
      relay->Off();
    }

    fullPacket resp;
    memcpy(resp.mac, fp->mac, 6);
    resp.direction = PacketDirection::SEND;
    resp.p.type = get;
    resp.p.len = 1;
    resp.p.data[0] = relay->read();

    wireless.send(&resp);
    return 1;
  }
  else if (fp->p.type == get)
  {
    Serial.println("Relay GET Pin " + String(relay->getPin()));
    fullPacket resp;
    memcpy(resp.mac, fp->mac, 6);
    resp.direction = PacketDirection::SEND;
    resp.p.type = get;
    resp.p.len = 1;
    resp.p.data[0] = relay->read();

    wireless.send(&resp);
    return 2;
  }
  return -1;
}

int parseCommand(fullPacket *fp)
{

  // ################### DOOR LOCK ###################
  if (fp->p.type == CMD_DOOR_LOCK)
  {
    if (fp->p.data[0] == 0x01)
    {
      Serial.println("Unlocking Doors");
      unlockDoor();
      return 1;
    }
    else
    {
      Serial.println("Locking Doors");
      lockDoor();
      return 2;
    }
  }

  // ################### RELAYS ###################
  else if (fp->p.type >= CMD_RELAY_1_SET && fp->p.type <= CMD_RELAY_6_GET)
  {
    int ret = -1;
    ret = parseForRelay(fp, &relay1, &relay1Busy, CMD_RELAY_1_GET, CMD_RELAY_1_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay2, &relay2Busy, CMD_RELAY_2_GET, CMD_RELAY_2_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay3, &relay3Busy, CMD_RELAY_3_GET, CMD_RELAY_3_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay4, &relay4Busy, CMD_RELAY_4_GET, CMD_RELAY_4_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay5, &relay5Busy, CMD_RELAY_5_GET, CMD_RELAY_5_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay6, &relay6Busy, CMD_RELAY_6_GET, CMD_RELAY_6_SET);
    if (ret != -1)
      return ret;
  }

  // ################### GET ALL RELAYS ###################
  else if (fp->p.type == CMD_RELAY_ALL)
  {
    Serial.println("Get All Relays");

    fullPacket resp;
    memcpy(resp.mac, fp->mac, 6);
    resp.direction = PacketDirection::SEND;
    resp.p.type = CMD_RELAY_ALL;
    resp.p.len = 6;
    resp.p.data[0] = relay1.read();
    resp.p.data[1] = relay2.read();
    resp.p.data[2] = relay3.read();
    resp.p.data[3] = relay4.read();
    resp.p.data[4] = relay5.read();
    resp.p.data[5] = relay6.read();

    wireless.send(&resp);
    return 3;
  }

  // ############ RELAY 1 FLASH ############
  else if (fp->p.type == CMD_RELAY_1_FLASH)
  {
    if (relay1Busy)
    {
      Serial.println("Relay 1 Busy");
      return -1;
    }

    relay1Busy = true;

    Serial.println("Relay 1 Flash");

    int count = fp->p.data[0];
    int speed = fp->p.data[1] * 10;

    relay1FlashCount = count;
    relay1FlashDelay = speed;

    // void *pvParameters[2] = {&count, &speed};

    // xTaskCreate([](void *pvParameters)
    //             {
    //               int count = *(int *)((void **)pvParameters)[0];
    //               int speed = *(int *)((void **)pvParameters)[1];

    //               for (int i = 0; i < count; i++)
    //               {
    //                 relay1.On();
    //                 vTaskDelay(speed / portTICK_PERIOD_MS);
    //                 relay1.Off();
    //                 vTaskDelay(speed / portTICK_PERIOD_MS);
    //               }

    //               relay1Busy = false;

    //               vTaskDelete(NULL);
    //               //
    //             },
    //             "Flash", 1000, pvParameters, 1, NULL);
    return 4;
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