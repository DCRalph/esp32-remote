#include "Commands.h"

static int parseForRelay(fullPacket *fp, GpIO *relay, uint8_t get, uint8_t set)
{
  if (fp->p.type == set)
  {
    if (fp->p.data[0] == 0x01)
    {
      Serial.println("Relay ON Pin" + String(relay->getPin()));
      relay->On();
    }
    else
    {
      Serial.println("Relay OFF Pin" + String(relay->getPin()));
      relay->Off();
    }

    fullPacket resp;
    memcpy(resp.mac, fp->mac, 6);
    resp.direction = PacketDirection::SEND;
    resp.p.type = set;
    resp.p.len = 1;
    resp.p.data[0] = relay->read();

    wireless.send(&resp);
    return 1;
  }
  else if (fp->p.type == get)
  {
    Serial.println("Relay GET Pin" + String(relay->getPin()));
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
      return 1;
    }
    else
    {
      Serial.println("Locking Doors");
      return 2;
    }
  }

  // ################### RELAYS ###################
  else if (fp->p.type >= CMD_RELAY_1_SET && fp->p.type <= CMD_RELAY_6_GET)
  {
    int ret = -1;
    ret = parseForRelay(fp, &relay1, CMD_RELAY_1_GET, CMD_RELAY_1_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay2, CMD_RELAY_2_GET, CMD_RELAY_2_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay3, CMD_RELAY_3_GET, CMD_RELAY_3_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay4, CMD_RELAY_4_GET, CMD_RELAY_4_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay5, CMD_RELAY_5_GET, CMD_RELAY_5_SET);
    if (ret != -1)
      return ret;
    parseForRelay(fp, &relay6, CMD_RELAY_6_GET, CMD_RELAY_6_SET);
    if (ret != -1)
      return ret;
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