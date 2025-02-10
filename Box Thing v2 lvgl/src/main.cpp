#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "config.h"

#include "IO/GPIO.h"
#include "IO/Wireless.h"
#include "IO/Battery.h"

#include <lvgl.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS 0x3C

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// LVGL Tick Handler
#define LVGL_TICK_PERIOD 50

unsigned long lastTick = 0;

// Global variables for encoder state
int lastPosition = 0;

lv_obj_t *encoderLabel; // Global label for displaying the encoder value

void printBits(uint8_t byte)
{
  for (int i = 7; i >= 0; i--)
  {
    Serial.print((byte >> i) & 1);
  }
}

void displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  // int16_t width = area->x2 - area->x1 + 1;
  // int16_t height = area->y2 - area->y1 + 1;

  Serial.println("Area:");

  Serial.print("x1: ");
  Serial.print(area->x1);
  Serial.print(" y1: ");
  Serial.println(area->y1);

  Serial.print("x2: ");
  Serial.print(area->x2);
  Serial.print(" y2: ");
  Serial.println(area->y2);

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();

  Serial.println("px_map:");
  int index = 0;
  for (int y = area->y1; y <= area->y2; y++)
  {
    for (int x = area->x1; x <= area->x2; x+=8)
    {
      printBits(px_map[index]);

      index++;
    }
    Serial.println();
  }

  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();

  // uint16_t length = width * height / 8;
  // for (uint16_t i = 0; i < length; i++)
  // {
  //   // if (i < 8)
  //   //   continue;
  //   for (int j = 0; j < 8; j++)
  //   {
  //     Serial.print((px_map[i + 8] >> j) & 1 ? "#" : " ");
  //   }
  //   if (i % 16 == 0)
  //   {
  //     Serial.println("|");
  //   }
  // }

  // Serial.println();
  // Serial.println();
  // Serial.println();

  // int width = area->x2 - area->x1 + 1;

    Serial.println("px_map preview:");

    for (int y = area->y1; y <= area->y2; y++) {
        Serial.print(String(y) + " \t");
        for (int x = area->x1; x <= area->x2; x++) {
            int i = (y * (SCREEN_WIDTH / 8)) + (x / 8);
            int bit = 7 - (x % 8);
            bool pixelOn = (px_map[i] >> bit) & 1;

            Serial.print(pixelOn ? "#" : " ");
            display.drawPixel(x, y, pixelOn ? SH110X_WHITE : SH110X_BLACK);
        }
        Serial.println();
    }

    Serial.println("px_map preview done.");
    display.display();
    lv_display_flush_ready(disp);
}

unsigned long batteryLoopMs = 0;
char buffer[100];

void setupWiFi()
{

  Serial.println("[INFO] [SETUP] WiFi...");
  WiFi.mode(WIFI_STA);

  wm.setConfigPortalBlocking(false);

  if (wm.autoConnect(AP_SSID))
  {
    Serial.println("[INFO] [SETUP] Connected");
  }
  else
  {
    Serial.println("[INFO] [SETUP] Config portal running");
  }
}

void espNowCb(fullPacket *fp)
{
  if (fp->p.type == CMD_PING)
  {
  }
}

void setupESPNOW()
{
  wireless.setup();
}

void setup()
{
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH); // Set latch pin to high as soon as possible

  initConfig();

  Wire.begin();

  Serial.println("[INFO] [SETUP] Starting...");
  Serial.println("[INFO] [SETUP] IOInit...");
  GpIO::initIO();

  delay(3000);

  Serial.println("[INFO] [SETUP] Display...");

  display.begin(I2C_ADDRESS, true);
  display.clearDisplay();
  display.display();

  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_WHITE);
  display.display();

  delay(1000);

  display.clearDisplay();
  display.display();

  for (int i = 0; i < 63; i++)
  {
    display.drawPixel(i, i, SH110X_WHITE);
    display.display();
  }

  delay(1000);

  display.clearDisplay();
  display.display();

  delay(1000);

  lv_init();
  delay(100);

  // Initialize the display buffer
  static uint8_t buf1[SCREEN_WIDTH * SCREEN_HEIGHT * 4]; // 1-bit-per-pixel buffer (128 * 64 / 8 = 1024 bytes)
  static uint8_t buf2[SCREEN_WIDTH * SCREEN_HEIGHT * 4]; // Double buffer for smoother updates

  // Display Driver
  lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  
  lv_display_set_flush_cb(disp, displayFlush);
  lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_FULL);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
  lv_display_set_resolution(disp, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_physical_resolution(disp, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Create a simple LVGL label
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Rotate to Navigate\nPress to Select");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Create a label for the encoder value
  encoderLabel = lv_label_create(lv_scr_act());
  lv_label_set_text(encoderLabel, "Encoder: 0");
  lv_obj_align(encoderLabel, LV_ALIGN_CENTER, 0, 20); // Position the encoder label below the instruction label

  Serial.print("offset x: ");
  Serial.println(lv_display_get_offset_x(disp));
  Serial.print("offset y: ");
  Serial.println(lv_display_get_offset_y(disp));

  if (preferences.getBool("espnowOn", false))
  {
    setupESPNOW();
  }
  else
  {
    setupWiFi();
  }

  wireless.setRecvCb(espNowCb);

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  do
  {
    ClickButtonEnc.Update();
  } while (ClickButtonEnc.depressed);
}

void loop()
{
  if (millis() - lastTick > LVGL_TICK_PERIOD)
  {
    lv_tick_inc(LVGL_TICK_PERIOD);
    lastTick = millis();
  }

  lv_task_handler();

  int newPosition = encoderGetCount();
  if (newPosition != lastPosition)
  {
    Serial.print("Encoder Position: ");
    Serial.println(newPosition);
    lastPosition = newPosition;

    // Update the encoder value on the display
    char encoderText[20];
    snprintf(encoderText, sizeof(encoderText), "Encoder: %d", newPosition);
    lv_label_set_text(encoderLabel, encoderText);
  }

  if (!wireless.isSetupDone())
    wm.process();

  if (millis() - batteryLoopMs > 500)
  {
    batteryLoopMs = millis();
    batteryUpdate();
  }

  if (ClickButtonEnc.clicks != 0)
    lastInteract = millis();
}
