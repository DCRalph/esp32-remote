#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Buttons.h"
#include "IO/GPIO.h"

class HomeScreen : public LVScreen
{
private:
  lv_obj_t *list;
  std::vector<std::function<void()> *> callbacks; // Store callbacks to prevent memory leaks

public:
  HomeScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list for menu items
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32); // Leave space for top bar
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create menu items
    createMenuItem("Control", "Control", LV_SYMBOL_SETTINGS);
    createMenuItem("Settings", "Settings", LV_SYMBOL_SETTINGS, lv_color_make(255, 0, 0));
    createMenuItem("Remote Relay", "RemoteRelay", LV_SYMBOL_POWER);
    createMenuItem("Box Thing Encoder", "BoxThingEncoder", LV_SYMBOL_EDIT);
    createMenuItem("Power Off", "", LV_SYMBOL_POWER, lv_color_make(255, 0, 0),
                   []()
                   { esp_deep_sleep_start(); });

    // Create FPS and frame time display
    createInfoLabel("FPS", &lastFps);
    createInfoLabel("Frame Time", &lastFrameTime);

    // Create top bar with blue background
    createTopBar(lv_color_make(0, 0, 255), lv_color_white());
  }

  ~HomeScreen()
  {
    // Clean up callbacks
    for (auto cb : callbacks)
    {
      delete cb;
    }
  }

  void onEnter() override
  {
    // Set LED color to match top bar
    btnLed.SetColor(0x0000ff);
  }

  void onExit() override
  {
    btnLed.Off();
  }

private:
  void createMenuItem(const char *text, const char *target, const char *symbol,
                      lv_color_t color = lv_color_white(),
                      std::function<void()> action = nullptr)
  {
    lv_obj_t *btn = lv_list_add_btn(list, symbol, text);
    lv_obj_set_style_text_color(btn, color, 0);

    if (action)
    {
      auto cb = new std::function<void()>(action);
      callbacks.push_back(cb);
      lv_obj_add_event_cb(btn, [](lv_event_t *e)
                          {
        auto cb = static_cast<std::function<void()>*>(e->user_data);
        (*cb)(); }, LV_EVENT_CLICKED, cb);
    }
    else if (target && strlen(target) > 0)
    {
      lv_obj_add_event_cb(btn, [](lv_event_t *e)
                          {
        const char* screenName = static_cast<const char*>(e->user_data);
        screenManager.setScreen(screenName); }, LV_EVENT_CLICKED, strdup(target));
    }

    if (navGroup)
    {
      lv_group_add_obj(navGroup, btn);
    }
  }

  struct InfoLabelData
  {
    const char *prefix;
    uint32_t *value;
    lv_obj_t *label;

    InfoLabelData(const char *p, uint32_t *v, lv_obj_t *l)
        : prefix(p), value(v), label(l) {}
  };

  void createInfoLabel(const char *prefix, uint32_t *value)
  {
    lv_obj_t *label = lv_label_create(list);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    // Create data structure for timer
    auto data = new InfoLabelData(prefix, value, label);

    // Update the label periodically
    lv_timer_create([](lv_timer_t *timer)
                    {
      auto data = static_cast<InfoLabelData*>(timer->user_data);
      char buf[32];
      snprintf(buf, sizeof(buf), "%s: %lu", data->prefix, *(data->value));
      lv_label_set_text(data->label, buf); }, 1000, data);
  }
};