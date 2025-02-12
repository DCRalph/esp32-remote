#pragma once

#include <Arduino.h>

#include "IO/GPIO.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

// WiFi

#define ESP_NOW_CHANNEL 1
#define CHANNEL 1
#define DEBUG_ESP_NOW
#define ESPNOW_NO_DISABLE_WIFI

// PINS

#define LED_PIN 15

#define NUM_LEDS 100 // Example LED strip length
#define LEDS_PIN 16

#define INPUT_1_PIN 1
#define INPUT_2_PIN 2
#define INPUT_3_PIN 3
#define INPUT_4_PIN 4
#define INPUT_5_PIN 5
#define INPUT_6_PIN 6