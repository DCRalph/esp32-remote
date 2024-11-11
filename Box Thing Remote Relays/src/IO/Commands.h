#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/LCD.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"



extern void runTestProcedure();
extern void fireRelays(bool *relayToFire);


extern int parseCommand(fullPacket *fp);
