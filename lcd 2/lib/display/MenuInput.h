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

  /// Clicks on up/down that count as one navigation step (default 1).
  int navigationClicks = 1;
  /// Physical click count that runs the default action (items with clicksToRun 1). e.g. 2 for 2-button = double-tap to select.
  int defaultSelectClicks = 1;

  // Encoder mode: required
  std::function<int()> getEncoderPosition;
  std::function<void()> resetEncoderPosition;
  // Encoder mode: optional (e.g. encoder button for select)
  std::function<int()> getEncoderSelectClicks;

  // Button modes: required for navigation
  std::function<int()> getUpClicks;
  std::function<int()> getDownClicks;
  // Buttons3/4: dedicated select
  std::function<int()> getSelectClicks;
  // Buttons4: dedicated back
  std::function<int()> getBackClicks;
};

namespace MenuInput
{
  void configure(const MenuInputConfig &config);

  /// Call once per frame before reading inputs (e.g. at start of Menu::update / Popup::update).
  void update();

  int getNavigationDelta();
  int getSelectClicks();
  int getBackClicks();

  int getNavigationClicks();
  int getDefaultSelectClicks();
  /// Value to match against menu item clicksToRun (defaultSelectClicks physical -> 1).
  int getSelectClicksForMatch();
}
