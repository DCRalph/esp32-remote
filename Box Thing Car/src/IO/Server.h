#pragma once

#include "config.h"

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

#include "IO/Commands.h"

#include "html.h"
#include "css.h"
#include "keypad_js.h"


extern AsyncWebServer server;


void setupServer();

void setupRoutes();