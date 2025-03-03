#include "config.h" // Your project configuration (includes display dimensions, LV_ROT, etc.)
#include <lvgl.h>
#include <vector>
#include <stdio.h>
#include "driver/rm67162.h"
#include "IO/buttons.h" // Definitions for physical buttons (ClickButtonUP, etc.)
#include "IO/Battery.h" // Battery monitoring definitions

// Global LVGL group for menu navigation.
// A group lets you navigate via physical buttons (up/down) and trigger the focused object.
static lv_group_t *menuGroup = nullptr;

// A C++ vector used as a screen stack to store previous screens for back navigation.
static std::vector<lv_obj_t *> screenStack;

// Forward declarations of functions that create menu screens.
// You can expand these later to add additional screens.
lv_obj_t *create_main_menu();
lv_obj_t *create_screen1();
lv_obj_t *create_screen2();
lv_obj_t *create_list_screen();
lv_obj_t *create_popup_screen(const char *text);

// Forward declarations of event handlers.
void menu_item_event_handler(lv_event_t *e);
void list_item_event_handler(lv_event_t *e);

/**
 * @brief Focus event callback.
 *
 * When an object gets focused, its text color is set to red.
 * When it is defocused, the text color is set to white.
 *
 * @param e Pointer to the LVGL event.
 */
static void focus_event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_FOCUSED)
  {
    lv_obj_set_style_text_color(obj, lv_color_make(255, 0, 0), 0);
  }
  else if (code == LV_EVENT_DEFOCUSED)
  {
    lv_obj_set_style_text_color(obj, lv_color_white(), 0);
  }
}

// ---------------------------------------------------------------------
// Screen Stack Management
// ---------------------------------------------------------------------

/**
 * @brief Push a new screen onto the stack.
 *
 * The currently active screen is pushed onto the screenStack.
 * Then the new screen is loaded with an animation.
 *
 * @param scr Pointer to the new screen object.
 */
void push_screen(lv_obj_t *scr)
{
  lv_obj_t *current = lv_scr_act();
  screenStack.push_back(current);
  // Animate the transition: slide the new screen from the right.
  // Set auto_del to false so the previous screen is not deleted.
  lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
}

/**
 * @brief Pop the current screen and return to the previous one.
 *
 * If the screen stack is not empty, the last screen is popped and loaded
 * with a reverse animation.
 */
void pop_screen()
{
  if (!screenStack.empty())
  {
    lv_obj_t *scr = screenStack.back();
    screenStack.pop_back();
    // Animate the back transition: previous screen slides in from the left.
    // Set auto_del to false so the popped screen remains valid.
    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
  }
}

// ---------------------------------------------------------------------
// Display Driver Flush Callback
// ---------------------------------------------------------------------

/**
 * @brief LVGL display flush callback.
 *
 * This function sends a rectangular area of pixel data to your RM67162
 * display driver. After finishing, it notifies LVGL by calling
 * lv_disp_flush_ready().
 *
 * @param disp Pointer to the display driver descriptor.
 * @param area Area to update.
 * @param color_p Pointer to the pixel data.
 */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  // The rm67162 driver is used to push colors to the display.
  lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

// ---------------------------------------------------------------------
// Menu Item and List Event Handlers
// ---------------------------------------------------------------------

/**
 * @brief Generic event handler for menu items.
 *
 * When a menu item is clicked, this handler is invoked. The user data
 * (a constant string) indicates which menu item was selected.
 *
 * @param e Pointer to the LVGL event.
 */
void menu_item_event_handler(lv_event_t *e)
{
  const char *menu_id = (const char *)lv_event_get_user_data(e);

  if (strcmp(menu_id, "screen1") == 0)
  {
    lv_obj_t *scr = create_screen1();
    push_screen(scr);
  }
  else if (strcmp(menu_id, "screen2") == 0)
  {
    lv_obj_t *scr = create_screen2();
    push_screen(scr);
  }
  else if (strcmp(menu_id, "list") == 0)
  {
    lv_obj_t *scr = create_list_screen();
    push_screen(scr);
  }
  else if (strcmp(menu_id, "back") == 0)
  {
    pop_screen();
  }
}

