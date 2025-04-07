#pragma once

#include <Arduino.h>
// #include "secrets.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include <WiFiManager.h>
#include <Preferences.h>

#define setBit(x, y, z) (x |= (z << y)) // Set a bit to a value
#define clearBit(x, y) (x &= ~(1 << y)) // Clear a bit
#define toggleBit(x, y) (x ^= (1 << y)) // Toggle a bit (0 -> 1, 1 -> 0) and return the new value
#define checkBit(x, y) ((x >> y) & 1)   // Check a bit and return 1 or 0

extern Preferences preferences;
extern WiFiManager wm;

extern uint64_t lastInteract;
extern int autoOffMin;

extern uint32_t fps;
extern uint32_t lastFps;
extern uint32_t frameTime;
extern uint32_t lastFrameTime;

extern uint64_t clearBufferTime;
extern uint64_t screenManagerDrawTime;
extern uint64_t drawTopBarTime;
extern uint64_t sendBufferTime;
extern uint64_t screenUpdateDrawTime;

#define BAUD_RATE 115200

#define DEBUG_GPIO

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
#define CMD_PING 0xd1
#define CMD_TEST 0xd2

#define CMD_ARM 0xf0
#define CMD_DISARM 0xf1

#define CMD_FIRE 0xf2

static uint8_t remote_addr[6] = {0x30, 0x30, 0xf9, 0x2b, 0xba, 0xf0};

void initConfig();
