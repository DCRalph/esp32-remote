#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/GPIO.h"

// Queue a 500 ms pulse on the lock (relay 7) or unlock (relay 8) relay. Safe from any
// task (Server.cpp's web handlers); doorLoop, on the Arduino loop, drives the relays.
extern void lockDoor();
extern void unlockDoor();
extern void doorLoop();

// Answers the CarRelays channels from lcd 2 and oled 3; onCommand runs after each one.
extern void registerCommands(void (*onCommand)());
