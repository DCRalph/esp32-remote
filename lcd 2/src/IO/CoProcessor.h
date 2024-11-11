#pragma once

#include "Arduino.h"
#include "config.h"
#include "copro_global.h"
#include "Logger.h"
#include <vector>
#include "Float2Bytes.h"
#include <map>

#define NUM_COPROS 1 


#define WaitTimeout 500
#define GetCooldown 500

// map of coprocessor commands to string
static const std::map<uint8_t, String> cmdMap = {
    {CMD_INIT, "CMD_INIT"},
    {CMD_RESET, "CMD_RESET"},
    {CMD_RETRY, "CMD_RETRY"},
    {CMD_PING, "CMD_PING"},
    {CMD_PONG, "CMD_PONG"},
    {GET_ALL_CMD, "GET_ALL_CMD"},
    {SET_WHITE_CMD, "SET_WHITE_CMD"},
    {GET_WHITE_CMD, "GET_WHITE_CMD"},
    {SET_UVC_CMD, "SET_UVC_CMD"},
    {GET_UVC_CMD, "GET_UVC_CMD"},
    {SET_UVA_CMD, "SET_UVA_CMD"},
    {GET_UVA_CMD, "GET_UVA_CMD"},
    {SET_HEATER_CMD, "SET_HEATER_CMD"},
    {GET_HEATER_CMD, "GET_HEATER_CMD"},
    {SET_HEATER_PID_ENABLED_CMD, "SET_HEATER_PID_ENABLED_CMD"},
    {GET_HEATER_PID_ENABLED_CMD, "GET_HEATER_PID_ENABLED_CMD"},
    {SET_HEATER_TARGET_TEMP_CMD, "SET_HEATER_TARGET_TEMP_CMD"},
    {GET_HEATER_TARGET_TEMP_CMD, "GET_HEATER_TARGET_TEMP_CMD"},
    {SET_HEATER_PID_KP_CMD, "SET_HEATER_PID_KP_CMD"},
    {GET_HEATER_PID_KP_CMD, "GET_HEATER_PID_KP_CMD"},
    {SET_HEATER_PID_KI_CMD, "SET_HEATER_PID_KI_CMD"},
    {GET_HEATER_PID_KI_CMD, "GET_HEATER_PID_KI_CMD"},
    {SET_HEATER_PID_KD_CMD, "SET_HEATER_PID_KD_CMD"},
    {GET_HEATER_PID_KD_CMD, "GET_HEATER_PID_KD_CMD"},
    {SET_HEATER_PID_DT_CMD, "SET_HEATER_PID_DT_CMD"},
    {GET_HEATER_PID_DT_CMD, "GET_HEATER_PID_DT_CMD"},
    {GET_TEMP_CMD, "GET_TEMP_CMD"},
    {GET_TEMP_CALIBRATION_CMD, "GET_TEMP_CALIBRATION_CMD"},
    {SET_TEMP_CALIBRATION_CMD, "SET_TEMP_CALIBRATION_CMD"},
    {GET_COLOUR_CMD, "GET_COLOUR_CMD"}};

String getCmdName(uint8_t cmd);

class CoProcessor;
struct TransferData
{
  uint8_t id;
  uint8_t cmd;
  uint8_t len;
  uint8_t checksum;

  uint8_t data[MAX_DATA_TRANSFER_SIZE];
  void print();
  void clear();
};

struct ColourSensorData
{
  uint16_t red;
  uint16_t green;
  uint16_t blue;
  uint16_t white;
};

struct TempCalibrationData
{
  float a;
  float b;
};

template <typename T>
class CoProcessorDevice
{
private:
  uint8_t getCmd;
  uint8_t setCmd;
  uint8_t len;

protected:
  CoProcessor *parent;
  T state;
  unsigned long lastGet = 0;
  bool cooldownDisabled = false;

public:
  CoProcessorDevice(uint8_t _getCmd, uint8_t _setCmd);
  CoProcessorDevice(uint8_t _getCmd);

  void init(CoProcessor *_parent);

  T get();                      // get state
  int set(T state);             // set state and send to coprocessor
  int read();                   // update state from coprocessor
  void updateState(T newState); // update state from provided value

  int toggle();
};

