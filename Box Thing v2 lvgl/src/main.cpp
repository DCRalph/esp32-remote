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

// Use these names consistently for LVGL display resolution.
#define DISPLAY_WIDTH  SCREEN_WIDTH
#define DISPLAY_HEIGHT SCREEN_HEIGHT

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

static const uint32_t BUFFER_SIZE = SCREEN_WIDTH * 10; // 10 lines per flush

// Allocate memory for the display buffer
static lv_color_t buf1[BUFFER_SIZE];

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;
// LVGL Tick Handler period in ms
#define LVGL_TICK_PERIOD 50
unsigned long lastTick = 0;

// Global variables for encoder state
int lastPosition = 0;

// For tracking currently selected menu index (global menu navigation)
uint8_t currentMenuItem = 0;  
const uint8_t totalMenuItems = 6; // total number of interactive items

// Array of pointers for our menu items (so we can highlight them)
lv_obj_t *menuItems[totalMenuItems];

// Dummy value for the numeric selector
int dummyNumber = 0;

// A flag for which page is active (e.g., main menu vs. a sub‐page)
enum MenuPage { MAIN_MENU, SETTINGS_PAGE, TOGGLES_PAGE };
MenuPage activePage = MAIN_MENU;

// Forward declarations for menu creation and update routines
void createMainMenu(void);
void createSettingsPage(void);
void createTogglesPage(void);
void updateMenuFocus(uint8_t focusedIndex);
void onEncoderButtonClick();

// -----------------------------------------------------------------
// Your existing flush callback (unchanged except for extra comments below)
// -----------------------------------------------------------------
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                   lv_color_t *color_p)
{
  uint16_t x1 = area->x1;
  uint16_t y1 = area->y1;
  uint16_t x2 = area->x2;
  uint16_t y2 = area->y2;

  // Calculate the width of the flush area
  uint16_t area_width = x2 - x1 + 1;

  // Serial.println("Flush");
  // Serial.print("x1: ");
  // Serial.print(x1);
  // Serial.print(" y1: ");
  // Serial.println(y1);
  // Serial.print("x2: ");
  // Serial.print(x2);
  // Serial.print(" y2: ");
  // Serial.println(y2);

  // Draw each pixel in the flush area
  for (uint16_t y = y1; y <= y2; y++) {
    for (uint16_t x = x1; x <= x2; x++) {
      // Compute the index relative to the flush area
      uint16_t idx = (y - y1) * area_width + (x - x1);

      // For convenience, a nonzero LVGL color means white
      if (color_p[idx].full == 0) {
        display.drawPixel(x, y, SH110X_BLACK);
      } else {
        display.drawPixel(x, y, SH110X_WHITE);
      }
    }
  }

  display.display();

  // Inform LVGL that flushing is done
  lv_disp_flush_ready(disp_drv);
}

// -----------------------------------------------------------------
// WiFi, ESPNOW, and other setup functions (unchanged)
// -----------------------------------------------------------------

void setupWiFi() {
  Serial.println("[INFO] [SETUP] WiFi...");
  WiFi.mode(WIFI_STA);
  wm.setConfigPortalBlocking(false);
  if (wm.autoConnect(AP_SSID)) {
    Serial.println("[INFO] [SETUP] Connected");
  } else {
    Serial.println("[INFO] [SETUP] Config portal running");
  }
}

void espNowCb(fullPacket *fp) {
  if (fp->p.type == CMD_PING) {
    // Dummy callback
  }
}

void setupESPNOW() { wireless.setup(); }

// -----------------------------------------------------------------
// Extra: Create a dummy menu system for non-touch navigation using
// the encoder and its button.
// -----------------------------------------------------------------

