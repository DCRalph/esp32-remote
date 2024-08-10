#pragma once

#include "config.h"

class MQTT
{
public:
  MQTT();
  void init();
  void loop();
};

extern MQTT mqtt;