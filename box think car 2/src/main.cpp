#include "config.h"

#include "IO/can.h"
#include "IO/savvycan.h"

#include <FastLED.h>

#include "Decoders/rpm.h"
#include "Decoders/headLights.h"
#include "Decoders/gear.h"
#include "Decoders/shifterPos.h"

#define SPI_CS_PIN 5 // SPI Chip Select pin

#define LED_PIN 21   // Define LED pin for FastLED
#define NUM_LEDS 1   // Number of LEDs
CRGB leds[NUM_LEDS]; // Create a CRGB array for LED

QueueHandle_t xBlinkQueue;
void BlinkTask(void *pvParameters);

void handleSerialInput();

// task to print the rpm and headlight state every 1 second
void printTask(void *pvParameters);


RPMDecoder rpmDecoder; // Create an RPM decoder with ID 0x1dc
HeadLightsDecoder headLightsDecoder; // Create a HeadLights decoder with ID 0x164
GearDecoder gearDecoder; // Create a Gear decoder with ID 0x188
ShifterPosDecoder shifterPosDecoder; // Create a ShifterPos decoder with ID 0x188

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  canInit();
  canNormal();

  // Initialize FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  // Create a queue for LED blink requests
  xBlinkQueue = xQueueCreate(100, sizeof(CRGB)); // Queue with 10 items

  // Create the blink task
  xTaskCreate(BlinkTask, "BlinkTask", 2048, NULL, 1, NULL);

  // Create the print task
  // xTaskCreate(printTask, "PrintTask", 2048, NULL, 1, NULL);

  Serial.println("Setup Complete. Ready to communicate with SavvyCAN.");

  // Add the RPM decoder to the decoder manager
  decoderManager.add(&rpmDecoder);
  decoderManager.add(&headLightsDecoder);
  decoderManager.add(&gearDecoder);
  decoderManager.add(&shifterPosDecoder);

  // canFrame frame;
  // frame.id = 0x1dc;
  // frame.extFlag = 0;
  // frame.len = 4;
  // frame.data[0] = 0x02;
  // frame.data[1] = 0x02;
  // frame.data[2] = 0xb3;
  // frame.data[3] = 0x2a;

  // decoderManager.process(frame);

  // Serial.print("RPM: ");
  // Serial.println(rpmDecoder.get());

  // frame.id = 0x164;
  // frame.extFlag = 0;
  // frame.len = 8;
  // frame.data[0] = 0x27;

  // decoderManager.process(frame);

  // Serial.print("Headlights: ");
  // Serial.println(HeadLightsStateMap.at(headLightsDecoder.get()).c_str());
  
}

void loop()
{
  // Check for incoming serial data (from SavvyCAN)
  if (Serial.available())
  {
    handleSerialInput();
    // Send red color to blink task without blocking
    CRGB color = CRGB::Red;
    xQueueSend(xBlinkQueue, &color, 0);
  }

  // Check for incoming CAN data
  if (canInterupt())
  {

    canFrame frame;

    canRead(&frame);

    savvyCanSend(&frame);

    // Process the frame with the decoder manager
    decoderManager.process(frame);

    // Send blue color to blink task without blocking
    CRGB color = CRGB::Blue;
    xQueueSend(xBlinkQueue, &color, 0);
  }
}

void handleSerialInput()
{
  static String inputString = "";
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '\r')
    {
      savvyCanHandleSerial(inputString);
      inputString = "";
    }
    else
    {
      inputString += inChar;
    }
  }
}

// Blink task using FreeRTOS
void BlinkTask(void *pvParameters)
{
  CRGB color;
  while (1)
  {
    // Wait for a color to be sent to the queue
    if (xQueueReceive(xBlinkQueue, &color, portMAX_DELAY) == pdTRUE)
    {
      leds[0] = color;
      FastLED.show();
      vTaskDelay(pdMS_TO_TICKS(100)); // Non-blocking delay
      leds[0] = CRGB::Black;
      FastLED.show();
    }
  }
}

void printTask(void *pvParameters)
{
  while (1)
  {
    Serial.print("RPM: ");
    Serial.println(rpmDecoder.get());
    Serial.print("Headlights: ");
    Serial.println(HeadLightsStateMap.at(headLightsDecoder.get()).c_str());
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}