#include "can.h"

const static char *tag = "CAN";

MCP_CAN CAN0(CAN0_CS);

int canInit()
{

  // Initialize MCP2515 at 500kbps
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
  {
    ESP_LOGI(tag, "MCP2515 Initialized Successfully!");
  }
  else
  {
    ESP_LOGE(tag, "Error Initializing MCP2515...");
    return -1;
  }

  pinMode(CAN0_INT, INPUT);
  return 0;
}

int canNormal()
{
  return CAN0.setMode(MCP_NORMAL);
}

int canSleep()
{
  return CAN0.setMode(MCP_SLEEP);
}

bool canInterupt()
{
  return !digitalRead(CAN0_INT);
}

int canRead(canFrame *frame)
{

  if (!digitalRead(CAN0_INT))
  {
    if (CAN0.readMsgBuf(&frame->id, &frame->extFlag, &frame->len, frame->data) == CAN_OK)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return -1;
}

int canSend(canFrame *frame)
{
  return CAN0.sendMsgBuf(frame->id, frame->extFlag, frame->len, frame->data);
}