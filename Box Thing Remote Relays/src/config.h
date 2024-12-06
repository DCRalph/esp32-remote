#pragma once

#include <Arduino.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "armed.h"

// WiFi

#define ESP_NOW_CHANNEL 1
#define DEBUG_ESP_NOW
#define ESPNOW_NO_DISABLE_WIFI

//

extern bool remoteConnected;
extern uint64_t lastRemotePing;

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

#define RGB_STRIP_PIN 16

#define BATTERY_SENSE_PIN 3
#define BATTERY_SENSE_R1 9980 // 10k
#define BATTERY_SENSE_R2 997   // 1k
#define BATTERY_SENSE_VCC 3.3
#define BATTERY_SENSE_OFFSET 0.47

#define BATTERY2_SENSE_PIN 4
#define BATTERY2_SENSE_R1 4620 // 4.7k
#define BATTERY2_SENSE_R2 993   // 1k
#define BATTERY2_SENSE_VCC 3.3
#define BATTERY2_SENSE_OFFSET -0.07

// #define DEBUG_BATTERY

// Leds
#define NUM_LEDS 2

// COMMANDS
#define CMD_PING 0xd1
#define CMD_TEST 0xd2

#define CMD_ARM 0xf0
#define CMD_DISARM 0xf1

#define CMD_FIRE 0xf2