// Create main menu with two options: "Settings" and "Toggles"
void createMainMenu(void) {
  lv_obj_clean(lv_scr_act());  // wipe the screen

  // Create a title label
  lv_obj_t *title = lv_label_create(lv_scr_act());
  lv_label_set_text(title, "Main Menu");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

  // Create two menu items for illustration: Settings and Toggles.
  menuItems[0] = lv_label_create(lv_scr_act());
  lv_label_set_text(menuItems[0], "1: Settings");
  lv_obj_align(menuItems[0], LV_ALIGN_TOP_LEFT, 5, 20);

  menuItems[1] = lv_label_create(lv_scr_act());
  lv_label_set_text(menuItems[1], "2: Toggles");
  lv_obj_align(menuItems[1], LV_ALIGN_TOP_LEFT, 5, 35);

  // We'll free unused menu slots (dummy items can be added later)
  for (uint8_t i = 2; i < totalMenuItems; i++) {
    menuItems[i] = NULL;
  }

  currentMenuItem = 0;
  updateMenuFocus(currentMenuItem);
  activePage = MAIN_MENU;
}

// Create a Settings page with a dummy numeric
// selector (using a label to display current number)
void createSettingsPage(void) {
  lv_obj_clean(lv_scr_act());
  activePage = SETTINGS_PAGE;

  lv_obj_t *title = lv_label_create(lv_scr_act());
  lv_label_set_text(title, "Settings");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

  // Create a value label
  menuItems[0] = lv_label_create(lv_scr_act());
  char buf[32];
  snprintf(buf, sizeof(buf), "Value: %d", dummyNumber);
  lv_label_set_text(menuItems[0], buf);
  lv_obj_align(menuItems[0], LV_ALIGN_CENTER, 0, 0);

  // Create a back button
  menuItems[1] = lv_label_create(lv_scr_act());
  lv_label_set_text(menuItems[1], "< Back");
  lv_obj_align(menuItems[1], LV_ALIGN_BOTTOM_MID, 0, -5);

  // Clean out the rest
  for (uint8_t i = 2; i < totalMenuItems; i++) {
    menuItems[i] = NULL;
  }
  currentMenuItem = 0;
  updateMenuFocus(currentMenuItem);
}

