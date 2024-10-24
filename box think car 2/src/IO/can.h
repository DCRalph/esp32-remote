#pragma once

#include "config.h"

#include "mcp_can.h"

#define CAN0_CS 5
#define CAN0_INT 2


extern MCP_CAN CAN0;

extern int canInit();

extern int canNormal();
extern int canSleep();

extern bool canInterupt();

extern int canRead(canFrame *frame);
extern int canSend(canFrame *frame);