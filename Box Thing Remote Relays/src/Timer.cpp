#include "Timer.h"

Timer::Timer(String _name)
{
  name = _name;
  startTime = 0;
  endTime = 0;
}

Timer::Timer(String _name, TimerAction _action)
{
  name = _name;
  if (_action == TimerAction::AUTOSTART)
  {
    start();
  }
  else if (_action == TimerAction::MANUAL)
  {
    startTime = 0;
    endTime = 0;
  }
  else
  {
    startTime = 0;
    endTime = 0;
    Serial.println("Invalid TimerAction");
  }
}

void Timer::reset()
{
  startTime = 0;
  endTime = 0;
}

void Timer::start()
{
  startTime = millis();
}

void Timer::stop()
{
  endTime = millis();
}

void Timer::print()
{
  uint64_t elapsedTime = getTime();

  Serial.println("######## TIMER INFO ########");
  Serial.print("# ");
  Serial.print(name);
  Serial.print(": ");
  Serial.print(elapsedTime);
  Serial.println(" ms");
  Serial.println("#############################");
}

uint64_t Timer::getTime()
{
  if (endTime == 0)
  {
    return millis() - startTime;
  }
  else
  {
    return endTime - startTime;
  }
}
