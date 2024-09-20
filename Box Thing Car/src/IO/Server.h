#pragma once

#include "config.h"

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

#include "IO/Commands.h"

#include "web/html.h"
#include "web/css.h"
#include "web/keypad_js.h"


extern AsyncWebServer server;


void setupServer();

void setupRoutes();