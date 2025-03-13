#pragma once

#include "config.h"
#include "IO/LVScreen.h"

class WiFiInfoScreen : public LVScreen
{
private:
  lv_obj_t *titleLabel;
  lv_obj_t *rssiLabel;
  lv_obj_t *ssidLabel;
  lv_obj_t *ipLabel;
  lv_obj_t *macLabel;
  lv_timer_t *updateTimer;

public:
  WiFiInfoScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create title
    titleLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_make(255, 165, 0), 0);
    lv_label_set_text(titleLabel, "Wi-Fi");
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 10, 30);

    // Create RSSI label
    rssiLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(rssiLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(rssiLabel, lv_color_white(), 0);
    lv_obj_align(rssiLabel, LV_ALIGN_TOP_LEFT, 105, 33);

    // Create SSID label
    ssidLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(ssidLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(ssidLabel, lv_color_make(0, 0, 255), 0);
    lv_obj_align(ssidLabel, LV_ALIGN_TOP_LEFT, 10, 60);

    // Create IP label
    ipLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(ipLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(ipLabel, lv_color_white(), 0);
    lv_obj_align(ipLabel, LV_ALIGN_TOP_LEFT, 10, 80);

    // Create MAC label
    macLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(macLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(macLabel, lv_color_white(), 0);
    lv_obj_align(macLabel, LV_ALIGN_TOP_LEFT, 10, 100);

    // Create back button
    createBackButton();

    // Create update timer
    updateTimer = lv_timer_create([](lv_timer_t *timer)
                                  {
      auto screen = static_cast<WiFiInfoScreen*>(timer->user_data);
      screen->updateInfo(); }, 1000, this);

    // Initial update
    updateInfo();
  }

  ~WiFiInfoScreen()
  {
    if (updateTimer)
    {
      lv_timer_del(updateTimer);
      updateTimer = nullptr;
    }
  }

private:
  void updateInfo()
  {
    if (WiFi.isConnected())
    {
      lv_label_set_text_fmt(rssiLabel, "%d", WiFi.RSSI());
      lv_label_set_text(ssidLabel, WiFi.SSID().c_str());
      lv_label_set_text(ipLabel, WiFi.localIP().toString().c_str());
      lv_label_set_text(macLabel, WiFi.macAddress().c_str());
    }
    else
    {
      lv_obj_set_style_text_font(rssiLabel, &lv_font_montserrat_32, 0);
      lv_obj_set_style_text_color(rssiLabel, lv_color_make(255, 0, 0), 0);
      lv_label_set_text(rssiLabel, "No Wi-Fi");
      lv_obj_align(rssiLabel, LV_ALIGN_CENTER, 0, 0);

      lv_obj_add_flag(ssidLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(ipLabel, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(macLabel, LV_OBJ_FLAG_HIDDEN);
    }
  }
};