#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/GPIO.h"

class RemoteRelayScreen : public LVScreen
{
private:
  lv_obj_t *list;
  lv_timer_t *updateTimer;
  uint64_t lastFetch = 0;

public:
  RemoteRelayScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create relay toggles
    createRelayToggle("R 1", 0, lv_color_make(255, 0, 0));
    createRelayToggle("R 2", 1, lv_color_make(0, 0, 255));
    createRelayToggle("R 3", 2);
    createRelayToggle("R 4", 3);
    createRelayToggle("R 5", 4);
    createRelayToggle("R 6", 5);

    // Create back button
    createBackButton();

    // Create update timer
    updateTimer = lv_timer_create([](lv_timer_t *timer)
                                  {
      auto screen = static_cast<RemoteRelayScreen*>(timer->user_data);
      screen->fetchRelayStates(); }, 1000, this);
  }

  ~RemoteRelayScreen()
  {
    if (updateTimer)
    {
      lv_timer_del(updateTimer);
      updateTimer = nullptr;
    }
  }

  void onEnter() override
  {
    fetchRelayStates();
  }

  void onExit() override
  {
    btnLed.Off();
  }

private:
  void createRelayToggle(const char *text, uint8_t index, lv_color_t color = lv_color_white())
  {
    lv_obj_t *cont = lv_obj_create(list);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_color(label, color, 0);

    lv_obj_t *sw = lv_switch_create(cont);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -10, 0);
    if (carRelays[index])
      lv_obj_add_state(sw, LV_STATE_CHECKED);

    lv_obj_add_event_cb(sw, [](lv_event_t *e)
                        { requestCarRelay(static_cast<uint8_t>(reinterpret_cast<uintptr_t>(e->user_data))); },
                        LV_EVENT_VALUE_CHANGED, reinterpret_cast<void *>(static_cast<uintptr_t>(index)));

    if (navGroup)
    {
      lv_group_add_obj(navGroup, cont);
      lv_group_add_obj(navGroup, sw);
    }
  }

  void fetchRelayStates()
  {
    fetchCarRelays();
  }
};