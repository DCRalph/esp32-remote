#include "CANPID.h"
#include "esp_log.h"

static const char *TAG = "CANPID";

// Global instance
CANPID canPID;

CANPID::CANPID(uint32_t timeout) : requestTimeout(timeout),
                                   lastRequestTime(0),
                                   waitingForResponse(false),
                                   currentMode(0),
                                   currentPID(0)
{
}

CANPID::~CANPID()
{
  // Clear any callbacks
  pidCallbacks.clear();
}

bool CANPID::init()
{
  ESP_LOGI(TAG, "Initializing CANPID system");
  return true;
}

bool CANPID::requestPID(uint8_t mode, uint8_t pid, PIDCallback callback)
{
  // If we're already waiting for a response, don't send another request
  if (waitingForResponse)
  {
    ESP_LOGW(TAG, "Already waiting for PID response, ignoring request");
    return false;
  }

  // Create a CAN frame for the OBD-II request
  canFrame frame;
  frame.id = OBD_REQUEST_ID;
  frame.extFlag = 0; // Standard ID
  frame.len = 8;     // OBD-II messages are always 8 bytes

  // Format according to OBD-II standard:
  // [Number of additional bytes] [Mode] [PID] [0] [0] [0] [0] [0]
  frame.data[0] = 0x02; // Mode + PID = 2 bytes
  frame.data[1] = mode; // Mode (e.g., 0x01 for current data)
  frame.data[2] = pid;  // PID (e.g., 0x0C for RPM)
  frame.data[3] = 0xCC; // Padding
  frame.data[4] = 0xCC; // Padding
  frame.data[5] = 0xCC; // Padding
  frame.data[6] = 0xCC; // Padding
  frame.data[7] = 0xCC; // Padding

  // Store the callback if provided
  if (callback)
  {
    pidCallbacks[pid] = callback;
  }

  // Send the request
  if (canSend(&frame) != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to send PID request");
    return false;
  }

  // Update state
  waitingForResponse = true;
  currentMode = mode;
  currentPID = pid;
  lastRequestTime = millis();

  ESP_LOGI(TAG, "Sent PID request: Mode 0x%02X, PID 0x%02X", mode, pid);
  return true;
}

bool CANPID::processPIDResponse(canFrame *frame)
{
  // Check if this is an OBD-II response (0x7E8 to 0x7EF)
  if (frame->id < OBD_RESPONSE_ID_BASE || frame->id > (OBD_RESPONSE_ID_BASE + 7))
  {
    return false;
  }

  ESP_LOGI(TAG, "Received PID frame");

  // Check if we're waiting for a response
  if (!waitingForResponse)
  {
    ESP_LOGD(TAG, "Received OBD-II response but not waiting for one");
    return false;
  }

  // Verify the response format
  if (frame->len < 3)
  {
    ESP_LOGW(TAG, "Invalid OBD-II response length");
    return false;
  }

  // Check if this is a response to our current request
  // Response mode is request mode + 0x40
  uint8_t responseMode = frame->data[1];
  uint8_t responsePID = frame->data[2];

  if (responseMode != (currentMode + 0x40) || responsePID != currentPID)
  {
    ESP_LOGW(TAG, "Received response for different Mode/PID: 0x%02X/0x%02X, expected: 0x%02X/0x%02X",
             responseMode, responsePID, currentMode + 0x40, currentPID);
    return false;
  }

  // Extract the data bytes
  uint8_t dataBytes = frame->data[0] - 2; // Subtract mode and PID bytes
  if (dataBytes > 5)
  {
    dataBytes = 5; // Maximum of 5 data bytes (8 - header bytes)
  }

  ESP_LOGI(TAG, "Received PID response: Mode 0x%02X, PID 0x%02X, %d data bytes",
           responseMode, responsePID, dataBytes);

  // Call the callback if registered
  auto callbackIt = pidCallbacks.find(currentPID);
  if (callbackIt != pidCallbacks.end())
  {
    // Pass the data bytes (starting from byte 3) to the callback
    callbackIt->second(&frame->data[3], dataBytes);
  }

  // Reset state
  waitingForResponse = false;

  return true;
}

void CANPID::checkTimeout()
{
  if (waitingForResponse && (millis() - lastRequestTime > requestTimeout))
  {
    ESP_LOGW(TAG, "PID request timed out: Mode 0x%02X, PID 0x%02X",
             currentMode, currentPID);
    waitingForResponse = false;
  }
}

// Helper functions to decode common PIDs

int16_t CANPID::decodeCoolantTemp(const uint8_t *data)
{
  // Formula: A - 40 (°C)
  return static_cast<int16_t>(data[0]) - 40;
}

uint16_t CANPID::decodeRPM(const uint8_t *data)
{
  // Formula: ((A * 256) + B) / 4 (RPM)
  return static_cast<uint16_t>((data[0] * 256 + data[1]) / 4);
}

uint8_t CANPID::decodeVehicleSpeed(const uint8_t *data)
{
  // Formula: A (km/h)
  return data[0];
}

uint8_t CANPID::decodeThrottlePosition(const uint8_t *data)
{
  // Formula: A * 100 / 255 (%)
  return static_cast<uint8_t>((data[0] * 100) / 255);
}

float CANPID::decodeFuelLevel(const uint8_t *data)
{
  // Formula: A * 100 / 255 (%)
  return (data[0] * 100.0f) / 255.0f;
}