#include "CoProcessor.h"

static const char *TAG = "CoProcessor";

String getCmdName(uint8_t cmd)
{
  // check if the command is in the map
  if (cmdMap.find(cmd) != cmdMap.end())
  {
    return cmdMap.at(cmd);
  }
  else
  {
    return "CMD_UNK";
  }
}

void TransferData::print()
{
  ESP_LOGI(TAG, "id: %d, cmd: 0x%04X, len: %d", id, cmd, len);
  ESP_LOGI(TAG, "data: ");

  int line = 0;

  for (int i = 0; i < len; i++)
  {
    if (i % 4 == 0)
    {
      logger.printf("%d: ", line);
    }

    logger.printf("%02X ", data[i]);

    if (i % 4 == 3)
    {
      logger.printf("\n");
      line++;
    }
  }
  logger.printf("\n");
}

void TransferData::clear()
{
  id = 0;
  cmd = 0;
  len = 0;
  checksum = 0;
  memset(data, 0, MAX_DATA_TRANSFER_SIZE);
}

template <typename T>
CoProcessorDevice<T>::CoProcessorDevice(uint8_t _getCmd, uint8_t _setCmd)
{
  getCmd = _getCmd;
  setCmd = _setCmd;
  len = sizeof(T);
  // state = 0;
}

template <typename T>
CoProcessorDevice<T>::CoProcessorDevice(uint8_t _getCmd)
{
  getCmd = _getCmd;
  setCmd = 0x00;
  len = sizeof(T);
  // state = 0;
}

template <typename T>
void CoProcessorDevice<T>::init(CoProcessor *_parent)
{
  parent = _parent;
}

template <typename T>
T CoProcessorDevice<T>::get()
{
  return state;
}

template <typename T>
int CoProcessorDevice<T>::set(T _state)
{
  if (!parent->isEnabled())
    return -1;

  // logger.println("set");
  // ESP_LOGI(TAG, "%d, set", parent->getId());

  if (parent->isCommunicationFailed())
  {
    // logger.println("[ERROR] [CoProcessorDevice] Communication failed");
    ESP_LOGI(TAG, "%d, Communication failed", parent->getId());
    return -1;
  }

  if (setCmd == 0x00)
  {
    // logger.println("[ERROR] [CoProcessorDevice] Set command not defined");
    ESP_LOGI(TAG, "%d, Set command not defined", parent->getId());
    return -1;
  }

  uint8_t _stateBytes[sizeof(T)];
  if (std::is_same<T, bool>::value)
  {
    _stateBytes[0] = _state ? 0x01 : 0x00;
  }
  else
  {
    memcpy(_stateBytes, &_state, sizeof(T));
  }

  // logger.println("before setSerialCommand");
  // ESP_LOGI(TAG, "%d, before setSerialCommand", parent->getId());

  int ret = CoProcessor::sendSerialCommand(parent, setCmd, len, _stateBytes);
  if (CoProcessor::waitForSerialDataRetry(parent, WaitTimeout, sizeof(T), 1) == 0)
  {
    if (parent->RXData.len != sizeof(T))
    {
      // ESP_LOGI(TAG, "%d, Invalid data length received: expected %u, received %u. cmd: 0x%04X", parent->getId(), sizeof(T), parent->RXData.len, setCmd);
      ESP_LOGI(TAG, "%d, Invalid data length received: expected %u, received %u. cmd: %s", parent->getId(), sizeof(T), parent->RXData.len, getCmdName(setCmd).c_str());
    }

    memcpy(&state, parent->RXData.data, sizeof(T));
  }

  return ret;
}

