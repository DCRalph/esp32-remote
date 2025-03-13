#include "Popup.h"

static const char *POPUP_TAG = "Popup";

Popup::Popup(const std::string &_title, const std::string &_message)
    : title(_title), message(_message)
{
}

Popup::~Popup()
{
  close();
}

void Popup::show()
{
  // Create popup container
  popup = lv_obj_create(lv_scr_act());
  lv_obj_set_size(popup, LV_HOR_RES - 60, LV_VER_RES - 40);
  lv_obj_align(popup, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_radius(popup, 10, 0);
  lv_obj_set_style_bg_color(popup, lv_color_white(), 0);
  lv_obj_set_style_pad_all(popup, 0, 0);

  // Create content box
  contentBox = lv_obj_create(popup);
  lv_obj_set_size(contentBox, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_radius(contentBox, 10, 0);
  lv_obj_set_style_border_width(contentBox, 0, 0);
  lv_obj_set_style_pad_all(contentBox, 10, 0);
  lv_obj_set_style_bg_color(contentBox, lv_color_white(), 0);
  lv_obj_clear_flag(contentBox, LV_OBJ_FLAG_SCROLLABLE);

  // Create title label
  titleLabel = lv_label_create(contentBox);
  lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(titleLabel, lv_color_black(), 0);
  lv_label_set_text(titleLabel, title.c_str());
  lv_obj_align(titleLabel, LV_ALIGN_TOP_LEFT, 0, 0);

  // Create message label
  messageLabel = lv_label_create(contentBox);
  lv_obj_set_style_text_font(messageLabel, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(messageLabel, lv_color_black(), 0);
  lv_label_set_text(messageLabel, message.c_str());
  lv_label_set_long_mode(messageLabel, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(messageLabel, LV_PCT(100));
  lv_obj_align(messageLabel, LV_ALIGN_TOP_LEFT, 0, 40);

  // Add click event to close popup
  lv_obj_add_event_cb(popup, closeEventHandler, LV_EVENT_CLICKED, this);
}

void Popup::close()
{
  if (popup)
  {
    lv_obj_del(popup);
    popup = nullptr;
    contentBox = nullptr;
    titleLabel = nullptr;
    messageLabel = nullptr;
  }
}

void Popup::closeEventHandler(lv_event_t *e)
{
  Popup *popup = static_cast<Popup *>(e->user_data);
  popup->close();
}

AutoClosePopup::AutoClosePopup(const std::string &_title, const std::string &_message, unsigned long _duration)
    : Popup(_title, _message), duration(_duration)
{
}

AutoClosePopup::~AutoClosePopup()
{
  close();
}

void AutoClosePopup::show()
{
  Popup::show();

  // Create progress bar
  progressBar = lv_bar_create(popup);
  lv_obj_set_size(progressBar, LV_PCT(100), 4);
  lv_obj_align(progressBar, LV_ALIGN_TOP_MID, 0, -2);
  lv_bar_set_range(progressBar, 0, 100);
  lv_bar_set_value(progressBar, 100, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(progressBar, lv_color_make(255, 0, 0), LV_PART_INDICATOR);

  // Create timer for auto-close
  closeTimer = lv_timer_create(timerCallback, duration / 100, this);
}

void AutoClosePopup::close()
{
  if (closeTimer)
  {
    lv_timer_del(closeTimer);
    closeTimer = nullptr;
  }
  Popup::close();
}

void AutoClosePopup::timerCallback(lv_timer_t *timer)
{
  AutoClosePopup *popup = static_cast<AutoClosePopup *>(timer->user_data);
  static int progress = 100;

  progress--;
  if (progress <= 0)
  {
    popup->close();
    progress = 100;
    return;
  }

  lv_bar_set_value(popup->progressBar, progress, LV_ANIM_OFF);
}
