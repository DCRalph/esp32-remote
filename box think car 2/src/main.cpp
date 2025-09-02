#include "config.h"

#include "IO/can.h"
#include "IO/savvycan.h"
#include "IO/CANPID.h"

// #include <FastLED.h>

#include "Decoders/rpm.h"
#include "Decoders/headLights.h"
#include "Decoders/gear.h"
#include "Decoders/shifterPos.h"
#include "Decoders/handBrake.h"

// #define SPI_CS_PIN  5 // SPI Chip Select pin

// #define LED_PIN 21   // Define LED pin for FastLED
// #define NUM_LEDS 1   // Number of LEDs
// CRGB leds[NUM_LEDS]; // Create a CRGB array for LED

uint64_t receivedFrames = 0;

// QueueHandle_t xBlinkQueue;
// void BlinkTask(void *pvParameters);

void handleSerialInput();

// task to print the rpm and headlight state every 1 second
void printTask(void *pvParameters);

RPMDecoder rpmDecoder;               // Create an RPM decoder with ID 0x1dc
HeadLightsDecoder headLightsDecoder; // Create a HeadLights decoder with ID 0x164
GearDecoder gearDecoder;             // Create a Gear decoder with ID 0x188
ShifterPosDecoder shifterPosDecoder; // Create a ShifterPos decoder with ID 0x188

// OBD-II PID values
uint16_t obdRPM = 0;
uint8_t obdSpeed = 0;
int16_t obdCoolantTemp = 0;
uint8_t obdThrottle = 0;
float obdFuelLevel = 0.0f;

// Variables for timing PID requests
uint32_t lastPIDRequestTime = 0;
uint8_t currentPIDIndex = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  canInit();
  canNormal();

  // Initialize CANPID system
  canPID.init();

  // pinMode(2, OUTPUT);
  // digitalWrite(2, LOW);
  delay(1000);

  // Initialize FastLED
  // FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  // FastLED.clear();
  // FastLED.show();

  // Create a queue for LED blink requests
  // xBlinkQueue = xQueueCreate(100, sizeof(CRGB)); // Queue with 10 items

  // Create the blink task
  // xTaskCreate(BlinkTask, "BlinkTask", 2048, NULL, 1, NULL);

  // Create the print task
  // xTaskCreate(printTask, "PrintTask", 2048, NULL, 1, NULL);

  Serial.println("Setup Complete. Ready to communicate with SavvyCAN.");
  Serial.println("Starting OBD-II PID requests in main loop");

  // Add the RPM decoder to the decoder manager
  decoderManager.add(&rpmDecoder);
  decoderManager.add(&headLightsDecoder);
  decoderManager.add(&gearDecoder);
  decoderManager.add(&shifterPosDecoder);

  // Initialize the last PID request time
  lastPIDRequestTime = millis();
}

void loop()
{
  // Check for incoming serial data (from SavvyCAN)
  if (Serial.available())
  {
    // handleSerialInput();
    // Send red color to blink task without blocking
    // CRGB color = CRGB::Red;
    // xQueueSend(xBlinkQueue, &color, 0);
  }

  // Check for incoming CAN data
  if (canInterupt())
  {
    canSleep();
    receivedFrames++;
    canFrame frame;

    canRead(&frame);
    canPID.processPIDResponse(&frame);

    // savvyCanSend(&frame);

    // Process the frame with the decoder manager
    uint64_t startProcessingTime = micros();
    bool res = decoderManager.process(&frame);

    // Process the frame as a potential PID response

    uint64_t endProcessingTime = micros();

    // if (!res)
    // {
    //   // canNormal();
    //   return;
    // }

    canNormal();
  }

  // Check for timeouts on PID requests
  canPID.checkTimeout();

  // Handle PID requests with 100ms timing
  if (millis() - lastPIDRequestTime >= 100)
  {
    lastPIDRequestTime = millis();

    // Cycle through different PID requests
    switch (currentPIDIndex)
    {
    case 0:
      // Request engine RPM (Mode 01, PID 0C)
      canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_ENGINE_RPM, [](const uint8_t *data, uint8_t length)
                        {
          obdRPM = CANPID::decodeRPM(data);
          Serial.print("OBD-II RPM: ");
          Serial.println(obdRPM); });
      break;
    case 1:
      // Request vehicle speed (Mode 01, PID 0D)
      canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_VEHICLE_SPEED, [](const uint8_t *data, uint8_t length)
                        {
          obdSpeed = CANPID::decodeVehicleSpeed(data);
          Serial.print("OBD-II Speed: ");
          Serial.print(obdSpeed);
          Serial.println(" km/h"); });
      break;
    case 2:
      // Request coolant temperature (Mode 01, PID 05)
      canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_COOLANT_TEMP, [](const uint8_t *data, uint8_t length)
                        {
          obdCoolantTemp = CANPID::decodeCoolantTemp(data);
          Serial.print("OBD-II Coolant Temp: ");
          Serial.print(obdCoolantTemp);
          Serial.println(" °C"); });
      break;
    case 3:
      // Request throttle position (Mode 01, PID 11)
      canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_THROTTLE_POSITION, [](const uint8_t *data, uint8_t length)
                        {
          obdThrottle = CANPID::decodeThrottlePosition(data);
          Serial.print("OBD-II Throttle: ");
          Serial.print(obdThrottle);
          Serial.println("%"); });
      break;
    case 4:
      // Request fuel level (Mode 01, PID 2F)
      canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_FUEL_LEVEL, [](const uint8_t *data, uint8_t length)
                        {
          obdFuelLevel = CANPID::decodeFuelLevel(data);
          Serial.print("OBD-II Fuel Level: ");
          Serial.print(obdFuelLevel, 1);
          Serial.println("%"); });
      break;
    }

    // Move to the next request
    currentPIDIndex = (currentPIDIndex + 1) % 5;
  }
}