template <typename T>
int CoProcessorDevice<T>::read()
{
  if (!parent->isEnabled())
    return -1;

  if (parent->isCommunicationFailed())
  {
    // logger.println("[ERROR] [CoProcessorDevice] Communication failed");
    ESP_LOGI(TAG, "%d, Communication failed", parent->getId());
    return -1;
  }

  if (getCmd == 0x00)
  {
    // logger.println("[ERROR] [CoProcessorDevice] Get command not defined");
    ESP_LOGI(TAG, "%d, Get command not defined", parent->getId());
    return -1;
  }

  if (millis() - lastGet < 1000 && cooldownDisabled == false)
  {
    // logger.println("[ERROR] [CoProcessorDevice] Get command called too soon");
    ESP_LOGI(TAG, "%d, Get command called too soon", parent->getId());
    return -1;
  }

  lastGet = millis();

  CoProcessor::sendSerialCommand(parent, getCmd);
  if (CoProcessor::waitForSerialDataRetry(parent, WaitTimeout, sizeof(T), 1) == 0)
  {
    if (parent->RXData.len != sizeof(T))
    {
      ESP_LOGI(TAG, "%d, Invalid data length received: expected %u, received %u. cmd: %s", parent->getId(), sizeof(T), parent->RXData.len, getCmdName(getCmd).c_str());

      parent->RXData.print();

      return -1;
    }

    memcpy(&state, parent->RXData.data, sizeof(T));
    return 0;
  }

  return -1;
}

template <typename T>
void CoProcessorDevice<T>::updateState(T newState)
{
  state = newState;
}

template <typename T>
int CoProcessorDevice<T>::toggle()
{
  if (std::is_same<T, bool>::value)
    return set(!state);

  // logger.println("[ERROR] [CoProcessorDevice] Toggle is only supported for bool type");
  ESP_LOGI(TAG, "%d, Toggle is only supported for bool type", parent->getId());
  return -1;
}

CoProcessorLed::CoProcessorLed(uint8_t _getCmd, uint8_t _setCmd) : CoProcessorDevice<bool>(_getCmd, _setCmd)
{
}

void CoProcessorLed::on()
{
  set(true);
}

void CoProcessorLed::off()
{
  set(false);
}

void CoProcessorLed::toggle()
{
  // logger.println("toggle");
  ESP_LOGI(TAG, "%d, toggle", parent->getId());
  set(!state);
}

CoProcessorColourSensor::CoProcessorColourSensor(uint8_t _getCmd) : CoProcessorDevice<ColourSensorData>(_getCmd)
{
  cooldownDisabled = true;
}

void CoProcessorColourSensor::readAll()
{
  read();
}

uint16_t CoProcessorColourSensor::getRed()
{
  return state.red;
}

uint16_t CoProcessorColourSensor::getGreen()
{
  return state.green;
}

uint16_t CoProcessorColourSensor::getBlue()
{
  return state.blue;
}

uint16_t CoProcessorColourSensor::getWhite()
{
  return state.white;
}

CoProcessor::CoProcessor()
{
  dataAvailable = false;
  comunicationFailed = true;
  currentId = 0;

#ifdef ENABLE_COPRO
  enabled = true;
#else
  enabled = false;
#endif
}

CoProcessor::CoProcessor(uint8_t _id)
{
  dataAvailable = false;
  comunicationFailed = true;
  currentId = _id;

#ifdef ENABLE_COPRO
  enabled = true;
#else
  enabled = false;
#endif
}

void CoProcessor::init()
{

  if (!isEnabled())
    return;

  whiteLED.init(this);
  uvcLED.init(this);
  uvaLED.init(this);

  temperatureDevice.init(this);
  tempCalibrationDevice.init(this);

  heaterPWMDevice.init(this);
  heaterTargetTempDevice.init(this);
  heaterPIDEnabledDevice.init(this);

  heaterPIDKpDevice.init(this);
  heaterPIDKiDevice.init(this);
  heaterPIDKdDevice.init(this);
  heaterPIDDtDevice.init(this);

  colourSensor.init(this);

  int ret = checkCommunication();

  if (ret != 0)
  {
    // logger.println("[ERROR] [CoProcessor] Communication failed " + String(ret));
    ESP_LOGI(TAG, "%d, Communication failed %d", currentId, ret);
    comunicationFailed = true;
    return;
  }
  comunicationFailed = false;
  // logger.println("[INFO] [CoProcessor] Communication established");
  ESP_LOGI(TAG, "%d, Communication established", currentId);
  CoProcessor::sendSerialCommand(this, CMD_INIT);

  if (CoProcessor::waitForSerialDataRetry(this, WaitTimeout, 1, 1) == 0)
  {
    if (RXData.len != 1)
    {
      // logger.println("[ERROR] [CoProcessor] Invalid data length received");
      ESP_LOGI(TAG, "%d, Invalid data length received", currentId);
      return;
    }

    currentIdVersion = RXData.data[0];
  }

  if (currentIdVersion < MIN_COPRO_VERSION)
  {
    // logger.println("[ERROR] [CoProcessor] Invalid coprocessor version");
    ESP_LOGI(TAG, "%d, Invalid coprocessor version", currentId);
    comunicationFailed = true;
    return;
  }
}

