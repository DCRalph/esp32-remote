#pragma once

#include <Arduino.h>

class Timer
{
private:
  String name;
  uint64_t startTime = 0;
  uint64_t endTime = 0;

public:
  enum class TimerAction
  {
    AUTOSTART,
    MANUAL
  };

  Timer(String _name);
  Timer(String _name, TimerAction _action);
  void reset();
  void start();
  void stop();
  void print();
  uint64_t getTime();
};