// Create a Toggles page with dummy toggle switches
void createTogglesPage(void) {
  lv_obj_clean(lv_scr_act());
  activePage = TOGGLES_PAGE;

  lv_obj_t *title = lv_label_create(lv_scr_act());
  lv_label_set_text(title, "Toggles");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

  // Create two dummy toggles
  menuItems[0] = lv_switch_create(lv_scr_act());
  lv_obj_align(menuItems[0], LV_ALIGN_TOP_LEFT, 5, 20);
  lv_obj_t *label0 = lv_label_create(lv_scr_act());
  lv_label_set_text(label0, "Toggle 1");
  lv_obj_align_to(label0, menuItems[0], LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  menuItems[1] = lv_switch_create(lv_scr_act());
  lv_obj_align(menuItems[1], LV_ALIGN_TOP_LEFT, 5, 40);
  lv_obj_t *label1 = lv_label_create(lv_scr_act());
  lv_label_set_text(label1, "Toggle 2");
  lv_obj_align_to(label1, menuItems[1], LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Create a back button
  menuItems[2] = lv_label_create(lv_scr_act());
  lv_label_set_text(menuItems[2], "< Back");
  lv_obj_align(menuItems[2], LV_ALIGN_BOTTOM_MID, 0, -5);

  // Mark remaining slots as unused
  for (uint8_t i = 3; i < totalMenuItems; i++) {
    menuItems[i] = NULL;
  }
  currentMenuItem = 0;
  updateMenuFocus(currentMenuItem);
}

// Update visual focus indicator (e.g., color change) on the current menu item
void updateMenuFocus(uint8_t focusedIndex) {
  // Iterate over known menu items and adjust style.
  // For demo purposes, we change text color.
  for (uint8_t i = 0; i < totalMenuItems; i++) {
    if (menuItems[i] != NULL) {
      if (i == focusedIndex) {
        // Set highlighted style
        lv_obj_set_style_text_color(menuItems[i],
          lv_palette_main(LV_PALETTE_RED), 0);
      } else {
        lv_obj_set_style_text_color(menuItems[i],
          lv_color_black(), 0);
      }
    }
  }
}

// Handle encoder button click based on active page and selection
void onEncoderButtonClick() {
  switch (activePage) {
    case MAIN_MENU:
      if (currentMenuItem == 0) {
        // "Settings" selected
        createSettingsPage();
      } else if (currentMenuItem == 1) {
        // "Toggles" selected
        createTogglesPage();
      }
      break;
    case SETTINGS_PAGE:
      // On Settings page, if "back" is selected (assuming index 1)
      if (currentMenuItem == 1) {
        createMainMenu();
      }
      break;
    case TOGGLES_PAGE:
      if (currentMenuItem == 2) {
        createMainMenu();
      }
      break;
    default:
      break;
  }
}

// -----------------------------------------------------------------
// Setup and loop functions
// -----------------------------------------------------------------
void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);  // Set latch pin to high as soon as possible
  initConfig();

  Wire.begin();

  Serial.begin(115200);
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
  delay(1000);

  // A simple test drawing diagonal pixels
  for (int i = 0; i < 63; i++) {
    display.drawPixel(i, i, SH110X_WHITE);
    display.display();
  }
  delay(1000);
  display.clearDisplay();
  display.display();
  delay(1000);

  // Initialize LVGL
  lv_init();
  delay(100);

  // Setup LVGL display buffer and driver.
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, BUFFER_SIZE);
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = DISPLAY_WIDTH;
  disp_drv.ver_res = DISPLAY_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;  // our flush callback
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Create the main menu
  createMainMenu();

  // Setup WiFi/ESPNOW as before
  if (preferences.getBool("espnowOn", false)) {
    setupESPNOW();
  } else {
    setupWiFi();
  }
  wireless.setRecvCb(espNowCb);

  Serial.println("[INFO] [SETUP] Done");
  Serial.println();

  // Wait for any encoder button depressed state to end.
  do {
    ClickButtonEnc.Update();
  } while (ClickButtonEnc.depressed);
}

unsigned long batteryLoopMs = 0;
char buffer[100];

void loop() {

  ClickButtonEnc.Update();
  // LVGL tick update
  if (millis() - lastTick > LVGL_TICK_PERIOD) {
    lv_tick_inc(LVGL_TICK_PERIOD);
    lastTick = millis();
  }
  lv_task_handler();

  // Handle encoder rotation
  int newPosition = encoderGetCount();
  if (newPosition != lastPosition) {
    // Positive rotation moves focus down; negative moves up.
    int diff = newPosition - lastPosition;
    lastPosition = newPosition;

    if (diff > 0) {
      // Move focus forward
      if (currentMenuItem < totalMenuItems - 1) {
        currentMenuItem++;
      }
    } else {
      // Move focus backward
      if (currentMenuItem > 0) {
        currentMenuItem--;
      }
    }
    updateMenuFocus(currentMenuItem);

    // If on Settings page, update the numeric value if focused item 0
    if (activePage == SETTINGS_PAGE && currentMenuItem == 0) {
      dummyNumber += diff;
      char numBuf[32];
      snprintf(numBuf, sizeof(numBuf), "Value: %d", dummyNumber);
      lv_label_set_text(menuItems[0], numBuf);
    }
  }

  // Check for encoder button click
  if (ClickButtonEnc.clicks != 0) {
    // Use button click as a selection (or "enter") event.
    onEncoderButtonClick();
    ClickButtonEnc.clicks = 0; // reset click count after handled
    lastInteract = millis();
  }

  // Process WiFi Manager if needed
  if (!wireless.isSetupDone()) {
    wm.process();
  }

  if (millis() - batteryLoopMs > 500) {
    batteryLoopMs = millis();
    batteryUpdate();
  }
}