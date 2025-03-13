#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Wireless.h"
#include "IO/GPIO.h"

class ControlScreen : public LVScreen
{
private:
  lv_obj_t *list;
  std::vector<std::function<void()> *> callbacks;

public:
  ControlScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create navigation buttons
    createNavigationButton("Car Control", "Car");

    // Create flash preset buttons
    createFlashPreset("Flash 5 30", 5, 30);
    createFlashPreset("Flash 2 40", 2, 40);
    createFlashPreset("Flash 8 50", 8, 50);

    // Create back button
    createBackButton();

    // Create top bar with cyan background
    createTopBar(lv_color_make(0, 255, 255), lv_color_black());
  }

  ~ControlScreen()
  {
    for (auto cb : callbacks)
    {
      delete cb;
    }
  }

  void onEnter() override
  {
    btnLed.SetColor(0x00ffff);
  }

  void onExit() override
  {
    btnLed.Off();
  }

private:
  void createNavigationButton(const char *text, const char *target)
  {
    lv_obj_t *btn = lv_list_add_btn(list, nullptr, text);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);

    lv_obj_add_event_cb(btn, [](lv_event_t *e)
                        {
      const char* target = static_cast<const char*>(e->user_data);
      screenManager.setScreen(target); }, LV_EVENT_CLICKED, strdup(target));

    if (navGroup)
      lv_group_add_obj(navGroup, btn);
  }

  void createFlashPreset(const char *text, uint8_t count, uint8_t delay)
  {
    lv_obj_t *btn = lv_list_add_btn(list, nullptr, text);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);

    auto cb = new std::function<void()>([count, delay]()
                                        {
      data_packet p;
      p.type = 0x30;
      p.len = 2;
      p.data[0] = count;
      p.data[1] = delay / 10;
      wireless.send(&p, remote_addr); });
    callbacks.push_back(cb);

    lv_obj_add_event_cb(btn, [](lv_event_t *e)
                        {
      auto cb = static_cast<std::function<void()>*>(e->user_data);
      (*cb)(); }, LV_EVENT_CLICKED, cb);

    if (navGroup)
      lv_group_add_obj(navGroup, btn);
  }
};
