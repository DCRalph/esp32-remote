#pragma once

#include <Arduino.h>

#include "IO/GPIO.h"

#include <WiFi.h>

// Soft AP channel; ESP-NOW peers use channel 1 too.
#define CHANNEL 1

// RELAY BUSY

extern bool relay1Busy;
extern bool relay2Busy;
extern bool relay3Busy;
extern bool relay4Busy;
extern bool relay5Busy;
extern bool relay6Busy;

// flash relay 1 settings
extern uint8_t relay1FlashCount;
extern uint16_t relay1FlashDelay; // ms per half cycle



// PINS

#define LED_PIN 21

#define RELAY_1_PIN 5
#define RELAY_2_PIN 6
#define RELAY_3_PIN 7
#define RELAY_4_PIN 10
#define RELAY_5_PIN 11
#define RELAY_6_PIN 12
#define RELAY_7_PIN 13
#define RELAY_8_PIN 14
