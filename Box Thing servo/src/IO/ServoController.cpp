#include "ServoController.h"
#include <ESP32Servo.h>
#include "config.h"

static Servo servoMotorX;
static Servo servoMotorY;

static volatile uint8_t targetX = 90;
static volatile uint8_t targetY = 90;
static volatile bool manualMode = false;
static volatile uint8_t rangeX = 180;
static volatile uint8_t rangeY = 180;
static volatile unsigned long lastPacketTime = 0;

// Variables for panning logic in auto mode
static int directionX = 1;
static int directionY = 1;
static uint8_t currentX = 90;
static uint8_t currentY = 90;

void initServos()
{
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servoMotorX.setPeriodHertz(50);
  servoMotorY.setPeriodHertz(50);

  servoMotorX.attach(SERVO_X_PIN, 1000, 2000);
  servoMotorY.attach(SERVO_Y_PIN, 1000, 2000);
  servoMotorX.write(90); // Center position
  servoMotorY.write(90); // Center position
  lastPacketTime = millis();
}

void servoSetX(uint8_t val)
{
  if (manualMode)
  {
    targetX = val;
  }
  else
  {
    rangeX = val;
  }
  lastPacketTime = millis();
}

void servoSetY(uint8_t val)
{
  if (manualMode)
  {
    targetY = val;
  }
  else
  {
    rangeY = val;
  }
  lastPacketTime = millis();
}

void servoSetMan(bool man)
{
  manualMode = man;
  lastPacketTime = millis();
}

void loopServos()
{
  static unsigned long lastExecutionTime = 0;
  unsigned long currentTime = millis();

  // Run every 10 ms

  // Check for timeout: no packet received within 100ms
  if (currentTime - lastPacketTime > 100)
  {
    if (manualMode)
    {
      manualMode = false; // Disable manual mode on timeout
      rangeX = 100;       // Default range values when timeout
      rangeY = 100;
    }
  }

  if (!manualMode)
  {
    if (currentTime - lastExecutionTime >= 30)
    {
      lastExecutionTime = currentTime;
      // Auto mode: Pan servo motors based on ranges around center (90 degrees)

      // Calculate half-ranges for X and Y
      int halfRangeX = rangeX / 2;
      int halfRangeY = rangeY / 2;
      int minX = 90 - halfRangeX;
      int maxX = 90 + halfRangeX;
      int minY = 90 - halfRangeY;
      int maxY = 90 + halfRangeY;

      // Update servo X position
      currentX = currentX + directionX;
      if (currentX <= minX || currentX >= maxX)
      {
        directionX = -directionX;
        currentX = constrain(currentX, minX, maxX);
      }
      servoMotorX.write(currentX);

      // Update servo Y position
      currentY = currentY + directionY;
      if (currentY <= minY || currentY >= maxY)
      {
        directionY = -directionY;
        currentY = constrain(currentY, minY, maxY);
      }
      servoMotorY.write(currentY);
    }
  }
  else
  {
    // Manual mode: Set servos directly to target positions
    servoMotorX.write(targetX);
    servoMotorY.write(targetY);
  }
}
