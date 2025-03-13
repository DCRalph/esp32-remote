#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Buttons.h"
#include "IO/Wireless.h"

static const char *SETTINGS_TAG = "Settings";

class Settings : public LVScreen
{
private:
  lv_obj_t *list;
  bool rainbowMode;
  long bootCount;
  std::vector<std::function<void()> *> callbacks;

public:
  Settings(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group), rainbowMode(false)
  {
    // Create list for menu items
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create menu items
    createNavigationItem("Battery", "Battery", LV_SYMBOL_BATTERY_FULL);
    createNavigationItem("System Info", "System Info", LV_SYMBOL_SETTINGS);
    createNavigationItem("Wi-Fi Info", "Wi-Fi info", LV_SYMBOL_WIFI);
    createNavigationItem("RSSI", "RSSI", LV_SYMBOL_WIFI);

    // Create toggle for rainbow mode
    createToggleItem("Rainbow Mode", &rainbowMode);

    // Create boot count display
    createNumberItem("Boot Count", &bootCount);

    // Create back button
    createBackButton();

    // Create top bar
    createTopBar(lv_color_make(255, 0, 0), lv_color_white());
  }

  ~Settings()
  {
    for (auto cb : callbacks)
    {
      delete cb;
    }
  }

private:
  void createNavigationItem(const char *text, const char *target, const char *symbol)
  {
    lv_obj_t *btn = lv_list_add_btn(list, symbol, text);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);

    lv_obj_add_event_cb(btn, [](lv_event_t *e)
                        {
            const char* screenName = static_cast<const char*>(e->user_data);
            screenManager.setScreen(screenName); }, LV_EVENT_CLICKED, strdup(target));

    if (navGroup)
    {
      lv_group_add_obj(navGroup, btn);
    }
  }

  void createToggleItem(const char *text, bool *value)
  {
    lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_OK, text);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);

    // Create a checkbox
    lv_obj_t *cb = lv_checkbox_create(btn);
    lv_checkbox_set_text(cb, "");
    lv_obj_align(cb, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_state(cb, *value ? LV_STATE_CHECKED : 0);

    // Add event handler
    lv_obj_add_event_cb(cb, [](lv_event_t *e)
                        {
                          bool *value = static_cast<bool *>(e->user_data);
                          *value = lv_obj_get_state(e->target) & LV_STATE_CHECKED;
                          //
                        },
                        LV_EVENT_CLICKED, value);

    if (navGroup)
    {
      lv_group_add_obj(navGroup, btn);
      lv_group_add_obj(navGroup, cb);
    }
  }

  void createNumberItem(const char *text, long *value)
  {
    lv_obj_t *cont = lv_obj_create(list);
    lv_obj_set_size(cont, LV_HOR_RES - 20, 40);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);

    // Create label
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    // Create spinbox
    lv_obj_t *spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(spinbox, 0, 999999);
    lv_spinbox_set_value(spinbox, *value);
    lv_obj_set_width(spinbox, 100);
    lv_obj_align(spinbox, LV_ALIGN_RIGHT_MID, -10, 0);

    // Add event handler
    lv_obj_add_event_cb(spinbox, [](lv_event_t *e)
                        {
            long* value = static_cast<long*>(e->user_data);
            *value = lv_spinbox_get_value(static_cast<lv_obj_t*>(e->target)); }, LV_EVENT_VALUE_CHANGED, value);

    if (navGroup)
    {
      lv_group_add_obj(navGroup, spinbox);
    }
  }
};