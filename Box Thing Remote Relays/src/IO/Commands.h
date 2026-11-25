#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/LCD.h"
#include "IO/GPIO.h"

extern void runTestProcedure();
// Bit n fires relay n + 1 for 100 ms; refused while DISARMED or LOCKED.
extern void fireRelays(uint8_t relayMask);

// Answers the Box Thing v2 fire box screen. Call once DirectComms is attached.
extern void registerCommands();
// Set before each receive drain: true after a loop stall (over 500 ms), so queued frames are stale.
extern bool rxStale;
