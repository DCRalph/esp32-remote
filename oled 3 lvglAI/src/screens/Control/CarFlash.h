#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Wireless.h"

class CarFlashScreen : public LVScreen
{
private:
  lv_obj_t *list;
  uint8_t count = 5;
  uint8_t delay = 50;
  std::vector<std::function<void()> *> callbacks;

public:
  CarFlashScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create flash button
    createActionButton("Flash R 1", [this]()
                       {
                         //  data_packet p;
                         //  p.type = CMD_RELAY_1_FLASH;
                         //  p.len = 2;
                         //  p.data[0] = count;
                         //  p.data[1] = uint8_t(delay / 10);
                         //  wireless.send(&p, car_addr);
                         //
                       });

    // Create count spinbox
    createSpinbox("Count", &count, 1, 100, 1);

    // Create delay spinbox
    createSpinbox("Delay", &delay, 20, 250, 10);

    // Create back button
    createBackButton();

    // Create top bar
    createTopBar(lv_color_white(), lv_color_black());
  }

  ~CarFlashScreen()
  {
    for (auto cb : callbacks)
    {
      delete cb;
    }
  }

private:
  void createActionButton(const char *text, std::function<void()> action)
  {
    lv_obj_t *btn = lv_list_add_btn(list, nullptr, text);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);

    auto cb = new std::function<void()>(action);
    callbacks.push_back(cb);
    lv_obj_add_event_cb(btn, [](lv_event_t *e)
                        {
      auto cb = static_cast<std::function<void()>*>(e->user_data);
      (*cb)(); }, LV_EVENT_CLICKED, cb);

    if (navGroup)
      lv_group_add_obj(navGroup, btn);
  }

  void createSpinbox(const char *text, uint8_t *value, uint8_t min, uint8_t max, uint8_t step)
  {
    lv_obj_t *cont = lv_obj_create(list);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    lv_obj_t *spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(spinbox, min, max);
    lv_spinbox_set_step(spinbox, step);
    lv_spinbox_set_value(spinbox, *value);
    lv_obj_set_width(spinbox, 100);
    lv_obj_align(spinbox, LV_ALIGN_RIGHT_MID, -10, 0);

    lv_obj_add_event_cb(spinbox, [](lv_event_t *e)
                        {
      auto value = static_cast<uint8_t*>(e->user_data);
      *value = lv_spinbox_get_value(e->target); }, LV_EVENT_VALUE_CHANGED, value);

    if (navGroup)
    {
      lv_group_add_obj(navGroup, cont);
      lv_group_add_obj(navGroup, spinbox);
    }
  }
};