#pragma once

#include <Arduino.h>

#include "IO/GPIO.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

// WiFi

#define ESP_NOW_CHANNEL 1
#define DEBUG_ESP_NOW
#define ESPNOW_NO_DISABLE_WIFI

// RELAY BUSY

extern bool relay1Busy;
extern bool relay2Busy;
extern bool relay3Busy;
extern bool relay4Busy;
extern bool relay5Busy;
extern bool relay6Busy;
extern bool relay7Busy;
extern bool relay8Busy;

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

#define ARMED_PIN 15

// COMMANDS
#define CMD_PING 0x01

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

#define CMD_RELAY_7_SET 0x2d
#define CMD_RELAY_7_GET 0x2e

#define CMD_RELAY_8_SET 0x2f
#define CMD_RELAY_8_GET 0x30
