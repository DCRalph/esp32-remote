#include "Commands.h"

void parseCommand(fullPacket *fp)
{
  // char buf[32];
  // sprintf(buf, "Command: 0x%02X from: %02X:%02X:%02X:%02X:%02X:%02X", wireless.lastRecvPacket.p.type, wireless.lastRecvPacket.mac[0], wireless.lastRecvPacket.mac[1], wireless.lastRecvPacket.mac[2], wireless.lastRecvPacket.mac[3], wireless.lastRecvPacket.mac[4], wireless.lastRecvPacket.mac[5]);

  if (fp->p.type == DOOR_LOCK_CMD)
  {
    if (fp->p.data[0] == 0x01)
      Serial.println("Unlocking Doors");
    else
      Serial.println("Locking Doors");
  }
  else
  {
    Serial.println("Unknown Command");

    for (int i = 0; i < fp->p.len; i++)
    {
      Serial.print(fp->p.data[i], HEX);
      Serial.print(" ");
    }
  }

}