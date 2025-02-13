#pragma once

#include <Arduino.h>

#include "IO/GPIO.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

// WiFi

#define ESP_NOW_CHANNEL 1
#define CHANNEL 1
// #define DEBUG_ESP_NOW
#define ESPNOW_NO_DISABLE_WIFI




// PINS

#define LED_PIN 21

#define SERVO_X_PIN 5
#define SERVO_Y_PIN 6
#define LASER_PIN 7
