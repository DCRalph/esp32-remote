#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/GPIO.h"
#include "IO/Wireless.h"

extern void lockDoor();
extern void unlockDoor();

extern int parseCommand(fullPacket *fp);
