#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"

static void relayChangeCb(uint8_t cmd, bool *global)
{
  fullPacket fp;
  memcpy(fp.mac, remote_addr, 6);
  fp.direction = PacketDirection::SEND;
  fp.p.type = cmd;
  fp.p.len = 1;
  fp.p.data[0] = !*global;

  wireless.send(&fp);
}

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
    createRelayToggle("R 1", &globalRelay1, CMD_RELAY_1_SET, lv_color_make(255, 0, 0));
    createRelayToggle("R 2", &globalRelay2, CMD_RELAY_2_SET, lv_color_make(0, 0, 255));
    createRelayToggle("R 3", &globalRelay3, CMD_RELAY_3_SET);
    createRelayToggle("R 4", &globalRelay4, CMD_RELAY_4_SET);
    createRelayToggle("R 5", &globalRelay5, CMD_RELAY_5_SET);
    createRelayToggle("R 6", &globalRelay6, CMD_RELAY_6_SET);
    createRelayToggle("R 7", &globalRelay7, CMD_RELAY_7_SET);
    createRelayToggle("R 8", &globalRelay8, CMD_RELAY_8_SET);

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
  void createRelayToggle(const char *text, bool *value, uint8_t cmd, lv_color_t color = lv_color_white())
  {
    lv_obj_t *cont = lv_obj_create(list);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_color(label, color, 0);

    lv_obj_t *sw = lv_switch_create(cont);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -10, 0);
    if (*value)
      lv_obj_add_state(sw, LV_STATE_CHECKED);

    struct RelayData
    {
      uint8_t cmd;
      bool *value;
    };
    auto data = new RelayData{cmd, value};

    lv_obj_add_event_cb(sw, [](lv_event_t *e)
                        {
      auto data = static_cast<RelayData*>(e->user_data);
      relayChangeCb(data->cmd, data->value); }, LV_EVENT_VALUE_CHANGED, data);

    if (navGroup)
    {
      lv_group_add_obj(navGroup, cont);
      lv_group_add_obj(navGroup, sw);
    }
  }

  void fetchRelayStates()
  {
    fullPacket fp;
    memcpy(fp.mac, remote_addr, 6);
    fp.direction = PacketDirection::SEND;
    fp.p.type = CMD_RELAY_ALL;
    fp.p.len = 0;
    wireless.send(&fp);
  }
};