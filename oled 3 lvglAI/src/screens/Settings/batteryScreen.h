#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Buttons.h"
#include "IO/Battery.h"

class BatteryScreen : public LVScreen
{
private:
  lv_obj_t *batteryIcon;
  lv_obj_t *voltageLabel;
  lv_obj_t *percentLabel;
  lv_timer_t *updateTimer;

public:
  BatteryScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create battery icon
    batteryIcon = lv_obj_create(screen);
    lv_obj_set_size(batteryIcon, 80, LV_VER_RES - 120);
    lv_obj_align(batteryIcon, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_set_style_radius(batteryIcon, 10, 0);
    lv_obj_set_style_border_width(batteryIcon, 2, 0);
    lv_obj_set_style_border_color(batteryIcon, lv_color_white(), 0);

    // Create battery terminal
    lv_obj_t *terminal = lv_obj_create(screen);
    lv_obj_set_size(terminal, 20, 10);
    lv_obj_align_to(terminal, batteryIcon, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(terminal, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(terminal, LV_OPA_COVER, 0);

    // Create labels
    lv_obj_t *titleLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(titleLabel, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_make(0, 255, 0), 0);
    lv_label_set_text(titleLabel, "Battery");
    lv_obj_set_style_text_opa(titleLabel, LV_OPA_COVER, 0);
    lv_obj_set_style_text_line_space(titleLabel, -8, 0); // Tighter line spacing
    lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 10, 50);

    voltageLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(voltageLabel, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(voltageLabel, lv_color_white(), 0);
    lv_obj_align(voltageLabel, LV_ALIGN_LEFT_MID, 10, -20);

    percentLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(percentLabel, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(percentLabel, lv_color_white(), 0);
    lv_obj_align(percentLabel, LV_ALIGN_LEFT_MID, 10, 20);

    // Create back button
    createBackButton();

    // Create top bar
    createTopBar(lv_color_make(0, 255, 0), lv_color_black());

    // Create update timer
    updateTimer = lv_timer_create([](lv_timer_t *timer)
                                  {
            auto screen = static_cast<BatteryScreen*>(timer->user_data);
            screen->updateBatteryDisplay(); }, 1000, this);

    // Initial update
    updateBatteryDisplay();
  }

  ~BatteryScreen()
  {
    if (updateTimer)
    {
      lv_timer_del(updateTimer);
    }
  }

private:
  void updateBatteryDisplay()
  {
    // Update voltage label
    char voltageBuf[16];
    snprintf(voltageBuf, sizeof(voltageBuf), "%.2fV", battery.getVoltage());
    lv_label_set_text(voltageLabel, voltageBuf);

    // Update percentage label
    char percentBuf[16];
    snprintf(percentBuf, sizeof(percentBuf), "%d%%", battery.getPercentageI());
    lv_label_set_text(percentLabel, percentBuf);

    // Update battery fill
    float percent = battery.getPercentageF();
    percent = std::max(0.0f, std::min(100.0f, percent));
    int height = (percent / 100.0f) * (LV_VER_RES - 124);

    lv_color_t fillColor;
    if (percent < 20)
    {
      fillColor = lv_color_make(255, 0, 0);
    }
    else if (percent < 50)
    {
      fillColor = lv_color_make(255, 165, 0);
    }
    else
    {
      fillColor = lv_color_make(0, 255, 0);
    }

    // Create or update fill rectangle
    static lv_obj_t *fillRect = nullptr;
    if (!fillRect)
    {
      fillRect = lv_obj_create(batteryIcon);
      lv_obj_set_style_radius(fillRect, 8, 0);
    }

    lv_obj_set_size(fillRect, 76, height);
    lv_obj_align(fillRect, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_obj_set_style_bg_color(fillRect, fillColor, 0);
    lv_obj_set_style_bg_opa(fillRect, LV_OPA_COVER, 0);
  }
};