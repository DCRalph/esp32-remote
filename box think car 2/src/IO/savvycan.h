#pragma once

#include "config.h"
#include "IO/can.h"

extern int savvyCanInit();

extern int savvyCanHandleSerial(String command);

extern int savvyCanWrite(String command);    // Function to write standard CAN frame
extern int savvyCanWriteEXT(String command); // Function to write extended CAN frame

extern int savvyCanSend(canFrame *frame); // Function to send CAN frame to savvycan
