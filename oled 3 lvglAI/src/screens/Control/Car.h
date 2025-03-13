#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Wireless.h"

// static void relayChangeCb(uint8_t cmd, bool *global)
// {
//   fullPacket fp;
//   memcpy(fp.mac, car_addr, 6);
//   fp.direction = PacketDirection::SEND;
//   fp.p.type = cmd;
//   fp.p.data[0] = !*global;

//   wireless.send(&fp);
// }

class CarControlScreen : public LVScreen
{
private:
  lv_obj_t *list;
  std::vector<std::function<void()> *> callbacks;

public:
  CarControlScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create door control buttons
    createActionButton("Lock Door", []()
                       {
                         //  data_packet p;
                         //  p.type = CMD_DOOR_LOCK;
                         //  p.len = 1;
                         //  p.data[0] = 0;
                         //  wireless.send(&p, car_addr);
                         //
                       });

    createActionButton("Unlock Door", []()
                       {
                         // data_packet p;
                         // p.type = CMD_DOOR_LOCK;
                         // p.len = 1;
                         // p.data[0] = 1;
                         // wireless.send(&p, car_addr);
                         //
                       });

    // Create navigation button
    createNavigationButton("Flash", "CarFlash");

    // Create relay toggles
    createRelayToggle("R 1", &globalRelay1, CMD_RELAY_1_SET, lv_color_make(255, 0, 0));
    createRelayToggle("R 2", &globalRelay2, CMD_RELAY_2_SET, lv_color_make(0, 0, 255));
    createRelayToggle("R 3", &globalRelay3, CMD_RELAY_3_SET);
    createRelayToggle("R 4", &globalRelay4, CMD_RELAY_4_SET);
    createRelayToggle("R 5", &globalRelay5, CMD_RELAY_5_SET);
    createRelayToggle("R 6", &globalRelay6, CMD_RELAY_6_SET);

    // Create back button
    createBackButton();

    // Create top bar with white background
    createTopBar(lv_color_white(), lv_color_black());
  }

  ~CarControlScreen()
  {
    for (auto cb : callbacks)
    {
      delete cb;
    }
  }

  void onEnter() override
  {
    btnLed.SetColor(0xffffff);

    // fullPacket fp;
    // memcpy(fp.mac, car_addr, 6);
    // fp.direction = PacketDirection::SEND;
    // fp.p.type = CMD_RELAY_ALL;
    // fp.p.len = 0;
    // wireless.send(&fp);
  }

  void onExit() override
  {
    btnLed.Off();
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
};