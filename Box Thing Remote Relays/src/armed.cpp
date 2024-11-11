#include "armed.h"

Armed::Armed()
{
  armedSW = false;
  armedRemote = false;
}

void Armed::update()
{
  if (locked)
  {
    state = ArmedState::LOCKED;
  }
  else if (armedSW && armedRemote)
  {
    state = ArmedState::ARMED;
  }
  else if (armedSW)
  {
    state = ArmedState::READY;
  }
  else
  {
    state = ArmedState::DISARMED;
  }

  if (state != lastState)
  {
    lastState = state;
    if (onStateChange != nullptr)
    {
      onStateChange(state);
    }
  }
}

ArmedState Armed::get()
{
  return state;
}

String Armed::getStateString()
{
  return ArmedStateMap.at(state);
}

void Armed::setOnStateChange(void (*callback)(ArmedState))
{
  onStateChange = callback;
}

void Armed::setSW(bool state)
{
  armedSW = state;
}

void Armed::setRemote(bool state)
{
  armedRemote = state;
}

void Armed::setLocked(bool state)
{
  locked = state;
}

Armed armed;