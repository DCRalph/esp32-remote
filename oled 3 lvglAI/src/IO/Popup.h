#pragma once

#include "config.h"
#include "lvgl.h"
#include <string>
#include <functional>

class Popup
{
public:
  Popup(const std::string &_title, const std::string &_message);
  virtual ~Popup();
  virtual void show();
  virtual void close();

// protected:
  std::string title;
  std::string message;
  lv_obj_t *popup = nullptr;
  lv_obj_t *contentBox = nullptr;
  lv_obj_t *titleLabel = nullptr;
  lv_obj_t *messageLabel = nullptr;

  static void closeEventHandler(lv_event_t *e);
};

class AutoClosePopup : public Popup
{
private:
  unsigned long duration;
  lv_timer_t *closeTimer = nullptr;
  lv_obj_t *progressBar = nullptr;

public:
  AutoClosePopup(const std::string &_title, const std::string &_message, unsigned long _duration);
  ~AutoClosePopup() override;
  void show() override;
  void close() override;

private:
  static void timerCallback(lv_timer_t *timer);
};