class CoProcessorLed : public CoProcessorDevice<bool>
{
public:
  CoProcessorLed(uint8_t _getCmd, uint8_t _setCmd);

  void on();
  void off();
  void toggle();
};

class CoProcessorColourSensor : public CoProcessorDevice<ColourSensorData>
{
public:
  CoProcessorColourSensor(uint8_t _getCmd);

  void readAll();

  uint16_t getRed();
  uint16_t getGreen();
  uint16_t getBlue();
  uint16_t getWhite();
};
class CoProcessor
{
private:
  bool enabled;
  bool comunicationFailed;

  uint8_t currentId;
  uint8_t currentIdVersion;
  bool dataAvailable;

  bool inUse = false;

public:
  CoProcessor();
  CoProcessor(uint8_t _id);

  TransferData RXData;
  TransferData LastSentData;

  void init();
  int checkCommunication();
  bool isEnabled();
  bool isCommunicationFailed();

  void loop();
  void setId(uint8_t _id);
  uint8_t getId();
  uint8_t getIdVersion();

  bool getInUse();
  void setInUse(bool _inUse);

  static int sendSerialCommand(CoProcessor *parent, uint8_t cmd, uint8_t len, uint8_t *data);
  static int sendSerialCommand(CoProcessor *parent, uint8_t cmd, uint8_t data);
  static int sendSerialCommand(CoProcessor *parent, uint8_t cmd);

  static int write(CoProcessor *parent, TransferData *packet);

  static int waitForSerialData(CoProcessor *parent, int timeout);
  static int waitForSerialDataRetry(CoProcessor *parent, int timeout, uint16_t expectedLen, int retries);

  uint8_t calculateChecksum(TransferData *packet);
  bool verifyChecksum(TransferData *packet);

  void readAll();

  CoProcessorLed whiteLED = CoProcessorLed(GET_WHITE_CMD, SET_WHITE_CMD);
  CoProcessorLed uvcLED = CoProcessorLed(GET_UVC_CMD, SET_UVC_CMD);
  CoProcessorLed uvaLED = CoProcessorLed(GET_UVA_CMD, SET_UVA_CMD);

  CoProcessorDevice<float> temperatureDevice = CoProcessorDevice<float>(GET_TEMP_CMD);
  CoProcessorDevice<TempCalibrationData> tempCalibrationDevice = CoProcessorDevice<TempCalibrationData>(GET_TEMP_CALIBRATION_CMD, SET_TEMP_CALIBRATION_CMD);

  CoProcessorColourSensor colourSensor = CoProcessorColourSensor(GET_COLOUR_CMD);

  CoProcessorDevice<uint8_t> heaterPWMDevice = CoProcessorDevice<uint8_t>(GET_HEATER_CMD, SET_HEATER_CMD);
  CoProcessorDevice<float> heaterTargetTempDevice = CoProcessorDevice<float>(GET_HEATER_TARGET_TEMP_CMD, SET_HEATER_TARGET_TEMP_CMD);
  CoProcessorDevice<bool> heaterPIDEnabledDevice = CoProcessorDevice<bool>(GET_HEATER_PID_ENABLED_CMD, SET_HEATER_PID_ENABLED_CMD);

  CoProcessorDevice<float> heaterPIDKpDevice = CoProcessorDevice<float>(GET_HEATER_PID_KP_CMD, SET_HEATER_PID_KP_CMD);
  CoProcessorDevice<float> heaterPIDKiDevice = CoProcessorDevice<float>(GET_HEATER_PID_KI_CMD, SET_HEATER_PID_KI_CMD);
  CoProcessorDevice<float> heaterPIDKdDevice = CoProcessorDevice<float>(GET_HEATER_PID_KD_CMD, SET_HEATER_PID_KD_CMD);
  CoProcessorDevice<float> heaterPIDDtDevice = CoProcessorDevice<float>(GET_HEATER_PID_DT_CMD, SET_HEATER_PID_DT_CMD);
};

template class CoProcessorDevice<int>;
template class CoProcessorDevice<uint8_t>;
template class CoProcessorDevice<bool>;
template class CoProcessorDevice<float>;

extern CoProcessor *coProcessor[NUM_COPROS];
void initAllCoPro();
void loopAllCoPro();