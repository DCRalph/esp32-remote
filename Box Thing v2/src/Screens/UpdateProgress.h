#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

enum UpdateProgressState
{
  GETTING_READY,
  UPDATING,
  SHOWING_MESSAGE
};

class UpdateProgressScreen : public Screen
{
public:
  UpdateProgressScreen(String _name);

  void draw() override;

  uint8_t progress;
  UpdateProgressState state;

  String msg;

  void setProgress(uint8_t _progress);
  void setMessage(String _msg);
};
