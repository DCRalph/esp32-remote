#pragma once

#include <Arduino.h>
// #include "secrets.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include <WiFiManager.h>
#include <Preferences.h>

extern Preferences preferences;
extern WiFiManager wm;

extern uint64_t lastInteract;
extern int autoOffMin;

#define BAUD_RATE 115200

// WiFi
#define HOSTNAME "esp32-box-thing"
#define AP_SSID "ESP32-Box-Thing"

#define ESP_NOW_CHANNEL 1
#define DEBUG_ESP_NOW

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// PINS
#define LED_PIN 21

#define LATCH_PIN 2

#define ENCODER_PIN_BUTTON 1
#define ENCODER_PIN_A 17
#define ENCODER_PIN_B 18

#define BATTERY_SENSE_PIN 3
#define BATTERY_SENSE_R1 10000 // 10k
#define BATTERY_SENSE_R2 997   // 1k
#define BATTERY_SENSE_VCC 3.3
#define BATTERY_SENSE_OFFSET 0.33

#define MCP_POWER_GOOD_PIN 38
#define MCP_STATE_1_PIN 47
#define MCP_STATE_2_PIN 48

// COMMANDS
#define DOOR_LOCK_CMD 0x10

#define RELAY_1_CMD 0x20
#define RELAY_2_CMD 0x21
#define RELAY_3_CMD 0x22
#define RELAY_4_CMD 0x23
#define RELAY_5_CMD 0x24
#define RELAY_6_CMD 0x25

// static uint8_t car_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};
static uint8_t car_addr[6] = {0x30, 0x30, 0xf9, 0x2b, 0xba, 0xf0};

void initConfig();
