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
// #define DEBUG_ESP_NOW

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// global relay booleans
extern bool globalRelay1;
extern bool globalRelay2;
extern bool globalRelay3;
extern bool globalRelay4;
extern bool globalRelay5;
extern bool globalRelay6;

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
#define CMD_DOOR_LOCK 0x10

#define CMD_RELAY_ALL 0x20

#define CMD_RELAY_1_SET 0x21
#define CMD_RELAY_1_GET 0x22

#define CMD_RELAY_2_SET 0x23
#define CMD_RELAY_2_GET 0x24

#define CMD_RELAY_3_SET 0x25
#define CMD_RELAY_3_GET 0x26

#define CMD_RELAY_4_SET 0x27
#define CMD_RELAY_4_GET 0x28

#define CMD_RELAY_5_SET 0x29
#define CMD_RELAY_5_GET 0x2a

#define CMD_RELAY_6_SET 0x2b
#define CMD_RELAY_6_GET 0x2c

#define CMD_RELAY_1_FLASH 0x30

// static uint8_t car_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};
static uint8_t car_addr[6] = {0x30, 0x30, 0xf9, 0x2b, 0xba, 0xf0};

void initConfig();
