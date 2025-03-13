#pragma once

#include "config.h"
#include "IO/LVScreen.h"

// Helper function to convert bytes to a human-readable format (KB or MB)
static std::string formatBytes(size_t bytes)
{
  char buf[32];
  if (bytes < 1024)
  {
    snprintf(buf, sizeof(buf), "%u B", (unsigned)bytes);
  }
  else if (bytes < (1024 * 1024))
  {
    snprintf(buf, sizeof(buf), "%.2f KB", bytes / 1024.0f);
  }
  else
  {
    snprintf(buf, sizeof(buf), "%.2f MB", bytes / 1024.0f / 1024.0f);
  }
  return std::string(buf);
}

class SystemInfoScreen : public LVScreen
{
private:
  lv_obj_t *list;
  lv_timer_t *updateTimer;

public:
  SystemInfoScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create list
    list = lv_list_create(screen);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES - 32);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 32);

    // Create info items
    createInfoItem("Heap Size", formatBytes(ESP.getHeapSize()).c_str());
    createInfoItem("Free Heap", formatBytes(ESP.getFreeHeap()).c_str());
    createInfoItem("PSRAM Size", formatBytes(ESP.getPsramSize()).c_str());
    createInfoItem("Free PSRAM", formatBytes(ESP.getFreePsram()).c_str());

    // Create back button
    createBackButton();

    // Create update timer
    updateTimer = lv_timer_create([](lv_timer_t *timer)
                                  {
      auto screen = static_cast<SystemInfoScreen*>(timer->user_data);
      screen->updateInfo(); }, 1000, this);
  }

  ~SystemInfoScreen()
  {
    if (updateTimer)
    {
      lv_timer_del(updateTimer);
      updateTimer = nullptr;
    }
  }

private:
  void createInfoItem(const char *label, const char *value)
  {
    lv_obj_t *cont = lv_obj_create(list);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(cont, 5, 0);

    lv_obj_t *labelObj = lv_label_create(cont);
    lv_obj_set_style_text_font(labelObj, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(labelObj, lv_color_white(), 0);
    lv_label_set_text(labelObj, label);
    lv_obj_align(labelObj, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *valueObj = lv_label_create(cont);
    lv_obj_set_style_text_font(valueObj, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(valueObj, lv_color_make(0, 255, 255), 0);
    lv_label_set_text(valueObj, value);
    lv_obj_align(valueObj, LV_ALIGN_RIGHT_MID, -10, 0);

    if (navGroup)
      lv_group_add_obj(navGroup, cont);
  }

  void updateInfo()
  {
    // Update all info items with current values
    lv_obj_clean(list);
    createInfoItem("Heap Size", formatBytes(ESP.getHeapSize()).c_str());
    createInfoItem("Free Heap", formatBytes(ESP.getFreeHeap()).c_str());
    createInfoItem("PSRAM Size", formatBytes(ESP.getPsramSize()).c_str());
    createInfoItem("Free PSRAM", formatBytes(ESP.getFreePsram()).c_str());
  }
};
