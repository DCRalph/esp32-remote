#pragma once

#include <Arduino.h>
#include "secrets.h"

#include "driver/rtc_io.h"
#include "esp_sleep.h"

#include <Preferences.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_log.h"



extern Preferences preferences;
extern esp_sleep_wakeup_cause_t wakeup_reason;

#define ESP_NOW_CHANNEL 1

#define BAUD_RATE 115200

#define DEBUG true
#define DEBUG_ESP_NOW false

#define BTN_UP_PIN 13
#define BTN_DOWN_PIN 12
#define BTN_UP_GPIO GPIO_NUM_13
#define BTN_DOWN_GPIO GPIO_NUM_12

#define LED_PIN 38

#define BATT_MAX 4.2
#define BATT_MIN 3.0


/***********************config*************************/
#define LCD_USB_QSPI_DREVER 1

#define SPI_FREQUENCY 75000000
#define TFT_SPI_MODE SPI_MODE0
#define TFT_SPI_HOST SPI2_HOST

/***********************config*************************/

#define TFT_BIG

#ifdef TFT_BIG
#define TFT_SCALE 2
#else
#define TFT_SCALE 1
#endif

#define TFT_ROT 3

#if TFT_ROT == 0 || TFT_ROT == 2
#define LCD_WIDTH 240
#define LCD_HEIGHT 536
#elif TFT_ROT == 1 || TFT_ROT == 3
#define LCD_WIDTH 536
#define LCD_HEIGHT 240
#endif

#define SEND_BUF_SIZE (0x4000) //(LCD_WIDTH * LCD_HEIGHT + 8) / 10

#define TFT_DC 7
#define TFT_RES 17
#define TFT_CS 6
#define TFT_MOSI 18
#define TFT_SCK 47

#define TFT_QSPI_CS 6
#define TFT_QSPI_SCK 47
#define TFT_QSPI_D0 18
#define TFT_QSPI_D1 7
#define TFT_QSPI_D2 48
#define TFT_QSPI_D3 5
#define TFT_QSPI_RST 17

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
void configureDeepSleep();
void deepSleepSetup();