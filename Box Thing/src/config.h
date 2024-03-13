#pragma once

#include <Arduino.h>
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

extern WiFiManager wm;

#define BAUD_RATE 74880

// WiFi
#define HOSTNAME "esp-box-thing"
#define AP_SSID "ESP-Box-Thing"

#define WIFI_SSID SECRET_SSID
#define WIFI_PASS SECRET_PASS

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// PINS
#define LED_PIN 2

#define LATCH_PIN 15

#define SWITCH_PIN 0

#define ENCODER_PIN_BUTTON 16
#define ENCODER_PIN_A 12
#define ENCODER_PIN_B 13

#define BATTERY_PIN A0
