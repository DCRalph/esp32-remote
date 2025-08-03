#pragma once

#include "config.h"

// #define CAN_MCP
#define CAN_ESP

//############################

#ifdef CAN_MCP

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

#endif

#ifdef CAN_ESP

#include "driver/twai.h"

#define CAN_TX 7
#define CAN_RX 6
#define POLLING_RATE_MS 1000

extern int canInit();

extern int canNormal();
extern int canSleep();

extern bool canInterupt();

extern int canRead(canFrame *frame);
extern int canSend(canFrame *frame);

#endif