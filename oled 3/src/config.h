#pragma once

#include <RemoteProtocol.h>
#include <Wireless.h>

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "driver/rtc_io.h"
#include "esp_sleep.h"

#include <Preferences.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_log.h"
#include "IO/GPIO.h"

extern Preferences preferences;
extern esp_sleep_wakeup_cause_t wakeup_reason;

// The ESP-NOW transport singleton, under the name the screens use.
extern Wireless &wireless;

// loop() sleeps out what is left of each frame. Buttons are sampled once per frame,
// so presses shorter than about 30 ms (20 ms debounce plus a sample) can be missed.
#define FRAME_INTERVAL_MS 10 // up to 100 fps

// dcr_timeProfiler key for display.render(): calls per second and duration.
#define PROFILE_FRAME "frame"

#define ESP_NOW_CHANNEL 1

#define BAUD_RATE 115200

#define DEBUG true
#define DEBUG_ESP_NOW false

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

// Last relay states the car reported; the RemoteRelay screen's toggles show these.
extern bool carRelays[RemoteProtocol::CarRelays::kRelayCount];
void applyCarRelays(const RemoteProtocol::CarRelays::Relays &relays);

// Box Thing Car.
static uint8_t remote_addr[6] = {0x30, 0x30, 0xf9, 0x2b, 0xba, 0xf0};

// #########################################
void initConfig();
void configureDeepSleep();
void deepSleepSetup();