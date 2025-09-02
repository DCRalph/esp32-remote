#include "CANPID.h"
#include "esp_log.h"

static const char *TAG = "CANPIDExample";

// Example function to request engine RPM
void requestEngineRPM()
{
  // Request RPM (Mode 01, PID 0C) with a callback function
  canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_ENGINE_RPM, [](const uint8_t *data, uint8_t length)
                    {
        // Decode the RPM value
        uint16_t rpm = CANPID::decodeRPM(data);
        ESP_LOGI(TAG, "Engine RPM: %u", rpm); });
}

// Example function to request vehicle speed
void requestVehicleSpeed()
{
  // Request vehicle speed (Mode 01, PID 0D) with a callback function
  canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_VEHICLE_SPEED, [](const uint8_t *data, uint8_t length)
                    {
        // Decode the speed value
        uint8_t speed = CANPID::decodeVehicleSpeed(data);
        ESP_LOGI(TAG, "Vehicle Speed: %u km/h", speed); });
}

// Example function to request coolant temperature
void requestCoolantTemp()
{
  // Request coolant temperature (Mode 01, PID 05) with a callback function
  canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_COOLANT_TEMP, [](const uint8_t *data, uint8_t length)
                    {
        // Decode the temperature value
        int16_t temp = CANPID::decodeCoolantTemp(data);
        ESP_LOGI(TAG, "Coolant Temperature: %d °C", temp); });
}

// Example function to request throttle position
void requestThrottlePosition()
{
  // Request throttle position (Mode 01, PID 11) with a callback function
  canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_THROTTLE_POSITION, [](const uint8_t *data, uint8_t length)
                    {
        // Decode the throttle position value
        uint8_t throttle = CANPID::decodeThrottlePosition(data);
        ESP_LOGI(TAG, "Throttle Position: %u%%", throttle); });
}

// Example function to request fuel level
void requestFuelLevel()
{
  // Request fuel level (Mode 01, PID 2F) with a callback function
  canPID.requestPID(OBD_MODE_CURRENT_DATA, PID_FUEL_LEVEL, [](const uint8_t *data, uint8_t length)
                    {
        // Decode the fuel level value
        float level = CANPID::decodeFuelLevel(data);
        ESP_LOGI(TAG, "Fuel Level: %.1f%%", level); });
}

// Example of how to process CAN frames for PID responses
void processPIDResponses()
{
  canFrame frame;

  // Check if there's a CAN message available
  if (canRead(&frame) == 1)
  {
    // Process the frame as a potential PID response
    canPID.processPIDResponse(&frame);
  }

  // Check for timeouts on PID requests
  canPID.checkTimeout();
}

// Example of a periodic task to request PIDs
void periodicPIDRequests()
{
  static uint32_t lastRequestTime = 0;
  static uint8_t requestIndex = 0;

  // Request a different PID every 100ms
  if (millis() - lastRequestTime >= 100)
  {
    lastRequestTime = millis();

    // Cycle through different PID requests
    switch (requestIndex)
    {
    case 0:
      requestEngineRPM();
      break;
    case 1:
      requestVehicleSpeed();
      break;
    case 2:
      requestCoolantTemp();
      break;
    case 3:
      requestThrottlePosition();
      break;
    case 4:
      requestFuelLevel();
      break;
    }

    // Move to the next request
    requestIndex = (requestIndex + 1) % 5;
  }
}