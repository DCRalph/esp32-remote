#include "Experiment.h"
// #include <IO/Files.h>

ExperimentManager experimentManager = ExperimentManager();

// Experiment::Experiment(String _uniqueId)
// {
//   uniqueId = _uniqueId;
// }

void Experiment::setUID(String _uniqueId)
{
  uniqueId = _uniqueId;
}

void Experiment::setup()
{
  // Experiment-specific setup code
}

void Experiment::loop()
{
  // Experiment-specific loop code
}

void Experiment::start()
{
  logger.println("[INFO] [" + name + "] Started");
  startTime = millis();
  stage = 1;
  setupDone = false;
}

void Experiment::stop()
{
  logger.println("[INFO] [" + name + "] Stopped");

  if (coPro != nullptr)
  {
    coPro->whiteLED.off();
    coPro->uvcLED.off();
    coPro->uvaLED.off();

    coPro->heaterTargetTempDevice.set(0);
    coPro->heaterPIDEnabledDevice.set(false);
    coPro->heaterPWMDevice.set(0);
  }

#ifdef OLD_LED
  Wled.off();
  UVled.off();
#endif

  done = true;
}

void Experiment::complete()
{
  stop();

#if defined(ENABLE_BUZZER)
  buzzer.playMelody(buzzer.Success);
#endif

  logger.println("[INFO] [" + name + "] Completed");
  done = true;

  // EndExperimentFile(id);

  // Notify the experiment manager to stop this experiment
  experimentManager.stop(this);
}

// #############################################################
// #############################################################

ExperimentManager::ExperimentManager()
{
}

void ExperimentManager::init()
{
  logger.println("\t[INFO] [EXPERIMENT MANAGER] Initialized");
}

int ExperimentManager::start(Experiment *experiment, int id)
{

  if (!coProcessor[id]->getInUse())
  {
    // Assign coprocessor to the experiment
    coProcessor[id]->setInUse(true);
    experiment->coPro = coProcessor[id];

    // Initialize coprocessor if necessary
    // coProcessor[i]->init();

    // Set up experiment properties
    int experIdx = preferences.getUInt("experIdx", 0);
    experIdx++;
    preferences.putUInt("experIdx", experIdx);

    experiment->id = experIdx;

    experiment->lastSend = millis();
    experiment->start();
    experiment->setupDone = false;
    experiment->done = false;
    experiment->startTime = millis();

    // Add to the list of running experiments
    experiments.push_back(experiment);

    if (indicatedCoproc == coProcessor[id])
    {
      clearIndicator();
    }

    return coProcessor[id]->getId(); // Return the coprocessor ID
  }

  // No available coprocessor found
  return -1;
}

int ExperimentManager::start(Experiment *experiment)
{
  // Find the next available coprocessor
  for (int i = 0; i < NUM_COPROS; i++)
  {
    if (!coProcessor[i]->getInUse())
    {
      return start(experiment, i);
    }
  }

  // No available coprocessor found
  return -1;
}

void ExperimentManager::stop(Experiment *experiment)
{
  // Find the experiment in the list
  auto it = std::find(experiments.begin(), experiments.end(), experiment);
  if (it != experiments.end())
  {
    // Mark experiment as done
    experiment->done = true;

    // Release the coprocessor
    experiment->coPro->setInUse(false);

    // Remove from the list and delete the experiment
    delete experiment;
    experiments.erase(it);
  }
}

void ExperimentManager::clearExperiments()
{
  // Stop and delete all experiments
  for (Experiment *experiment : experiments)
  {
    experiment->coPro->setInUse(false);
    delete experiment;
  }
  experiments.clear();
}

std::vector<Experiment *> &ExperimentManager::getExperiments()
{
  return experiments;
}

