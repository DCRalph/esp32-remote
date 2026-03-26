#include "MenuInput.h"

static MenuInputConfig s_config;
static int s_cachedNavDelta = 0;
static int s_cachedSelectClicks = 0;
static int s_cachedBackClicks = 0;

void MenuInput::configure(const MenuInputConfig &config)
{
  s_config = config;
  s_cachedNavDelta = 0;
  s_cachedSelectClicks = 0;
  s_cachedBackClicks = 0;
}

static void pollInputs()
{
  s_cachedNavDelta = 0;
  s_cachedSelectClicks = 0;
  s_cachedBackClicks = 0;

  switch (s_config.mode)
  {
  case MenuInputMode::Encoder:
  {
    if (s_config.getEncoderPosition)
    {
      s_cachedNavDelta = s_config.getEncoderPosition();
      if (s_config.resetEncoderPosition)
        s_config.resetEncoderPosition();
    }
    if (s_config.getEncoderSelectClicks)
      s_cachedSelectClicks = s_config.getEncoderSelectClicks();
    break;
  }
  case MenuInputMode::Buttons2:
  {
    int up = s_config.getUpClicks ? s_config.getUpClicks() : 0;
    int down = s_config.getDownClicks ? s_config.getDownClicks() : 0;
    int select = s_config.getSelectClicks ? s_config.getSelectClicks() : 0;
    int nav = s_config.navigationClicks > 0 ? s_config.navigationClicks : 1;
    if (down == nav)
      s_cachedNavDelta = 1;
    else if (up == nav)
      s_cachedNavDelta = -1;
    if (down > nav)
      s_cachedSelectClicks = down;
    else if (up > nav)
      s_cachedSelectClicks = up;
    if (select > 0)
      s_cachedSelectClicks = select;
    break;
  }
  case MenuInputMode::Buttons3:
  {
    int up = s_config.getUpClicks ? s_config.getUpClicks() : 0;
    int down = s_config.getDownClicks ? s_config.getDownClicks() : 0;
    int nav = s_config.navigationClicks > 0 ? s_config.navigationClicks : 1;
    if (down == nav)
      s_cachedNavDelta = 1;
    else if (up == nav)
      s_cachedNavDelta = -1;
    if (s_config.getSelectClicks)
      s_cachedSelectClicks = s_config.getSelectClicks();
    break;
  }
  case MenuInputMode::Buttons4:
  {
    int up = s_config.getUpClicks ? s_config.getUpClicks() : 0;
    int down = s_config.getDownClicks ? s_config.getDownClicks() : 0;
    int nav = s_config.navigationClicks > 0 ? s_config.navigationClicks : 1;
    if (down == nav)
      s_cachedNavDelta = 1;
    else if (up == nav)
      s_cachedNavDelta = -1;
    if (s_config.getSelectClicks)
      s_cachedSelectClicks = s_config.getSelectClicks();
    if (s_config.getBackClicks)
      s_cachedBackClicks = s_config.getBackClicks();
    break;
  }
  }
}

void MenuInput::update()
{
  pollInputs();
}

int MenuInput::getNavigationDelta()
{
  return s_cachedNavDelta;
}

int MenuInput::getSelectClicks()
{
  return s_cachedSelectClicks;
}

int MenuInput::getBackClicks()
{
  return s_cachedBackClicks;
}

int MenuInput::getNavigationClicks()
{
  return s_config.navigationClicks > 0 ? s_config.navigationClicks : 1;
}

int MenuInput::getDefaultSelectClicks()
{
  return s_config.defaultSelectClicks > 0 ? s_config.defaultSelectClicks : 1;
}

int MenuInput::getSelectClicksForMatch()
{
  int raw = s_cachedSelectClicks;
  if (raw == 0)
    return 0;
  int def = s_config.defaultSelectClicks > 0 ? s_config.defaultSelectClicks : 1;
  if (raw == def)
    return 1;
  return raw;
}
