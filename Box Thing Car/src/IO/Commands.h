#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/GPIO.h"

extern void lockDoor();
extern void unlockDoor();

// Answers the CarRelays channels from lcd 2 and oled 3; onCommand runs after each one.
extern void registerCommands(void (*onCommand)());