void handleSerialInput()
{
  int res = 0;
  static String inputString = "";
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '\r')
    {
      res = savvyCanHandleSerial(inputString);
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }

  switch (res)
  {
  case ESP_OK:
    res = 0x00;
    break;
  case ESP_ERR_INVALID_ARG:
    res = 0x01;
    break;
  case ESP_ERR_TIMEOUT:
    res = 0x02;
    break;
  case ESP_FAIL:
    res = 0x03;
    break;
  case ESP_ERR_INVALID_STATE:
    res = 0x04;
    break;
  case ESP_ERR_NOT_SUPPORTED:
    res = 0x05;
    break;
  default:
    // res = 0x44;
    break;
  }

  uint8_t data[4];
  memcpy(data, &res, 4);

  canFrame temp;
  temp.id = 0x899;
  temp.extFlag = false;
  temp.len = 4;
  memcpy(temp.data, data, 4);

  savvyCanSend(&temp);
}

// Blink task using FreeRTOS
void BlinkTask(void *pvParameters)
{
  // CRGB color;
  // while (1)
  // {
  //   // Wait for a color to be sent to the queue
  //   if (xQueueReceive(xBlinkQueue, &color, portMAX_DELAY) == pdTRUE)
  //   {
  //     leds[0] = color;
  //     FastLED.show();
  //     digitalWrite(2, HIGH);
  //     vTaskDelay(pdMS_TO_TICKS(100)); // Non-blocking delay
  //     leds[0] = CRGB::Black;
  //     FastLED.show();
  //     digitalWrite(2, LOW);
  //   }
  // }
}

// MARK: getCmdName
// String getCmdName(uint8_t cmd)
// {
//   if (cmdMap.find(cmd) != cmdMap.end())
//   {
//     return cmdMap.at(cmd);
//   }
//   else
//   {
//     return "CMD_UNK:" + String(cmd);
//   }
// }

template <typename K, typename V>
const char *getMapName(const K &value, const std::unordered_map<K, V> &map)
{
  auto it = map.find(value);
  if (it != map.end())
    return it->second.c_str();
  else
    return "Unknown";
}

void printTask(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(1000));

  while (1)
  {
    Serial.print("RPM: ");
    Serial.print(rpmDecoder.get());

    Serial.print("\t");

    Serial.print("Headlights: ");
    Serial.print(getMapName(headLightsDecoder.get(), HeadLightsStateMap));

    Serial.print("\t");

    Serial.print("Gear: ");
    Serial.print(getMapName(gearDecoder.get(), GearStateMap));

    Serial.print("\t");

    Serial.print("Shifter Position: ");
    Serial.print(getMapName(shifterPosDecoder.get(), ShifterStateMap));

    Serial.print("\n");

    // Print OBD-II PID values
    Serial.print("OBD-II - RPM: ");
    Serial.print(obdRPM);

    Serial.print("\tSpeed: ");
    Serial.print(obdSpeed);
    Serial.print(" km/h");

    Serial.print("\tCoolant: ");
    Serial.print(obdCoolantTemp);
    Serial.print(" °C");

    Serial.print("\tThrottle: ");
    Serial.print(obdThrottle);
    Serial.print("%");

    Serial.print("\tFuel: ");
    Serial.print(obdFuelLevel, 1);
    Serial.println("%");

    vTaskDelay(pdMS_TO_TICKS(1000 / 20));
  }
}