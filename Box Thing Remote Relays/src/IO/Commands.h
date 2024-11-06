#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/GPIO.h"
#include "IO/Wireless.h"

extern int parseCommand(fullPacket *fp);
