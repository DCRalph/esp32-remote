#pragma once

#include <Arduino.h>

#include "IO/GPIO.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

// WiFi

#define ESP_NOW_CHANNEL 1
#define DEBUG_ESP_NOW

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

// COMMANDS
#define DOOR_LOCK_CMD 0x10

#define RELAY_1_CMD 0x20
#define RELAY_2_CMD 0x21
#define RELAY_3_CMD 0x22
#define RELAY_4_CMD 0x23
#define RELAY_5_CMD 0x24
#define RELAY_6_CMD 0x25
