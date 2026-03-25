#pragma once

#include <functional>

enum class MenuInputMode
{
  Encoder,
  Buttons2,
  Buttons3,
  Buttons4
};

struct MenuInputConfig
{
  MenuInputMode mode = MenuInputMode::Buttons2;

  int navigationClicks = 1;
  int defaultSelectClicks = 1;

  std::function<int()> getEncoderPosition;
  std::function<void()> resetEncoderPosition;
  std::function<int()> getEncoderSelectClicks;

  std::function<int()> getUpClicks;
  std::function<int()> getDownClicks;
  std::function<int()> getSelectClicks;
  std::function<int()> getBackClicks;
};

namespace MenuInput
{
  void configure(const MenuInputConfig &config);
  void update();

  int getNavigationDelta();
  int getSelectClicks();
  int getBackClicks();

  int getNavigationClicks();
  int getDefaultSelectClicks();
  int getSelectClicksForMatch();
}
