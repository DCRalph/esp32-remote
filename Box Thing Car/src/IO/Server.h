#pragma once

#include "config.h"

#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>

#include "html.h"
#include "IO/Commands.h"


extern AsyncWebServer server;


void setupServer();

void setupRoutes();