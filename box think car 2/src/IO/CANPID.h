#pragma once

#include "config.h"
#include "can.h"
#include <functional>
#include <unordered_map>

// Standard OBD-II CAN IDs
#define OBD_REQUEST_ID 0x7DF
#define OBD_RESPONSE_ID_BASE 0x7E8

// OBD-II Modes
#define OBD_MODE_CURRENT_DATA 0x01
#define OBD_MODE_FREEZE_FRAME 0x02
#define OBD_MODE_STORED_DTC 0x03
#define OBD_MODE_CLEAR_DTC 0x04
#define OBD_MODE_TEST_RESULTS 0x05
#define OBD_MODE_CONTROL 0x08
#define OBD_MODE_VEHICLE_INFO 0x09

// Common OBD-II PIDs for Mode 01
#define PID_ENGINE_LOAD 0x04
#define PID_COOLANT_TEMP 0x05
#define PID_SHORT_TERM_FUEL_TRIM_1 0x06
#define PID_LONG_TERM_FUEL_TRIM_1 0x07
#define PID_ENGINE_RPM 0x0C
#define PID_VEHICLE_SPEED 0x0D
#define PID_INTAKE_AIR_TEMP 0x0F
#define PID_MAF_RATE 0x10
#define PID_THROTTLE_POSITION 0x11
#define PID_O2_SENSOR_VOLTAGE 0x14
#define PID_FUEL_LEVEL 0x2F

// Class to handle OBD-II PID requests and responses
class CANPID
{
private:
  // Callback function type for PID responses
  using PIDCallback = std::function<void(const uint8_t *, uint8_t)>;

  // Map to store callbacks for PIDs
  std::unordered_map<uint8_t, PIDCallback> pidCallbacks;

  // Timeout for PID requests in milliseconds
  uint32_t requestTimeout;

  // Last request time
  uint32_t lastRequestTime;

  // Flag to track if we're waiting for a response
  bool waitingForResponse;

  // Current mode and PID being requested
  uint8_t currentMode;
  uint8_t currentPID;

public:
  CANPID(uint32_t timeout = 1000);
  ~CANPID();

  // Initialize the CANPID system
  bool init();

  // Request a PID with optional callback
  bool requestPID(uint8_t mode, uint8_t pid, PIDCallback callback = nullptr);

  // Process incoming CAN frames for PID responses
  bool processPIDResponse(canFrame *frame);

  // Check for timeouts on PID requests
  void checkTimeout();

  // Helper functions to decode common PIDs
  static int16_t decodeCoolantTemp(const uint8_t *data);
  static uint16_t decodeRPM(const uint8_t *data);
  static uint8_t decodeVehicleSpeed(const uint8_t *data);
  static uint8_t decodeThrottlePosition(const uint8_t *data);
  static float decodeFuelLevel(const uint8_t *data);
};

// Global instance
extern CANPID canPID;
