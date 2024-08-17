#pragma once

#include <Arduino.h>
#include "secrets.h"

#include <Preferences.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

extern Preferences preferences;

#define ESP_NOW_CHANNEL 1
#define DEBUG_ESP_NOW

#define BAUD_RATE 115200

#define DEBUG true
#define DEBUG_ESP_NOW false

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

static uint8_t car_addr[6] = {0x30, 0x30, 0xf9, 0x2b, 0xba, 0xf0};

// #########################################
void initConfig();