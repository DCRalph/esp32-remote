#include "ScreenManager.h"
#include "ScreenFactory.h"
#include <esp_log.h>
#include <lvgl.h>

static const char *SCREEN_MANAGER_TAG = "ScreenManager";

ScreenManager::ScreenManager() : navGroup(nullptr), currentPopup(nullptr), popupActive(false)
{
}

void ScreenManager::init()
{
  ESP_LOGI(SCREEN_MANAGER_TAG, "Initializing ScreenManager");
  Serial.println("Initializing ScreenManager");

  // Set initial screen
  lv_obj_t *homeScreen = screenFactory.getScreen("Home");
  if (homeScreen != nullptr)
  {
    setScreen(homeScreen);
    ESP_LOGI(SCREEN_MANAGER_TAG, "Home screen loaded successfully");
    Serial.println("Home screen loaded successfully");
  }
  else
  {
    ESP_LOGE(SCREEN_MANAGER_TAG, "Failed to load Home screen!");
    Serial.println("Failed to load Home screen!");
    // Try to load error screen as fallback
    lv_obj_t *errorScreen = screenFactory.getScreen("Error");
    if (errorScreen != nullptr)
    {
      setScreen(errorScreen);
      ESP_LOGI(SCREEN_MANAGER_TAG, "Error screen loaded as fallback");
      Serial.println("Error screen loaded as fallback");
    }
    else
    {
      ESP_LOGE(SCREEN_MANAGER_TAG, "Failed to load any screen!");
      Serial.println("Failed to load any screen!");
    }
  }
}

void ScreenManager::setInitialFocus(lv_obj_t *screen)
{
  if (!navGroup)
    return;

  // Clear current focus
  lv_group_remove_all_objs(navGroup);

  // Find first focusable object in the screen
  lv_obj_t *child = lv_obj_get_child(screen, 0);
  while (child != nullptr)
  {
    if (lv_obj_get_group(child))
    {
      lv_group_add_obj(navGroup, child);
      lv_group_focus_obj(child);
      break;
    }
    child = lv_obj_get_child(screen, lv_obj_get_child_cnt(screen) - 1);
  }
}

void ScreenManager::pushScreen(lv_obj_t *screen)
{
  if (lv_scr_act() != nullptr)
  {
    screenHistory.push_back(lv_scr_act());
  }

  // Load new screen with animation
  lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
  setInitialFocus(screen);

  ESP_LOGI(SCREEN_MANAGER_TAG, "Pushed new screen");
}

void ScreenManager::popScreen()
{
  if (!screenHistory.empty())
  {
    lv_obj_t *prevScreen = screenHistory.back();
    screenHistory.pop_back();

    // Load previous screen with reverse animation
    lv_scr_load_anim(prevScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    setInitialFocus(prevScreen);

    ESP_LOGI(SCREEN_MANAGER_TAG, "Popped to previous screen");
  }
  else
  {
    ESP_LOGW(SCREEN_MANAGER_TAG, "No screens in history to pop");
  }
}

void ScreenManager::setScreen(lv_obj_t *screen)
{
  if (lv_scr_act() != nullptr && lv_scr_act() != screen)
  {
    screenHistory.push_back(lv_scr_act());
  }
  lv_scr_load(screen);
  setInitialFocus(screen);
  ESP_LOGI(SCREEN_MANAGER_TAG, "Set new screen");
}

void ScreenManager::setScreen(const std::string &name)
{
  lv_obj_t *screen = screenFactory.getScreen(name);
  if (screen != nullptr)
  {
    if (lv_scr_act() != nullptr && lv_scr_act() != screen)
    {
      screenHistory.push_back(lv_scr_act());
    }
    lv_scr_load(screen);
    setInitialFocus(screen);
    ESP_LOGI(SCREEN_MANAGER_TAG, "Set new screen: %s", name.c_str());
  }
  else
  {
    ESP_LOGE(SCREEN_MANAGER_TAG, "Failed to get screen: %s", name.c_str());
  }
}

lv_obj_t *ScreenManager::getCurrentScreen()
{
  return lv_scr_act();
}

void ScreenManager::clearHistory()
{
  screenHistory.clear();
}

void ScreenManager::showPopup(Popup *popup)
{
  currentPopup = popup;
  popupActive = true;

  // Create LVGL popup container
  lv_obj_t *popupObj = lv_obj_create(lv_scr_act());
  lv_obj_set_size(popupObj, LV_HOR_RES - 60, LV_VER_RES - 40);
  lv_obj_align(popupObj, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_color(popupObj, lv_color_white(), 0);
  lv_obj_set_style_radius(popupObj, 10, 0);

  // Add title
  lv_obj_t *title = lv_label_create(popupObj);
  lv_obj_set_style_text_font(title, LV_FONT_DEFAULT, 0);
  lv_obj_set_style_text_color(title, lv_color_black(), 0);
  lv_label_set_text(title, popup->title.c_str());
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, 10, 10);

  // Add message
  lv_obj_t *msg = lv_label_create(popupObj);
  lv_obj_set_style_text_font(msg, LV_FONT_DEFAULT, 0);
  lv_obj_set_style_text_color(msg, lv_color_black(), 0);
  lv_label_set_text(msg, popup->message.c_str());
  lv_label_set_long_mode(msg, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(msg, LV_HOR_RES - 80);
  lv_obj_align(msg, LV_ALIGN_TOP_LEFT, 10, 50);
}

void ScreenManager::closePopup()
{
  if (popupActive && currentPopup)
  {
    // Find and delete the popup container
    lv_obj_t *act_scr = lv_scr_act();
    lv_obj_t *popup = lv_obj_get_child(act_scr, 0);
    if (popup)
    {
      lv_obj_del(popup);
    }

    delete currentPopup;
    currentPopup = nullptr;
    popupActive = false;
  }
}

void ScreenManager::updatePopup()
{
  if (popupActive && currentPopup)
  {
    // Handle any popup updates if needed
  }
}

void ScreenManager::handleUpButton()
{
  if (navGroup)
  {
    lv_group_focus_prev(navGroup);
  }
}

void ScreenManager::handleDownButton()
{
  if (navGroup)
  {
    lv_group_focus_next(navGroup);
  }
}

void ScreenManager::handleTriggerButton()
{
  if (navGroup)
  {
    lv_obj_t *focused = lv_group_get_focused(navGroup);
    if (focused)
    {
      lv_event_send(focused, LV_EVENT_CLICKED, nullptr);
    }
  }
}

ScreenManager screenManager;
