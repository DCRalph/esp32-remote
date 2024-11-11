#pragma once

#include <Arduino.h>
#include "secrets.h"

#include <Preferences.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

extern Preferences preferences;
extern HardwareSerial Copro;

#define ESP_NOW_CHANNEL 1
#define DEBUG_ESP_NOW

#define BAUD_RATE 115200

#define DEBUG true
#define DEBUG_ESP_NOW false
#define DEBUG_COPRO true
#define ENABLE_COPRO true

#define BTN_UP_PIN 14
#define BTN_DOWN_PIN 0

#define LED_PIN 38

#define BATT_MAX 4.2
#define BATT_MIN 3.0

/***********************config*************************/

#define TFT_ROT 1

#if TFT_ROT == 0 || TFT_ROT == 2
#define LCD_WIDTH 170
#define LCD_HEIGHT 320
#elif TFT_ROT == 1 || TFT_ROT == 3
#define LCD_WIDTH 320
#define LCD_HEIGHT 170
#endif

// #define TFT_WIDTH             240
// #define TFT_HEIGHT            536
// #define TFT_WIDTH             536
// #define TFT_HEIGHT            240

// #define SEND_BUF_SIZE         (0x4000) //(LCD_WIDTH * LCD_HEIGHT + 8) / 10

// ################ ESP NOW ################


extern bool globalRelay1;
extern bool globalRelay2;
extern bool globalRelay3;
extern bool globalRelay4;
extern bool globalRelay5;
extern bool globalRelay6;

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

// #########################################
void initConfig();