/**
 * @brief Event handler for list screen items.
 *
 * When a list item in the list screen is activated, this handler creates
 * a popup screen displaying the selected item.
 *
 * @param e Pointer to the LVGL event.
 */
void list_item_event_handler(lv_event_t *e)
{
  lv_obj_t *target = lv_event_get_target(e);
  // Get the text from the list button.
  const char *txt = lv_list_get_btn_text(lv_obj_get_parent(target), target);
  lv_obj_t *popup = create_popup_screen(txt);
  push_screen(popup);
}

// ---------------------------------------------------------------------
// Menu Screens Creation Functions
// ---------------------------------------------------------------------

/**
 * @brief Create the main menu screen.
 *
 * The main menu is a list with three items: Screen 1, Screen 2, and List Screen.
 *
 * @return Pointer to the newly created main menu screen.
 */
lv_obj_t *create_main_menu()
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  // Create a list widget to hold menu options.
  lv_obj_t *list = lv_list_create(scr);
  lv_obj_set_size(list, LV_HOR_RES_MAX, LV_VER_RES_MAX);
  lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

  // Add items to the list and assign an event callback with a unique identifier.
  lv_obj_t *btn1 = lv_list_add_btn(list, NULL, "Screen 1");
  lv_obj_add_event_cb(btn1, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"screen1");
  lv_obj_set_style_text_color(btn1, lv_color_white(), 0);
  lv_obj_add_event_cb(btn1, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(btn1, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);

  lv_obj_t *btn2 = lv_list_add_btn(list, NULL, "Screen 2");
  lv_obj_add_event_cb(btn2, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"screen2");
  lv_obj_set_style_text_color(btn2, lv_color_white(), 0);
  lv_obj_add_event_cb(btn2, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(btn2, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);

  lv_obj_t *btn3 = lv_list_add_btn(list, NULL, "List Screen");
  lv_obj_add_event_cb(btn3, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"list");
  lv_obj_set_style_text_color(btn3, lv_color_white(), 0);
  lv_obj_add_event_cb(btn3, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(btn3, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);

  // Add each list item to the navigation group.
  lv_group_add_obj(menuGroup, btn1);
  lv_group_add_obj(menuGroup, btn2);
  lv_group_add_obj(menuGroup, btn3);

  return scr;
}

/**
 * @brief Create Screen 1.
 *
 * A simple screen with a label and a Back button.
 *
 * @return Pointer to Screen 1.
 */
lv_obj_t *create_screen1()
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, "Screen 1");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(label, lv_color_white(), 0);

  // Create a Back button.
  lv_obj_t *back_btn = lv_btn_create(scr);
  lv_obj_set_size(back_btn, 80, 40);
  lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, "Back");
  lv_obj_center(back_label);
  lv_obj_set_style_text_color(back_label, lv_color_white(), 0);
  lv_obj_add_event_cb(back_btn, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"back");
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);
  lv_group_add_obj(menuGroup, back_btn);

  return scr;
}

/**
 * @brief Create Screen 2.
 *
 * Similar to Screen 1 but displays "Screen 2".
 *
 * @return Pointer to Screen 2.
 */
lv_obj_t *create_screen2()
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text(label, "Screen 2");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(label, lv_color_white(), 0);

  lv_obj_t *back_btn = lv_btn_create(scr);
  lv_obj_set_size(back_btn, 80, 40);
  lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, "Back");
  lv_obj_center(back_label);
  lv_obj_set_style_text_color(back_label, lv_color_white(), 0);
  lv_obj_add_event_cb(back_btn, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"back");
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);
  lv_group_add_obj(menuGroup, back_btn);

  return scr;
}

/**
 * @brief Create the List Screen.
 *
 * A scrollable list of items. Selecting an item opens a popup screen.
 *
 * @return Pointer to the list screen.
 */
