#include "config.h"
#include "IO/Wireless.h"

#include <Wire.h>
#include <U8g2lib.h>
#include <lvgl.h>

// Create a U8g2 instance for the SSD1309 with the NONAME2 driver.
// Note: Change the reset pin if you've connected one; here U8X8_PIN_NONE is used.
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

/*
 * The display flush callback for LVGL.
 * LVGL calls this function when a region of the display needs to be refreshed.
 *
 * Because our U8g2 library’s "F" (full buffer) mode works by
 * clearing its internal buffer, drawing pixels, and then sending the entire
 * buffer to the display, we ignore the possibility of partial updates.
 *
 * In a 1-bit configuration we assume that a pixel value of 1 means "on" (i.e.
 * the pixel should be drawn) and 0 means off.
 */
static void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                          lv_color_t *color_p)
{
  // For a full-screen update, clear the U8g2 buffer.
  u8g2.clearBuffer();

  // (area->x1, area->y1) to (area->x2, area->y2) should cover the full screen when
  // using a full-screen buffer in LVGL. For each pixel in this area, draw it if the
  // corresponding lv_color_t value equals 1 (i.e. "on").
  uint16_t w = area->x2 - area->x1 + 1;
  for (uint16_t y = area->y1; y <= area->y2; y++)
  {
    for (uint16_t x = area->x1; x <= area->x2; x++)
    {
      uint16_t index = (y - area->y1) * w + (x - area->x1);
      // In our configuration: 1 = pixel on, 0 = pixel off.
      if (color_p[index].full == 0)
      {
        u8g2.drawPixel(x, y);
      }
    }
  }

  // Push the internal buffer to the display hardware.
  u8g2.sendBuffer();

  // Inform LVGL that flushing is done.
  lv_disp_flush_ready(disp_drv);
}

/*
 * Simple animation callback: this will be called as the animation executes.
 * It sets the x coordinate of the provided LVGL object.
 */
static void anim_x_cb(void *var, int32_t value)
{
  Serial.println(value);
  lv_obj_set_x(static_cast<lv_obj_t *>(var), value);
}

uint64_t ledBlinkMs = 0;
uint8_t blinks = 0;

void espNowRecvCb(fullPacket *fp)
{

  blinks = 4;
  ledBlinkMs = millis();
}

void setup()
{
  Serial.begin(115200);

  delay(200);
  WiFi.mode(WIFI_AP_STA);

  u8g2.begin(); // Initialize the OLED display
  lv_init();

  GpIO::initIO();

  wireless.setup();
  wireless.setOnReceiveOther(espNowRecvCb);

  led.On();
  delay(500);
  led.Off();

  static lv_color_t buf[128 * 64];
  static lv_disp_draw_buf_t disp_buf;
  lv_disp_draw_buf_init(&disp_buf, buf, nullptr, 128 * 64);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 128;
  disp_drv.ver_res = 64;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*----------------------------------------------------------------
   * Create a simple label object and animate its horizontal position.
   *----------------------------------------------------------------*/
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "LVGL");
  // In a monochrome display, use white (i.e. pixel=1) for drawn objects.
  lv_obj_set_style_text_color(label, LV_COLOR_BLACK, 0);
  // Start the label at x = 0 and vertically centered.
  lv_obj_set_pos(label, 0, 20);

  /* Set up an animation to move the label horizontally.
   * The label’s x coordinate will animate from 0 to (128 - label_width)
   * over 2000 ms, wait 500 ms, then animate back over 2000 ms.
   * The animation repeats infinitely.
   */

  int32_t start_x = 0;
  int32_t end_x = 80;
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, label);
  lv_anim_set_values(&a, start_x, end_x);
  lv_anim_set_time(&a, 500);          // Duration in milliseconds.
  lv_anim_set_repeat_delay(&a, 500);  // Delay before repeating.
  lv_anim_set_playback_time(&a, 500); // Duration for the reverse animation.
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

  lv_anim_set_exec_cb(&a, anim_x_cb);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

  lv_anim_start(&a);
}

uint64_t lastTime = 0;

void loop()
{
  // if (blinks > 0)
  // {
  //   if (millis() - ledBlinkMs > 100)
  //   {
  //     ledBlinkMs = millis();
  //     led.Toggle();
  //     blinks--;
  //   }

  //   if (blinks == 0)
  //     led.Off();
  // }

  // if (Serial.available() > 0)
  // {
  //   String input = Serial.readString();
  //   while (Serial.available() > 0)
  //     input += Serial.readString();
  //   if (input == "reboot")
  //     ESP.restart();
  //   else if (input == "getip")
  //     Serial.println(WiFi.localIP());
  //   else if (input == "getmac")
  //     Serial.println(WiFi.macAddress());
  //   else
  //     Serial.println("[INFO] [SERIAL] " + input);
  // }

  lv_timer_handler();

  lv_tick_inc(millis() - lastTime);
  lastTime = millis();
}
