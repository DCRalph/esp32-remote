#pragma once

#include "config.h"
#include <ArduinoOTA.h>

#include "IO/Display.h"
#include "Screens/UpdateProgress.h"

extern bool otaSetup;
extern bool otaInProgress;

extern void InitOta();