lv_obj_t *create_list_screen()
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  lv_obj_t *list = lv_list_create(scr);
  lv_obj_set_size(list, LV_HOR_RES_MAX, LV_VER_RES_MAX - 60);
  lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 0);

  // Populate the list with 10 items.
  for (int i = 1; i <= 10; i++)
  {
    char item_text[32];
    sprintf(item_text, "Item %d", i);
    lv_obj_t *item_btn = lv_list_add_btn(list, NULL, item_text);
    lv_obj_set_style_text_color(item_btn, lv_color_white(), 0);
    lv_obj_add_event_cb(item_btn, list_item_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(item_btn, focus_event_handler, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(item_btn, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);
    lv_group_add_obj(menuGroup, item_btn);
  }
  // Add a Back item.
  lv_obj_t *back_btn = lv_list_add_btn(list, NULL, "Back");
  lv_obj_set_style_text_color(back_btn, lv_color_white(), 0);
  lv_obj_add_event_cb(back_btn, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"back");
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);
  lv_group_add_obj(menuGroup, back_btn);

  return scr;
}

/**
 * @brief Create a popup screen.
 *
 * This screen displays the given text (e.g. the selected list item)
 * along with a Back button.
 *
 * @param text The text to display.
 * @return Pointer to the popup screen.
 */
lv_obj_t *create_popup_screen(const char *text)
{
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  lv_obj_t *label = lv_label_create(scr);
  lv_label_set_text_fmt(label, "Selected: %s", text);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(label, lv_color_white(), 0);

  lv_obj_t *back_btn = lv_btn_create(scr);
  lv_obj_set_size(back_btn, 80, 40);
  lv_obj_align(back_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, "Back");
  lv_obj_center(back_label);
  lv_obj_set_style_text_color(back_label, lv_color_white(), 0);
  lv_obj_add_event_cb(back_btn, menu_item_event_handler, LV_EVENT_CLICKED, (void *)"back");
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_FOCUSED, NULL);
  lv_obj_add_event_cb(back_btn, focus_event_handler, LV_EVENT_DEFOCUSED, NULL);
  lv_group_add_obj(menuGroup, back_btn);

  return scr;
}

// ---------------------------------------------------------------------
// Main Setup and Loop
// ---------------------------------------------------------------------

void setup()
{
  // Initialize hardware I/O.
  GpIO::initIO();
  buttons.setup();
  battery.init();
  battery.update();

  // Initialize the display driver and set rotation.
  rm67162_init();
  lcd_setRotation(LV_ROT); // LV_ROT is defined in config.h

  // Initialize LVGL.
  lv_init();

  // Allocate a LVGL draw buffer in PSRAM.
  static lv_disp_draw_buf_t draw_buf;
  lv_color_t *buf = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE);
  assert(buf);
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);

  // Initialize and register the LVGL display driver.
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.hor_res = LV_HOR_RES_MAX;
  disp_drv.ver_res = LV_VER_RES_MAX;
  lv_disp_drv_register(&disp_drv);

  // Create an LVGL group for menu navigation and set it as default.
  menuGroup = lv_group_create();
  lv_group_set_default(menuGroup);

  // Load the main menu with a fade-in animation.
  lv_obj_t *main_menu = create_main_menu();
  lv_scr_load_anim(main_menu, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
}

void loop()
{
  // Update the physical buttons' state.
  buttons.update();

  // Process navigation using the physical buttons.
  if (ClickButtonUP.clicks != 0)
  {
    for (int i = 0; i < abs(ClickButtonUP.clicks); i++)
    {
      if (ClickButtonUP.clicks > 0)
        lv_group_focus_prev(menuGroup);
      else
        lv_group_focus_next(menuGroup);
    }
    ClickButtonUP.clicks = 0;
  }
  if (ClickButtonDOWN.clicks != 0)
  {
    for (int i = 0; i < abs(ClickButtonDOWN.clicks); i++)
    {
      if (ClickButtonDOWN.clicks > 0)
        lv_group_focus_next(menuGroup);
      else
        lv_group_focus_prev(menuGroup);
    }
    ClickButtonDOWN.clicks = 0;
  }
  if (ClickButtonTRIGGER.clicks != 0)
  {
    // When the trigger is pressed, send a click event to the focused object.
    lv_obj_t *focused = lv_group_get_focused(menuGroup);
    if (focused != NULL)
    {
      lv_event_send(focused, LV_EVENT_CLICKED, NULL);
    }
    ClickButtonTRIGGER.clicks = 0;
  }

  // Handle LVGL tasks.
  lv_timer_handler();
  delay(2);
}
