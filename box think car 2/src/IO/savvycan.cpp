#include "savvycan.h"

static String padHex(unsigned long num, uint8_t width)
{
  String hexStr = String(num, HEX);
  while (hexStr.length() < width)
  {
    hexStr = "0" + hexStr;
  }
  return hexStr;
}

int savvyCanInit()
{
  return 0;
}

int savvyCanHandleSerial(String command)
{
  char cmd = command.charAt(0);

  switch (cmd)
  {
  case 'O': // Open CAN channel
    return canNormal();
    Serial.print("\r");
    break;

  case 'C': // Close CAN channel
    return canSleep();
    Serial.print("\r");
    break;

  case 't':
    return savvyCanWrite(command);
    break;

  case 'T':
    return savvyCanWriteEXT(command);
    break;

    // Implement other SLCAN commands as needed

  default:
    Serial.print("\a"); // Bell character for error
    break;
  }

  return 0;
}

int savvyCanWrite(String command)
{
  // Command format: tIIILDDDDDDDD
  // example t7df030205
  // III: 3-digit standard CAN ID in hex
  unsigned long canId = strtoul(command.substring(1, 4).c_str(), NULL, 16);
  uint8_t len = strtoul(command.substring(4, 5).c_str(), NULL, 16);
  uint8_t data[8];

  for (uint8_t i = 0; i < len; i++)
  {
    data[i] = strtoul(command.substring(5 + i * 2, 7 + i * 2).c_str(), NULL, 16);
  }

  canFrame frame;
  frame.id = canId;
  frame.extFlag = 0;
  frame.len = len;
  memcpy(frame.data, data, len);

  return canSend(&frame);
}

int savvyCanWriteEXT(String command)
{
  // Command format: TIIIIIIIIDLDDDDDDDD
  // IIIIIIII: 8-digit extended CAN ID in hex
  unsigned long canId = strtoul(command.substring(1, 9).c_str(), NULL, 16);
  uint8_t len = strtoul(command.substring(9, 10).c_str(), NULL, 16);
  uint8_t data[8];

  for (uint8_t i = 0; i < len; i++)
  {
    data[i] = strtoul(command.substring(10 + i * 2, 12 + i * 2).c_str(), NULL, 16);
  }

  canFrame frame;
  frame.id = canId;
  frame.extFlag = 1;
  frame.len = len;
  memcpy(frame.data, data, len);

  return canSend(&frame);
}

int savvyCanSend(canFrame *frame)
{

  if (frame->extFlag == 0)
  {
    // Standard frame
    Serial.print('t');
    Serial.print(padHex(frame->id, 3));
  }
  else
  {
    // Extended frame
    Serial.print('T');
    Serial.print(padHex(frame->id, 8));
  }

  Serial.print(frame->len, HEX);

  for (int i = 0; i < frame->len; i++)
  {
    Serial.print(padHex(frame->data[i], 2));
  }

  Serial.print('\r');

  return 0;
}
