

#pragma once

#include "config.h"
#include <map>

/**
 * @enum ArmedState
 * @brief Enumeration representing the armed state of the system.
 *
 * This enumeration defines the possible states of the system:
 * - LOCKED: The system is locked and cannot be armed.
 * - DISARMED: The system is not armed.
 * - READY: The system is ready to be armed. eg the switch is on but the remote is not armed.
 * - ARMED: The system is armed.
 */
enum class ArmedState
{
  LOCKED,
  DISARMED,
  READY,
  ARMED
};

static const std::map<ArmedState, String> ArmedStateMap = {
    {ArmedState::LOCKED, "LOCKED"},
    {ArmedState::DISARMED, "DISARMED"},
    {ArmedState::READY, "READY"},
    {ArmedState::ARMED, "ARMED"}};

class Armed
{
private:
  bool locked;
  bool armedSW;
  bool armedRemote;

  ArmedState state;
  ArmedState lastState;

  void (*onStateChange)(ArmedState);

public:
  Armed();

  void update();

  ArmedState get();
  String getStateString();

  void setOnStateChange(void (*callback)(ArmedState));

  void setSW(bool state);
  void setRemote(bool state);
  void setLocked(bool state);
};

extern Armed armed;