#include "Commands.h"

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

  // ################### RELAYS ###################
  if (fp->p.type >= CMD_RELAY_1_SET && fp->p.type <= CMD_RELAY_8_GET)
  {
    int ret = -1;
    ret = parseForRelay(fp, &relay1, &relay1Busy, CMD_RELAY_1_GET, CMD_RELAY_1_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay2, &relay2Busy, CMD_RELAY_2_GET, CMD_RELAY_2_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay3, &relay3Busy, CMD_RELAY_3_GET, CMD_RELAY_3_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay4, &relay4Busy, CMD_RELAY_4_GET, CMD_RELAY_4_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay5, &relay5Busy, CMD_RELAY_5_GET, CMD_RELAY_5_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay6, &relay6Busy, CMD_RELAY_6_GET, CMD_RELAY_6_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay7, &relay7Busy, CMD_RELAY_7_GET, CMD_RELAY_7_SET);
    if (ret != -1)
      return ret;
    ret = parseForRelay(fp, &relay8, &relay8Busy, CMD_RELAY_8_GET, CMD_RELAY_8_SET);
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
    resp.p.len = 8;
    resp.p.data[0] = relay1.read();
    resp.p.data[1] = relay2.read();
    resp.p.data[2] = relay3.read();
    resp.p.data[3] = relay4.read();
    resp.p.data[4] = relay5.read();
    resp.p.data[5] = relay6.read();
    resp.p.data[6] = relay7.read();
    resp.p.data[7] = relay8.read();

    wireless.send(&resp);
    return 3;
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