int CoProcessor::checkCommunication()
{
  if (!isEnabled())
    return -1;
  CoProcessor::sendSerialCommand(this, CMD_PING);

  if (CoProcessor::waitForSerialDataRetry(this, WaitTimeout, 1, 1) == 0)
  {
    // if (RXData.id != 0xFF)
    //   return 1;

    if (RXData.cmd != CMD_PING)
      return 2;

    if (RXData.len != 1)
      return 3;

    if (RXData.data[0] != CMD_PONG)
      return 4;

    return 0;
  }

  return 5;
}

bool CoProcessor::isEnabled()
{
  return enabled;
}

bool CoProcessor::isCommunicationFailed()
{
  return comunicationFailed;
}

void CoProcessor::loop()
{
  if (!isEnabled())
    return;

  dataAvailable = false;

  if (isCommunicationFailed())
    return;

  if (CoProcessor::waitForSerialData(this, 0) == 0)
    dataAvailable = true;
}

void CoProcessor::setId(uint8_t _id)
{
  if (!isEnabled())
    return;

  currentId = _id;
  comunicationFailed = true;

  int ret = checkCommunication();
  if (ret != 0)
  {
    comunicationFailed = true;
    // logger.println("[ERROR] [CoProcessor] Communication failed");
    ESP_LOGI(TAG, "%d, Communication failed", currentId);
    return;
  }
  else
  {
    comunicationFailed = false;
    // logger.println("[INFO] [CoProcessor] Communication established");
    ESP_LOGI(TAG, "%d, Communication established", currentId);
  }

  init();
}

uint8_t CoProcessor::getId()
{
  return currentId;
}

uint8_t CoProcessor::getIdVersion()
{
  return currentIdVersion;
}

bool CoProcessor::getInUse()
{
  return inUse;
}

void CoProcessor::setInUse(bool _inUse)
{
  inUse = _inUse;
}

int CoProcessor::sendSerialCommand(CoProcessor *parent, uint8_t cmd, uint8_t len, uint8_t *data)
{
  if (!parent->isEnabled())
    return 1;

  // if (parent->isCommunicationFailed())
  // {
  //   ESP_LOGI(TAG, "%d, Communication failed", parent->currentId);
  //   return -1;
  // }

  TransferData packet;
  packet.id = parent->getId();
  packet.cmd = cmd;
  packet.len = len;

  if (len > 0)
  {
    memcpy(packet.data, data, len);
  }

  write(parent, &packet);

  // uint8_t buffer[len + COPRO_HEADER_BYTES];

  // buffer[IDX_ID] = parent->getId();
  // buffer[IDX_CMD] = cmd;
  // buffer[IDX_LEN] = len;

  // for (int i = 0; i < len; i++)
  // {
  //   buffer[i + COPRO_HEADER_BYTES] = data[i];
  // }

  // Copro.write(buffer, len + COPRO_HEADER_BYTES);

  return 0;
}

int CoProcessor::write(CoProcessor *parent, TransferData *packet)
{
  if (!parent->isEnabled())
    return -1;

  // if (parent->isCommunicationFailed())
  // {
  //   ESP_LOGI(TAG, "%d, Communication failed", parent->currentId);
  //   return -1;
  // }

  uint8_t buffer[packet->len + COPRO_HEADER_BYTES];

  packet->checksum = parent->calculateChecksum(packet);

  buffer[IDX_ID] = packet->id;
  buffer[IDX_CMD] = packet->cmd;
  buffer[IDX_LEN] = packet->len;
  buffer[IDX_CHECKSUM] = packet->checksum;

  for (int i = 0; i < packet->len; i++)
  {
    buffer[i + COPRO_HEADER_BYTES] = packet->data[i];
  }

  memcpy(&parent->LastSentData, packet, sizeof(TransferData));

  Copro.write(buffer, packet->len + COPRO_HEADER_BYTES);

#if (DEBUG == true && DEBUG_COPRO == true)
  logger.println("##########  Send  ##############");
  logger.print("ID: ");
  logger.hexln(parent->getId());
  logger.print("Command: ");
  logger.println(getCmdName(packet->cmd));
  logger.print("Length: ");
  logger.hexln(packet->len);
  logger.print("Checksum: ");
  logger.hexln(packet->checksum);
  logger.print("Data: ");
  logger.hex(packet->data, packet->len);
  logger.println();
  logger.println("###############################");
#endif

  return 0;
}

