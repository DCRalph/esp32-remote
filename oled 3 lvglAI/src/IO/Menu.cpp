#include "Menu.h"
#include "ScreenManager.h"

static const char *TAG = "Menu";

// ###### MenuItem ######

MenuItem::MenuItem(const std::string &_name)
    : name(_name), type(MenuItemType::None)
{
  textColor = lv_color_white();
  activeTextColor = lv_color_black();
  bgColor = lv_color_white();
}

void MenuItem::addFunc(int8_t _clicksToRun, std::function<void()> _func)
{
  if (_clicksToRun == 0)
    return;

  ActionFunction actionFunc;
  actionFunc.clicksToRunUp = _clicksToRun;
  actionFunc.clicksToRunDown = _clicksToRun;
  actionFunc.func = _func;

  functions.push_back(actionFunc);
}

void MenuItem::addFunc(int8_t _clicksToRunUp, int8_t _clicksToRunDown, std::function<void()> _func)
{
  if (_clicksToRunUp == 0 && _clicksToRunDown == 0)
    return;

  ActionFunction actionFunc;
  actionFunc.clicksToRunUp = _clicksToRunUp;
  actionFunc.clicksToRunDown = _clicksToRunDown;
  actionFunc.func = _func;

  functions.push_back(actionFunc);
}

void MenuItem::setName(const std::string &_name)
{
  name = _name;
}

std::string MenuItem::getName() const
{
  return name;
}

MenuItemType MenuItem::getType() const
{
  return type;
}

void MenuItem::setHidden(bool _hidden)
{
  hidden = _hidden;
}

void MenuItem::setTextColor(lv_color_t _color)
{
  textColor = _color;
}

void MenuItem::setActiveTextColor(lv_color_t _color)
{
  activeTextColor = _color;
}

void MenuItem::setBgColor(lv_color_t _color)
{
  bgColor = _color;
}

bool MenuItem::isHidden() const
{
  return hidden;
}

void MenuItem::executeFunc()
{
  if (!functions.empty() && functions[0].clicksToRunUp == 2)
    functions[0].func();
}

void MenuItem::executeFunc(int8_t _clicks)
{
  for (const auto &actionFunc : functions)
  {
    if (actionFunc.clicksToRunUp == _clicks || actionFunc.clicksToRunDown == _clicks)
    {
      actionFunc.func();
      break;
    }
  }
}

// ###### MenuItemAction ######

MenuItemAction::MenuItemAction(const std::string &_name, int8_t _clicksToRun, std::function<void()> _func)
    : MenuItem(_name)
{
  type = MenuItemType::Action;
  addFunc(_clicksToRun, _func);
}

lv_obj_t *MenuItemAction::createLVGLObject(lv_obj_t *parent, lv_group_t *group)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, LV_PCT(100), LV_SIZE_CONTENT);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, getName().c_str());
  lv_obj_center(label);

  if (!functions.empty())
  {
    lv_obj_add_event_cb(btn, [](lv_event_t *e)
                        {
      auto func = static_cast<std::function<void()>*>(e->user_data);
      (*func)(); }, LV_EVENT_CLICKED, (void *)&functions[0].func);
  }

  if (group)
    lv_group_add_obj(group, btn);
  return btn;
}

// ###### MenuItemNavigate ######

MenuItemNavigate::MenuItemNavigate(const std::string &_name, const std::string &_target)
    : MenuItem(_name), target(_target)
{
  type = MenuItemType::Navigate;
  addFunc(2, [this]()
          { screenManager.setScreen(target); });
}

void MenuItemNavigate::addRoute(int8_t _clicksToRun, const std::string &_target)
{
  addFunc(_clicksToRun, [this, _target]()
          { screenManager.setScreen(_target); });
}

lv_obj_t *MenuItemNavigate::createLVGLObject(lv_obj_t *parent, lv_group_t *group)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, LV_PCT(100), LV_SIZE_CONTENT);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, getName().c_str());
  lv_obj_center(label);

  lv_obj_add_event_cb(btn, [](lv_event_t *e)
                      {
    auto target = static_cast<std::string*>(e->user_data);
    screenManager.setScreen(*target); }, LV_EVENT_CLICKED, (void *)&target);

  if (group)
    lv_group_add_obj(group, btn);
  return btn;
}

// ###### MenuItemBack ######

MenuItemBack::MenuItemBack() : MenuItem("Back")
{
  type = MenuItemType::Back;
  addFunc(2, []()
          { screenManager.popScreen(); });
}

lv_obj_t *MenuItemBack::createLVGLObject(lv_obj_t *parent, lv_group_t *group)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, LV_PCT(100), LV_SIZE_CONTENT);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, getName().c_str());
  lv_obj_center(label);

  lv_obj_add_event_cb(btn, [](lv_event_t *e)
                      { screenManager.popScreen(); }, LV_EVENT_CLICKED, nullptr);

  if (group)
    lv_group_add_obj(group, btn);
  return btn;
}

// ###### MenuItemToggle ######

MenuItemToggle::MenuItemToggle(const std::string &_name, bool *_value, bool _isMutable)
    : MenuItem(_name)
{
  type = MenuItemType::Toggle;
  value = _value;
  isMutable = _isMutable;

  addFunc(2, [this]()
          {
    if (isMutable) {
      *value = !*value;
      if (onChange) onChange();
    } });
}

void MenuItemToggle::setOnChange(std::function<void()> _onChange)
{
  onChange = _onChange;
}

void MenuItemToggle::removeOnChange()
{
  onChange = nullptr;
}

