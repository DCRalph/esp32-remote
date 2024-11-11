#pragma once

#include <Arduino.h>

#include "Experiment.h"

class test35 : public Experiment
{
public:
  test35();

  void setup() override;
  void loop() override;

  unsigned long startTime = 0;
};

test35::test35()
{
  name = "Test 35";
  stage = 1;
  loopSpeed = 100;
  saveDataSpeed = 30000; // defualt 10000
  saveDataFlag = true;
  duration = 1000 * 60 * 60 * 23; // 23 hours
}

void test35::setup()
{
  startTime = millis();

  coPro->heaterTargetTempDevice.set(35);
  coPro->heaterPIDEnabledDevice.set(true);
}

void test35::loop()
{

  if (millis() - startTime > duration)
  {
    stage = 2;

    complete();
  }
}