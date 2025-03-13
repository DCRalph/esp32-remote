#pragma once

#include "config.h"
#include <lvgl.h>
#include <string>
#include "ScreenManager.h"
#include "Battery.h"

class LVScreen
{
protected:
  lv_obj_t *screen;
  std::string name;
  lv_group_t *navGroup;
  lv_style_t screenStyle;

public:
  LVScreen(const std::string &screenName, lv_group_t *group = nullptr)
      : name(screenName), navGroup(group)
  {
    // Create base screen
    screen = lv_obj_create(nullptr);

    // Initialize and apply screen style
    lv_style_init(&screenStyle);
    lv_style_set_bg_color(&screenStyle, lv_color_black());
    lv_style_set_bg_opa(&screenStyle, LV_OPA_COVER);
    lv_obj_add_style(screen, &screenStyle, 0);

    // Set screen size
    lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
  }

  virtual ~LVScreen()
  {
    if (screen)
    {
      lv_obj_del(screen);
    }
    lv_style_reset(&screenStyle);
  }

  // Get the LVGL screen object
  lv_obj_t *getScreen() const { return screen; }

  // Get screen name
  const std::string &getName() const { return name; }

  // Set navigation group
  void setNavGroup(lv_group_t *group) { navGroup = group; }

  // Virtual functions for screen lifecycle
  virtual void onEnter() {}
  virtual void onExit() {}

protected:
  // Helper to create a back button
  lv_obj_t *createBackButton()
  {
    lv_obj_t *btn = lv_btn_create(screen);
    lv_obj_set_size(btn, 80, 40);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Back");
    lv_obj_center(label);

    lv_obj_add_event_cb(btn, [](lv_event_t *e)
                        {
                          screenManager.popScreen();
                          Serial.println("back");
                          //
                        },
                        LV_EVENT_CLICKED, nullptr);

    if (navGroup)
    {
      lv_group_add_obj(navGroup, btn);
    }

    return btn;
  }

  // Helper to create a top bar
  void createTopBar(lv_color_t bgColor, lv_color_t textColor)
  {
    // Create top bar container
    lv_obj_t *topBar = lv_obj_create(screen);
    lv_obj_set_size(topBar, LV_HOR_RES, 32);
    lv_obj_align(topBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(topBar, bgColor, 0);
    lv_obj_set_style_bg_opa(topBar, LV_OPA_COVER, 0);

    // Add screen name
    lv_obj_t *titleLabel = lv_label_create(topBar);
    lv_label_set_text(titleLabel, name.c_str());
    lv_obj_align(titleLabel, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_color(titleLabel, textColor, 0);

    // Add battery percentage
    lv_obj_t *batteryLabel = lv_label_create(topBar);
    char buf[10];
    snprintf(buf, sizeof(buf), "%d%%", battery.getPercentageI());
    lv_label_set_text(batteryLabel, buf);
    lv_obj_align(batteryLabel, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_color(batteryLabel, textColor, 0);
  }
};