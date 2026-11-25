#pragma once

#include <Arduino.h>
#include "config.h"

#include "IO/GPIO.h"

// Answers the Servo channel from Box Thing v2; onCommand runs after each message.
extern void registerCommands(void (*onCommand)());
