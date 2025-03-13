#pragma once

#include "config.h"
#include "IO/LVScreen.h"

class RSSIMeter : public LVScreen
{
private:
  lv_obj_t *rssiLabel;
  lv_obj_t *valueLabel;
  lv_timer_t *updateTimer;

public:
  RSSIMeter(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create RSSI label
    rssiLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(rssiLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(rssiLabel, lv_color_make(0, 255, 255), 0);
    lv_label_set_text(rssiLabel, "RSSI");
    lv_obj_align(rssiLabel, LV_ALIGN_CENTER, 0, -30);

    // Create value label
    valueLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(valueLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(valueLabel, lv_color_white(), 0);
    lv_label_set_text_fmt(valueLabel, "%d", WiFi.RSSI());
    lv_obj_align(valueLabel, LV_ALIGN_CENTER, 0, 30);

    // Create back button
    createBackButton();

    // Create update timer
    updateTimer = lv_timer_create([](lv_timer_t *timer)
                                  {
      auto screen = static_cast<RSSIMeter*>(timer->user_data);
      screen->updateRSSI(); }, 1000, this);
  }

  ~RSSIMeter()
  {
    if (updateTimer)
    {
      lv_timer_del(updateTimer);
      updateTimer = nullptr;
    }
  }

private:
  void updateRSSI()
  {
    lv_label_set_text_fmt(valueLabel, "%d", WiFi.RSSI());
  }
};