int CoProcessor::sendSerialCommand(CoProcessor *parent, uint8_t cmd, uint8_t data)
{
  uint8_t buffer[1] = {data};

  return sendSerialCommand(parent, cmd, 1, buffer);
}

int CoProcessor::sendSerialCommand(CoProcessor *parent, uint8_t cmd)
{
  return sendSerialCommand(parent, cmd, 0, NULL);
}

int CoProcessor::waitForSerialData(CoProcessor *parent, int timeout)
{
  if (!parent->isEnabled())
    return -1;

  unsigned long startMillis = millis();

  do
  {
    if (Copro.available() > 0)
    {
      uint8_t buffer[MAX_DATA_TRANSFER_SIZE + COPRO_HEADER_BYTES];

      Copro.readBytes(buffer, COPRO_HEADER_BYTES);
      parent->RXData.id = buffer[IDX_ID];
      parent->RXData.cmd = buffer[IDX_CMD];
      parent->RXData.len = buffer[IDX_LEN];
      parent->RXData.checksum = buffer[IDX_CHECKSUM];
      uint8_t realLen = 0;

      if (parent->RXData.len > 0)
      {
        realLen = Copro.readBytes(parent->RXData.data, parent->RXData.len);
        Copro.flush();
      }

      parent->RXData.len = realLen;

      bool valid = parent->verifyChecksum(&parent->RXData);
      uint8_t doubleCheck = parent->calculateChecksum(&parent->RXData);

#if (DEBUG == true && DEBUG_COPRO == true)
      logger.println("##########  Receive  ##############");
      logger.print("ID: ");
      logger.hexln(parent->RXData.id);
      logger.print("Command: ");
      logger.println(getCmdName(parent->RXData.cmd));
      logger.print("Length: ");
      logger.hexln(parent->RXData.len);
      logger.print("Checksum: ");
      logger.hexln(parent->RXData.checksum);
      logger.print("Double Check Checksum: ");
      logger.hexln(doubleCheck);
      logger.print("Data: ");
      logger.hex(parent->RXData.data, parent->RXData.len);
      logger.println();
      logger.println("###############################");
#endif

      if (!valid)
      {
        // logger.println("[ERROR] [CoProcessor] Invalid checksum");
        ESP_LOGI(TAG, "%d, Invalid checksum", parent->currentId);

        return 10;
      }

      return 0;
    }
  } while (millis() - startMillis < timeout);

  return 1;
}

int CoProcessor::waitForSerialDataRetry(CoProcessor *parent, int timeout, uint16_t expectedLen, int retries)
{
  int ret = -1;
  uint64_t startMillis = millis();

  ESP_LOGI(TAG, "[%d] Waiting for serial data, timeout: %d, expectedLen: %u, retries: %d", parent->currentId, timeout, expectedLen, retries);
  ESP_LOGI(TAG, "[%d] ret: %d, retries: %d, parent->RXData.len: %u, expectedLen: %u", parent->currentId, ret, retries, parent->RXData.len, expectedLen);

  while (ret != 0 && retries > -1)
  {

    ret = waitForSerialData(parent, timeout);

    if (ret == 0 && parent->RXData.len != expectedLen)
    {
      ESP_LOGI(TAG, "[%d] Retry failed, Invalid data length received: expected %u, received %u. Trying %d more times", parent->currentId, expectedLen, parent->RXData.len, retries - 1);

      parent->sendSerialCommand(parent, CMD_RETRY);
      ret = -1;
    }
    else if (ret == 10)
    {
      ESP_LOGI(TAG, "[%d] Retry failed, Invalid checksum, trying %d more times", parent->currentId, retries - 1);

      parent->sendSerialCommand(parent, CMD_RETRY);
    }
    else if (ret != 0)
    {
      ESP_LOGI(TAG, "[%d] Retry failed, trying %d more times", parent->currentId, retries - 1);

      parent->sendSerialCommand(parent, CMD_RETRY);
    }

    retries--;
  }

  uint64_t elapsed = millis() - startMillis;
  ESP_LOGI(TAG, "[%d] Waiting end. Elapsed: %llu ms", parent->currentId, elapsed);

  return ret;
}

