#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Wireless.h"

class BoxThingEncoderScreen : public LVScreen
{
private:
  lv_obj_t *titleLabel;
  lv_obj_t *clicksLabel;
  lv_obj_t *countLabel;
  uint64_t encCount = 0;
  int8_t clicks = 0;
  int8_t lastClicks = 0;

public:
  BoxThingEncoderScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create title label
    titleLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_label_set_text(titleLabel, "Encoder");
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 60);

    // Create clicks label
    clicksLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(clicksLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(clicksLabel, lv_color_white(), 0);
    lv_label_set_text_fmt(clicksLabel, "%d", lastClicks);
    lv_obj_align(clicksLabel, LV_ALIGN_CENTER, 0, 0);

    // Create count label
    countLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(countLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(countLabel, lv_color_white(), 0);
    lv_label_set_text_fmt(countLabel, "%d", encCount);
    lv_obj_align(countLabel, LV_ALIGN_BOTTOM_MID, 0, -60);

    // Create back button
    createBackButton();
  }

  void onRecv(fullPacket *fp)
  {
    memcpy(&encCount, fp->p.data, 8);
    memcpy(&clicks, fp->p.data + 8, 1);
    if (clicks != 0)
    {
      lastClicks = clicks;
    }

    // Update labels
    lv_label_set_text_fmt(clicksLabel, "%d", lastClicks);
    lv_label_set_text_fmt(countLabel, "%d", encCount);
  }
};