void MenuItemToggle::set(bool _value)
{
  if (isMutable)
    *value = _value;
}

bool MenuItemToggle::get() const
{
  return *value;
}

lv_obj_t *MenuItemToggle::createLVGLObject(lv_obj_t *parent, lv_group_t *group)
{
  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);

  lv_obj_t *label = lv_label_create(cont);
  lv_label_set_text(label, getName().c_str());
  lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);

  lv_obj_t *sw = lv_switch_create(cont);
  lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -10, 0);
  if (*value)
    lv_obj_add_state(sw, LV_STATE_CHECKED);

  if (isMutable)
  {
    lv_obj_add_event_cb(sw, [](lv_event_t *e)
                        {
      auto item = static_cast<MenuItemToggle*>(e->user_data);
      *item->value = lv_obj_has_state(e->target, LV_STATE_CHECKED);
      if (item->onChange) item->onChange(); }, LV_EVENT_VALUE_CHANGED, this);
  }

  if (group)
  {
    lv_group_add_obj(group, cont);
    lv_group_add_obj(group, sw);
  }
  return cont;
}

// ###### MenuItemNumber ######

template <typename T>
MenuItemNumber<T>::MenuItemNumber(const std::string &_name, T *_value, T _min, T _max)
    : MenuItem(_name)
{
  type = MenuItemType::Number;
  value = _value;
  min = _min;
  max = _max;
  isMutable = true;
}

template <typename T>
MenuItemNumber<T>::MenuItemNumber(const std::string &_name, T *_value, T _min, T _max, T _step)
    : MenuItem(_name)
{
  type = MenuItemType::Number;
  value = _value;
  min = _min;
  max = _max;
  step = _step;
  isMutable = true;
}

template <typename T>
MenuItemNumber<T>::MenuItemNumber(const std::string &_name, T *_value)
    : MenuItem(_name)
{
  type = MenuItemType::Number;
  value = _value;
  min = 0;
  max = 0;
  isMutable = false;
}

template <typename T>
void MenuItemNumber<T>::setOnChange(std::function<void()> _onChange)
{
  onChange = _onChange;
}

template <typename T>
void MenuItemNumber<T>::removeOnChange()
{
  onChange = nullptr;
}

template <typename T>
bool MenuItemNumber<T>::isSelected() const
{
  return selected;
}

template <typename T>
void MenuItemNumber<T>::increase()
{
  if (isMutable && selected && *value < max)
  {
    *value += step;
    if (onChange)
      onChange();
  }
}

template <typename T>
void MenuItemNumber<T>::decrease()
{
  if (isMutable && selected && *value > min)
  {
    *value -= step;
    if (onChange)
      onChange();
  }
}

template <typename T>
lv_obj_t *MenuItemNumber<T>::createLVGLObject(lv_obj_t *parent, lv_group_t *group)
{
  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);

  lv_obj_t *label = lv_label_create(cont);
  lv_label_set_text(label, getName().c_str());
  lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);

  if (isMutable)
  {
    lv_obj_t *spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(spinbox, min, max);
    lv_spinbox_set_value(spinbox, *value);
    lv_obj_set_width(spinbox, 100);
    lv_obj_align(spinbox, LV_ALIGN_RIGHT_MID, -10, 0);

    lv_obj_add_event_cb(spinbox, [](lv_event_t *e)
                        {
      auto item = static_cast<MenuItemNumber<T>*>(e->user_data);
      *item->value = lv_spinbox_get_value(e->target);
      if (item->onChange) item->onChange(); }, LV_EVENT_VALUE_CHANGED, this);

    if (group)
      lv_group_add_obj(group, spinbox);
  }
  else
  {
    lv_obj_t *value_label = lv_label_create(cont);
    lv_label_set_text_fmt(value_label, "%d", (int)*value);
    lv_obj_align(value_label, LV_ALIGN_RIGHT_MID, -10, 0);
  }

  if (group)
    lv_group_add_obj(group, cont);
  return cont;
}

// ###### Menu ######

Menu::Menu(lv_obj_t *parent, lv_group_t *group)
    : active(0), numItems(0), numItemsPerPage(0), offsetFromTop(0), navGroup(group)
{
  list = lv_list_create(parent);
  lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
  lv_obj_center(list);
}

Menu::~Menu()
{
  for (auto item : items)
  {
    delete item;
  }
}

void Menu::setItemsPerPage(uint8_t _itemsPerPage)
{
  maxItemsPerPage = _itemsPerPage;
  numItemsPerPage = numItems < maxItemsPerPage ? numItems : maxItemsPerPage;
}

uint8_t Menu::getItemsPerPage() const
{
  return maxItemsPerPage;
}

void Menu::setActive(uint8_t _active)
{
  active = _active;
}

uint8_t Menu::getActive() const
{
  return active;
}

void Menu::nextItem()
{
  if (active < numItems - 1)
  {
    do
    {
      active++;
    } while (items[active]->isHidden());
  }
}

void Menu::prevItem()
{
  if (active > 0)
  {
    do
    {
      active--;
    } while (items[active]->isHidden());
  }
}

void Menu::addMenuItem(MenuItem *_item)
{
  items.push_back(_item);
  numItems = items.size();
  numItemsPerPage = numItems < maxItemsPerPage ? numItems : maxItemsPerPage;

  _item->createLVGLObject(list, navGroup);
}

void Menu::refresh()
{
  lv_obj_clean(list);
  for (auto item : items)
  {
    item->createLVGLObject(list, navGroup);
  }
}
