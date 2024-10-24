#include <SPI.h>
#include "mcp_can.h"
#include <FastLED.h>

#define SPI_CS_PIN 5    // SPI Chip Select pin
#define CAN_INT_PIN 27  // CAN Interrupt pin

#define LED_PIN 21      // Define LED pin for FastLED
#define NUM_LEDS 1      // Number of LEDs
CRGB leds[NUM_LEDS];    // Create a CRGB array for LED

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

MCP_CAN CAN0(SPI_CS_PIN); // Set CS pin

QueueHandle_t xBlinkQueue;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize MCP2515 at 500kbps
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
  } else {
    Serial.println("Error Initializing MCP2515...");
    while (1);
  }

  CAN0.setMode(MCP_NORMAL); // Set operation mode to normal
  pinMode(CAN_INT_PIN, INPUT);

  // Initialize FastLED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  // Create a queue for LED blink requests
  xBlinkQueue = xQueueCreate(100, sizeof(CRGB)); // Queue with 10 items

  // Create the blink task
  xTaskCreate(BlinkTask, "BlinkTask", 2048, NULL, 1, NULL);

  Serial.println("Setup Complete. Ready to communicate with SavvyCAN.");
}

void loop() {
  // Check for incoming serial data (from SavvyCAN)
  if (Serial.available()) {
    handleSerialInput();
    // Send red color to blink task without blocking
    CRGB color = CRGB::Red;
    xQueueSend(xBlinkQueue, &color, 0);
  }

  // Check for incoming CAN data
  if (!digitalRead(CAN_INT_PIN)) {
    readCanMessage();
    // Send blue color to blink task without blocking
    CRGB color = CRGB::Blue;
    xQueueSend(xBlinkQueue, &color, 0);
  }
}

void handleSerialInput() {
  static String inputString = "";
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\r') {
      processSlcanCommand(inputString);
      inputString = "";
    } else {
      inputString += inChar;
    }
  }
}

void processSlcanCommand(String command) {
  char cmd = command.charAt(0);

  switch (cmd) {
    case 'O': // Open CAN channel
      CAN0.setMode(MCP_NORMAL);
      Serial.print("\r");
      break;

    case 'C': // Close CAN channel
      CAN0.setMode(MCP_SLEEP);
      Serial.print("\r");
      break;

    case 't': // Transmit standard CAN frame
      sendStandardFrame(command);
      break;

    case 'T': // Transmit extended CAN frame
      sendExtendedFrame(command);
      break;

    // Implement other SLCAN commands as needed

    default:
      Serial.print("\a"); // Bell character for error
      break;
  }
}

void sendStandardFrame(String command) {
  // Command format: tIIIDLDDDDDDDD
  // IIID: 3-digit standard CAN ID in hex
  // L: Data length (1 digit)
  // DDD...: Data bytes in hex
  unsigned long canId = strtoul(command.substring(1, 4).c_str(), NULL, 16);
  uint8_t len = strtoul(command.substring(4, 5).c_str(), NULL, 16);
  uint8_t data[8];

  for (uint8_t i = 0; i < len; i++) {
    data[i] = strtoul(command.substring(5 + i * 2, 7 + i * 2).c_str(), NULL, 16);
  }

  CAN0.sendMsgBuf(canId, 0, len, data);
  Serial.print("\r");
}

void sendExtendedFrame(String command) {
  // Command format: TIIIIIIIIDLDDDDDDDD
  // IIIIIIII: 8-digit extended CAN ID in hex
  unsigned long canId = strtoul(command.substring(1, 9).c_str(), NULL, 16);
  uint8_t len = strtoul(command.substring(9, 10).c_str(), NULL, 16);
  uint8_t data[8];

  for (uint8_t i = 0; i < len; i++) {
    data[i] = strtoul(command.substring(10 + i * 2, 12 + i * 2).c_str(), NULL, 16);
  }

  CAN0.sendMsgBuf(canId, 1, len, data);
  Serial.print("\r");
}

void readCanMessage() {
  unsigned long canId;
  uint8_t len = 0;
  uint8_t data[8];
  uint8_t extFlag;

  CAN0.readMsgBuf(&canId, &extFlag, &len, data);

  if (extFlag == 0) {
    // Standard frame
    Serial.print('t');
    Serial.print(padHex(canId, 3));
  } else {
    // Extended frame
    Serial.print('T');
    Serial.print(padHex(canId, 8));
  }

  Serial.print(len, HEX);

  for (int i = 0; i < len; i++) {
    Serial.print(padHex(data[i], 2));
  }

  Serial.print('\r');
}

String padHex(unsigned long num, uint8_t width) {
  String hexStr = String(num, HEX);
  while (hexStr.length() < width) {
    hexStr = "0" + hexStr;
  }
  return hexStr;
}

// Blink task using FreeRTOS
void BlinkTask(void *pvParameters) {
  CRGB color;
  while (1) {
    // Wait for a color to be sent to the queue
    if (xQueueReceive(xBlinkQueue, &color, portMAX_DELAY) == pdTRUE) {
      leds[0] = color;
      FastLED.show();
      vTaskDelay(pdMS_TO_TICKS(100)); // Non-blocking delay
      leds[0] = CRGB::Black;
      FastLED.show();
    }
  }
}
