#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Wireless.h"

class CarLocksScreen : public LVScreen
{
private:
  lv_obj_t *lockBtn;
  lv_obj_t *unlockBtn;

public:
  CarLocksScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create lock button
    lockBtn = lv_btn_create(screen);
    lv_obj_set_size(lockBtn, 120, 80);
    lv_obj_align(lockBtn, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    lv_obj_t *lockLabel = lv_label_create(lockBtn);
    lv_label_set_text(lockLabel, "Lock");
    lv_obj_center(lockLabel);
    lv_obj_set_style_text_font(lockLabel, &lv_font_montserrat_28, 0);

    lv_obj_add_event_cb(lockBtn, [](lv_event_t *e)
                        {
                          // data_packet p;
                          // p.type = CMD_DOOR_LOCK;
                          // p.len = 1;
                          // p.data[0] = 0; // lock
                          // wireless.send(&p, car_addr);
                          //
                        },
                        LV_EVENT_CLICKED, nullptr);

    // Create unlock button
    unlockBtn = lv_btn_create(screen);
    lv_obj_set_size(unlockBtn, 120, 80);
    lv_obj_align(unlockBtn, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

    lv_obj_t *unlockLabel = lv_label_create(unlockBtn);
    lv_label_set_text(unlockLabel, "Unlock");
    lv_obj_center(unlockLabel);
    lv_obj_set_style_text_font(unlockLabel, &lv_font_montserrat_28, 0);

    lv_obj_add_event_cb(unlockBtn, [](lv_event_t *e)
                        {
                          // data_packet p;
                          // p.type = CMD_DOOR_LOCK;
                          // p.len = 1;
                          // p.data[0] = 1; // unlock
                          // wireless.send(&p, car_addr);
                          // screenManager.back();
                          //
                        },
                        LV_EVENT_CLICKED, nullptr);

    // Add buttons to navigation group
    if (navGroup)
    {
      lv_group_add_obj(navGroup, lockBtn);
      lv_group_add_obj(navGroup, unlockBtn);
    }

    // Create back button
    createBackButton();
  }
};