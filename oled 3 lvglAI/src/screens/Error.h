#pragma once

#include "config.h"
#include "IO/LVScreen.h"
#include "IO/Buttons.h"

class ErrorScreen : public LVScreen
{
public:
  ErrorScreen(const std::string &name, lv_group_t *group = nullptr)
      : LVScreen(name, group)
  {
    // Create error text
    lv_obj_t *errorLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(errorLabel, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(errorLabel, lv_color_make(255, 0, 0), 0);
    lv_label_set_text(errorLabel, "ERROR");
    lv_obj_align(errorLabel, LV_ALIGN_TOP_MID, 0, 50);

    // Create instruction text
    lv_obj_t *instructionLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(instructionLabel, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(instructionLabel, lv_color_white(), 0);
    lv_label_set_text(instructionLabel, "Press any button\nto continue");
    lv_obj_set_style_text_align(instructionLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(instructionLabel, LV_ALIGN_CENTER, 0, 0);

    // Add button event handlers
    lv_obj_add_event_cb(screen, [](lv_event_t *e)
                        {
            if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0) {
                screenManager.setScreen("Home");
            } }, LV_EVENT_PRESSED, nullptr);
  }
};