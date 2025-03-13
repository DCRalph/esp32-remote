#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Wireless.h"

class SendScreen : public LVScreen
{
private:
  lv_obj_t *list;
  std::vector<std::function<void()> *> callbacks;

public:
  SendScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create send button
    createActionButton("Send", []()
                       {
      data_packet p;
      p.type = 43;
      p.len = 5;
      memcpy(p.data, "Hello", 5);
      wireless.send(&p, remote_addr); });

    // Create back button
    createBackButton();
  }

  ~SendScreen()
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
};