uint8_t CoProcessor::calculateChecksum(TransferData *packet)
{
  uint8_t checksum = 0;

  checksum += packet->cmd;
  checksum += packet->len;

  for (uint8_t i = 0; i < packet->len; i++)
  {
    checksum += packet->data[i];
  }

  return checksum;

  // uint8_t checksum = 0;
  // for (uint8_t i = 0; i < packet->len; i++)
  // {
  //   checksum += packet->data[i];
  // }
  // return (~checksum) + 1; // Two's complement
}

bool CoProcessor::verifyChecksum(TransferData *packet)
{
  uint8_t checksum = calculateChecksum(packet);

  return checksum == packet->checksum;
}

void CoProcessor::readAll()
{
  if (!isEnabled())
    return;

  if (isCommunicationFailed())
  {
    // logger.println("[ERROR] [CoProcessor] Communication failed");
    ESP_LOGI(TAG, "%d, Communication failed", currentId);
    return;
  }

  CoProcessor::sendSerialCommand(this, GET_ALL_CMD);

  if (CoProcessor::waitForSerialDataRetry(this, WaitTimeout, 37, 3) == 0)
  {
    if (RXData.len != 37)
    {
      ESP_LOGI(TAG, "%d, WTF, Invalid data length received: expected 37, received %u", currentId, RXData.len);

      RXData.print();

      return;
    }

    whiteLED.updateState((bool)RXData.data[0]);
    uvcLED.updateState((bool)RXData.data[1]);
    uvaLED.updateState((bool)RXData.data[2]);
    heaterPWMDevice.updateState((uint8_t)RXData.data[3]);
    heaterPIDEnabledDevice.updateState((bool)RXData.data[4]);

    uint8_t pidTargetTemp[4];
    uint8_t temp[4];

    uint8_t tempCalA[4];
    uint8_t tempCalB[4];
    TempCalibrationData tempCalData;

    uint8_t kp[4];
    uint8_t ki[4];
    uint8_t kd[4];
    uint8_t dt[4];

    for (int i = 0; i < 4; i++)
    {
      temp[i] = RXData.data[5 + i];
      pidTargetTemp[i] = RXData.data[9 + i];

      tempCalA[i] = RXData.data[13 + i];
      tempCalB[i] = RXData.data[17 + i];

      kp[i] = RXData.data[21 + i];
      ki[i] = RXData.data[25 + i];
      kd[i] = RXData.data[29 + i];
      dt[i] = RXData.data[33 + i];
    }

    temperatureDevice.updateState(bytesToFloat(temp));
    heaterTargetTempDevice.updateState(bytesToFloat(pidTargetTemp));

    tempCalData.a = bytesToFloat(tempCalA);
    tempCalData.b = bytesToFloat(tempCalB);
    tempCalibrationDevice.updateState(tempCalData);

    heaterPIDKpDevice.updateState(bytesToFloat(kp));
    heaterPIDKiDevice.updateState(bytesToFloat(ki));
    heaterPIDKdDevice.updateState(bytesToFloat(kd));
    heaterPIDDtDevice.updateState(bytesToFloat(dt));
  }
}

CoProcessor *coProcessor[NUM_COPROS];

void initAllCoPro()
{
  for (int i = 0; i < NUM_COPROS; i++)
  {
    coProcessor[i] = new CoProcessor(i);
    coProcessor[i]->init();
    coProcessor[i]->readAll();
  }
}

void loopAllCoPro()
{
  for (int i = 0; i < NUM_COPROS; i++)
  {
    coProcessor[i]->loop();
  }
}