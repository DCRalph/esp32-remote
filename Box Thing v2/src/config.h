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

#define BAUD_RATE 115200

// WiFi
#define HOSTNAME "esp32-box-thing"
#define AP_SSID "ESP32-Box-Thing"

// #define WIFI_SSID SECRET_SSID
// #define WIFI_PASS SECRET_PASS

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
#define BATTERY_SENSE_R2 997 // 1k
#define BATTERY_SENSE_VCC 3.3
#define BATTERY_SENSE_OFFSET 0.33

static uint8_t car_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd1};

void initConfig();