void ExperimentManager::mainLoop()
{
  // Loop over all experiments
  for (auto it = experiments.begin(); it != experiments.end();)
  {
    Experiment *experiment = *it;

    if (!experiment->setupDone)
    {
      logger.println("[INFO] [" + experiment->name + "] Setup");
      experiment->setup();
      experiment->setupDone = true;
    }

    if (experiment->loopSpeed == 0 || millis() - experiment->lastLoop > experiment->loopSpeed)
    {
      experiment->lastLoop = millis();
      experiment->loop();
    }

    if (millis() - experiment->lastSend > experiment->saveDataSpeed && experiment->saveDataFlag && !experiment->done)
    {
      experiment->lastSend = millis();

#ifndef DO_REAL_TEST
      sendData(experiment);
#endif

#ifdef DO_REAL_TEST
      saveData(experiment);
#endif
    }

    if (millis() - experiment->startTime > experiment->duration && !experiment->done)
    {
      experiment->complete();
    }

    if (experiment->done)
    {
      // Release the coprocessor
      experiment->coPro->setInUse(false);

      // Remove from the list and delete the experiment
      delete experiment;
      it = experiments.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

void ExperimentManager::sendData(Experiment *experiment)
{
  // if (wifiSetup.bootTime < 1000)
  // {
  //   wifiSetup.bootTime = time(nullptr);
  //   logger.println("[INFO] [TIME] Boot time set: " + String(wifiSetup.bootTime));
  // }

  // Read data from the coprocessor
  experiment->coPro->readAll();

  DynamicJsonDocument jsonDoc(1024);

  jsonDoc["test"]["type"] = experiment->name;
  jsonDoc["test"]["stage"] = experiment->stage;
  jsonDoc["test"]["chamber"] = experiment->coPro->getId();
  jsonDoc["test"]["uid"] = experiment->uniqueId;

  jsonDoc["temp"]["temp"] = experiment->coPro->temperatureDevice.get();
  jsonDoc["temp"]["setPoint"] = experiment->coPro->heaterTargetTempDevice.get();
  jsonDoc["temp"]["pidOutput"] = experiment->coPro->heaterPWMDevice.get();

#ifdef OLD_LED
  Wled.on();
#else
  experiment->coPro->whiteLED.on();
#endif

  delay(50);
  delay(500);

#if !defined(OLD_COLOUR_SENSOR)
  experiment->coPro->colourSensor.readAll();

  jsonDoc["colour_w"]["red"] = experiment->coPro->colourSensor.getRed();
  jsonDoc["colour_w"]["green"] = experiment->coPro->colourSensor.getGreen();
  jsonDoc["colour_w"]["blue"] = experiment->coPro->colourSensor.getBlue();
  jsonDoc["colour_w"]["white"] = experiment->coPro->colourSensor.getWhite();
  jsonDoc["colour_w"]["ir"] = 0;
#else
  colourSensor.readAll();

  jsonDoc["colour_w"]["red"] = colourSensor.getRed();
  jsonDoc["colour_w"]["green"] = colourSensor.getGreen();
  jsonDoc["colour_w"]["blue"] = colourSensor.getBlue();
  jsonDoc["colour_w"]["white"] = colourSensor.getWhite();
  jsonDoc["colour_w"]["ir"] = 0;
#endif

#ifdef OLD_LED
  Wled.off();
#else
  experiment->coPro->whiteLED.off();
#endif

#ifdef OLD_LED
  UVled.on();
#else
  experiment->coPro->uvaLED.on();
#endif

  delay(50);
  delay(500);

#if !defined(OLD_COLOUR_SENSOR)
  experiment->coPro->colourSensor.readAll();

  jsonDoc["colour_uv"]["red"] = experiment->coPro->colourSensor.getRed();
  jsonDoc["colour_uv"]["green"] = experiment->coPro->colourSensor.getGreen();
  jsonDoc["colour_uv"]["blue"] = experiment->coPro->colourSensor.getBlue();
  jsonDoc["colour_uv"]["white"] = experiment->coPro->colourSensor.getWhite();
  jsonDoc["colour_uv"]["ir"] = 0;
#else
  colourSensor.readAll();

  jsonDoc["colour_uv"]["red"] = colourSensor.getRed();
  jsonDoc["colour_uv"]["green"] = colourSensor.getGreen();
  jsonDoc["colour_uv"]["blue"] = colourSensor.getBlue();
  jsonDoc["colour_uv"]["white"] = colourSensor.getWhite();
  jsonDoc["colour_uv"]["ir"] = 0;
#endif

#ifdef OLD_LED
  Wled.on();
#else
  experiment->coPro->whiteLED.on();
#endif

  delay(50);
  delay(500);

#if !defined(OLD_COLOUR_SENSOR)
  experiment->coPro->colourSensor.readAll();

  jsonDoc["colour_w_uv"]["red"] = experiment->coPro->colourSensor.getRed();
  jsonDoc["colour_w_uv"]["green"] = experiment->coPro->colourSensor.getGreen();
  jsonDoc["colour_w_uv"]["blue"] = experiment->coPro->colourSensor.getBlue();
  jsonDoc["colour_w_uv"]["white"] = experiment->coPro->colourSensor.getWhite();
  jsonDoc["colour_w_uv"]["ir"] = 0;
#else
  colourSensor.readAll();

  jsonDoc["colour_w_uv"]["red"] = colourSensor.getRed();
  jsonDoc["colour_w_uv"]["green"] = colourSensor.getGreen();
  jsonDoc["colour_w_uv"]["blue"] = colourSensor.getBlue();
  jsonDoc["colour_w_uv"]["white"] = colourSensor.getWhite();
  jsonDoc["colour_w_uv"]["ir"] = 0;
#endif

#ifdef OLD_LED
  Wled.off();
  UVled.off();
#else
  experiment->coPro->whiteLED.off();
  experiment->coPro->uvaLED.off();
#endif

  // jsonDoc["battery"]["voltage"] = getBatteryVoltage();
  // jsonDoc["battery"]["percentage"] = getBatteryPercentageI();

  jsonDoc["time"]["millis"] = millis();
  // jsonDoc["time"]["time"] = time(nullptr);
  // jsonDoc["time"]["start_time"] = wifiSetup.bootTime;

  // jsonDoc["device"]["id"] = getDeviceId();
  jsonDoc["device"]["internal_ip"] = WiFi.localIP().toString();
  // jsonDoc["device"]["version"] = VERSION;

#ifdef CUSTOM_NAME
  jsonDoc["device"]["custom_name"] = CUSTOM_NAME;
#endif

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  // Send data to database
  // db.push(jsonStr);
  // db.printResponse();
  logger.println(jsonStr);

  logger.println("[INFO] [EXPERIMENT] Sent data");
}

void ExperimentManager::saveData(Experiment *experiment)
{
  if (experiment != nullptr)
  {
    DynamicJsonDocument jsonDoc(1024);

    experiment->coPro->readAll();

    jsonDoc["stage"] = experiment->stage;
    jsonDoc["chamber"] = experiment->coPro->getId();
    jsonDoc["uid"] = experiment->uniqueId;

    jsonDoc["temp"] = experiment->coPro->temperatureDevice.get();
    jsonDoc["setTemp"] = experiment->coPro->heaterTargetTempDevice.get();
    jsonDoc["pid"] = experiment->coPro->heaterPWMDevice.get();

    experiment->coPro->whiteLED.on();

    delay(50);

    experiment->coPro->colourSensor.readAll();

    jsonDoc["w"]["r"] = experiment->coPro->colourSensor.getRed();
    jsonDoc["w"]["g"] = experiment->coPro->colourSensor.getGreen();
    jsonDoc["w"]["b"] = experiment->coPro->colourSensor.getBlue();
    jsonDoc["w"]["w"] = experiment->coPro->colourSensor.getWhite();

    experiment->coPro->whiteLED.off();
    experiment->coPro->uvaLED.on();

    delay(50);

    experiment->coPro->colourSensor.readAll();

    jsonDoc["uv"]["r"] = experiment->coPro->colourSensor.getRed();
    jsonDoc["uv"]["g"] = experiment->coPro->colourSensor.getGreen();
    jsonDoc["uv"]["b"] = experiment->coPro->colourSensor.getBlue();
    jsonDoc["uv"]["w"] = experiment->coPro->colourSensor.getWhite();

    experiment->coPro->whiteLED.off();
    experiment->coPro->uvaLED.off();

    jsonDoc["time"] = (millis() - experiment->startTime) / 1000;

    String jsonStr;
    serializeJson(jsonDoc, jsonStr);

    logger.println(jsonStr);

    logger.println("[INFO] [EXPERIMENT] Saved data");
    logger.println("[INFO] [EXPERIMENT] JSON Size: " + String(jsonStr.length()));

    // AddDataToExperimentFile(experiment->id, jsonStr);
  }
}

bool ExperimentManager::hasExperiments()
{
  return !experiments.empty();
}

void ExperimentManager::indicateNextAvailableCoproc()
{
  // First, clear any existing indicator
  clearIndicator();

  // Find the next available coprocessor
  for (int i = 0; i < NUM_COPROS; i++)
  {
    if (!coProcessor[i]->getInUse())
    {
      // Turn on the white LED to indicate
      coProcessor[i]->whiteLED.on();

      // Keep track of the indicated coprocessor
      indicatedCoproc = coProcessor[i];

      logger.println("[INFO] Indicating next available coprocessor: " + String(coProcessor[i]->getId()));

      return;
    }
  }

  // No available coprocessor found
  indicatedCoproc = nullptr;
  logger.println("[INFO] No available coprocessor to indicate.");
}

CoProcessor *ExperimentManager::getIndicatedCoproc()
{
  return indicatedCoproc;
}

void ExperimentManager::clearIndicator()
{
  if (indicatedCoproc != nullptr)
  {
    // Turn off the white LED
    indicatedCoproc->whiteLED.off();
    indicatedCoproc = nullptr;

    logger.println("[INFO] Cleared coprocessor indicator.");
  }
}