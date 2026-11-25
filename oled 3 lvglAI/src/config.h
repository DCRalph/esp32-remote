#pragma once

#include <Arduino.h>
#include "secrets.h"

#include "driver/rtc_io.h"
#include "esp_sleep.h"

#include <Preferences.h>

#include <RemoteProtocol.h>
#include <lvgl.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_log.h"
#include "IO/GPIO.h"

extern Preferences preferences;
extern esp_sleep_wakeup_cause_t wakeup_reason;

extern uint32_t fps;
extern uint32_t lastFps;
extern uint32_t frameTime;
extern uint32_t lastFrameTime;


#define BAUD_RATE 115200

#define DEBUG true

#define BTN_UP_PIN 13
#define BTN_DOWN_PIN 12
#define BTN_TRIGGER_PIN 14

#define BTN_UP_GPIO GPIO_NUM_13
#define BTN_DOWN_GPIO GPIO_NUM_12

#define LED_PIN 38

#define BTN_LED_R_PIN 1
#define BTN_LED_G_PIN 2
#define BTN_LED_B_PIN 3

#define BATT_MAX 4.2
#define BATT_MIN 3.0

/***********************config*************************/
#define LCD_USB_QSPI_DREVER 1

#define SPI_FREQUENCY 75000000
#define TFT_SPI_MODE SPI_MODE0
#define TFT_SPI_HOST SPI2_HOST

/***********************config*************************/

#define TFT_SCALE 1

#define LV_HOR_RES_MAX 536
#define LV_VER_RES_MAX 240
#define LVGL_LCD_BUF_SIZE     (LV_HOR_RES_MAX * LV_VER_RES_MAX)

#define LV_ROT 3

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

// Last relay states the car reported.
extern bool carRelays[RemoteProtocol::CarRelays::kRelayCount];
void applyCarRelays(const RemoteProtocol::CarRelays::Relays &relays);
// Ask the car to set relay `index`, or for every state; replies land in carRelays.
void requestCarRelay(uint8_t index, bool on);
void fetchCarRelays();
// Ask for the car's states and show the last reply on relay switches (one per relay).
void refreshCarRelaySwitches(lv_obj_t *const *switches);

// Box Thing Car.
static uint8_t remote_addr[6] = {0x30, 0x30, 0xf9, 0x2b, 0xba, 0xf0};

// #########################################
void initConfig();
void configureDeepSleep();
void deepSleepSetup();