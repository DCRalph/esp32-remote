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

uint64_t receivedFrames = 0;

QueueHandle_t xBlinkQueue;
void BlinkTask(void *pvParameters);

void handleSerialInput();

// task to print the rpm and headlight state every 1 second
void printTask(void *pvParameters);

RPMDecoder rpmDecoder;               // Create an RPM decoder with ID 0x1dc
HeadLightsDecoder headLightsDecoder; // Create a HeadLights decoder with ID 0x164
GearDecoder gearDecoder;             // Create a Gear decoder with ID 0x188
ShifterPosDecoder shifterPosDecoder; // Create a ShifterPos decoder with ID 0x188

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  canInit();
  canNormal();

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  delay(1000);

  // Initialize FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  // Create a queue for LED blink requests
  xBlinkQueue = xQueueCreate(100, sizeof(CRGB)); // Queue with 10 items

  // Create the blink task
  // xTaskCreate(BlinkTask, "BlinkTask", 2048, NULL, 1, NULL);

  // Create the print task
  // xTaskCreate(printTask, "PrintTask", 2048, NULL, 1, NULL);

  Serial.println("Setup Complete. Ready to communicate with SavvyCAN.");

  // Add the RPM decoder to the decoder manager
  decoderManager.add(&rpmDecoder);
  decoderManager.add(&headLightsDecoder);
  decoderManager.add(&gearDecoder);
  decoderManager.add(&shifterPosDecoder);
}

void loop()
{
  // Check for incoming serial data (from SavvyCAN)
  // if (Serial.available())
  // {
  //   // handleSerialInput();
  //   // Send red color to blink task without blocking
  //   CRGB color = CRGB::Red;
  //   xQueueSend(xBlinkQueue, &color, 0);
  // }

  // Check for incoming CAN data
  if (canInterupt())
  {
    // canSleep();
    receivedFrames++;
    canFrame frame;

    canRead(&frame);

    savvyCanSend(&frame);

    // Process the frame with the decoder manager
    // uint64_t startProcessingTime = micros();
    // bool res = decoderManager.process(&frame);
    // uint64_t endProcessingTime = micros();

    // if (!res)
    // {
    //   // canNormal();
    //   return;
    // }

    // Serial.print("Processing Time: ");
    // Serial.println(endProcessingTime - startProcessingTime);

    // Send blue color to blink task without blocking
    // CRGB color = CRGB::Blue;
    // xQueueSend(xBlinkQueue, &color, 0);

    // Serial.print("RPM: ");
    // Serial.print(rpmDecoder.get());

    // Serial.print("\t");

    // Serial.print("Headlights: ");
    // // Serial.print(HeadLightsStateMap.at(headLightsDecoder.get()).c_str());
    // Serial.print((int)headLightsDecoder.get());

    // Serial.print("\t");

    // Serial.print("Gear: ");
    // // Serial.print(GearStateMap.at(gearDecoder.get()).c_str());
    // Serial.print((int)gearDecoder.get());

    // Serial.print("\t");

    // Serial.print("Shifter Position: ");
    // // Serial.print(ShifterStateMap.at(shifterPosDecoder.get()).c_str());
    // Serial.print((int)shifterPosDecoder.get());

    // Serial.println();

    // if (rpmDecoder.get() < 100)
    // {
    //   frame.print();
    // }

    // canNormal();
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
      digitalWrite(2, HIGH);
      vTaskDelay(pdMS_TO_TICKS(100)); // Non-blocking delay
      leds[0] = CRGB::Black;
      FastLED.show();
      digitalWrite(2, LOW);
    }
  }
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
    // Serial.print(HeadLightsStateMap.at(headLightsDecoder.get()).c_str());
    Serial.print((int)headLightsDecoder.get());

    Serial.print("\t");

    Serial.print("Gear: ");
    // Serial.print(GearStateMap.at(gearDecoder.get()).c_str());
    Serial.print((int)gearDecoder.get());

    Serial.print("\t");

    Serial.print("Shifter Position: ");
    // Serial.print(ShifterStateMap.at(shifterPosDecoder.get()).c_str());
    Serial.print((int)shifterPosDecoder.get());

    Serial.println();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}