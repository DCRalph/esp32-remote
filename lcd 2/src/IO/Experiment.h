#pragma once

#include <Arduino.h>
#include "config.h"
// #include "IO/DB.h"
#include "IO/Battery.h"
#include "IO/CoProcessor.h"
#include "ArduinoJson.h"

#include <vector>
#include <algorithm>

class Experiment
{
public:

  // Experiment(String _uniqueId);

  String name = "Experiment";
  String uniqueId = "N/A";
  int id = -1;
  
  int stage = -1;
  bool setupDone = false;
  bool done = false;
  unsigned long startTime = 0;
  unsigned long duration = 0;
  unsigned long lastSend = 0;
  unsigned long lastLoop = 0;

  long loopSpeed = 1000;
  long saveDataSpeed = 10000;
  bool saveDataFlag = false;

  void setUID(String _uniqueId);

  CoProcessor *coPro = nullptr; // Pointer to the assigned coprocessor

  virtual void setup();
  virtual void loop();

  void start();
  void stop();
  void complete();
};

// Manages running experiments
class ExperimentManager
{
private:
  bool setupDone = false;
  unsigned long lastLoop = 0;

  // Vector to store multiple experiments
  std::vector<Experiment *> experiments;

  // Pointer to the coprocessor currently being indicated
  CoProcessor *indicatedCoproc = nullptr;

public:
  ExperimentManager();

  void init();

  void mainLoop();

  // Start an experiment and return the assigned coprocessor ID or -1 if none available
  int start(Experiment *experiment, int id);
  int start(Experiment *experiment);

  // Stop a specific experiment
  void stop(Experiment *experiment);

  // Clear all experiments
  void clearExperiments();

  // Get the list of experiments
  std::vector<Experiment *> &getExperiments();

  // Send and save data for an experiment
  void sendData(Experiment *experiment);
  void saveData(Experiment *experiment);

  // Check if there are any experiments running
  bool hasExperiments();

  // Indicate the next available coprocessor
  void indicateNextAvailableCoproc();

// Get the coprocessor currently being indicated
  CoProcessor* getIndicatedCoproc();


  // Clear the indicator
  void clearIndicator();
};

extern ExperimentManager experimentManager;