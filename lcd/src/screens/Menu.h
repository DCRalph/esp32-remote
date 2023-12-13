#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "buttons.h"

class MenuScreen : public Screen
{
public:
  MenuScreen(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;

  int mainMenu = 0;
};

void MenuScreen::draw()
{

  display.sprite.fillSmoothRoundRect(0, 25, LCD_WIDTH, 30, 20, TFT_WHITE);

  display.sprite.setTextSize(2);
  display.sprite.setTextColor(TFT_BLACK);
  display.sprite.setTextDatum(ML_DATUM);

  display.sprite.drawString("Main menu", 10, 40);

  switch (mainMenu)
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:

  {

    screenUtils.drawMenuItem("RSSI", 10, 0, mainMenu == 0, TFT_RED);
    screenUtils.drawMenuItem(screenManager.getNameFromId("deskLamp"), 10, 1, mainMenu == 1, TFT_GREEN);
    screenUtils.drawMenuItem(screenManager.getNameFromId("leds"), 10, 2, mainMenu == 2, TFT_BLUE);
    screenUtils.drawMenuItem(screenManager.getNameFromId("matrix"), 10, 3, mainMenu == 3, TFT_BLUE);
    screenUtils.drawMenuItem(screenManager.getNameFromId("bedLight"), 10, 4, mainMenu == 4, TFT_YELLOW);
    screenUtils.drawMenuItem(screenManager.getNameFromId("michaelLeds"), 10, 5, mainMenu == 5, TFT_SKYBLUE);
    screenUtils.drawMenuItem(screenManager.getNameFromId("michaelFan"), 10, 6, mainMenu == 6, TFT_SKYBLUE);
    screenUtils.drawMenuItem(screenManager.getNameFromId("livingRoom"), 10, 7, mainMenu == 7, TFT_SKYBLUE);


    break;
  }
    // case 3:
    // case 4:
    // case 5:
    // {
    //   display.sprite.setTextSize(3);
    //   display.sprite.setTextColor(TFT_WHITE);
    //   display.sprite.setTextDatum(TL_DATUM);

    //   screenUtils.drawMenuItem("Menu 4", 10, 0, mainMenu == 3, TFT_YELLOW);
    //   screenUtils.drawMenuItem("Menu 5", 10, 1, mainMenu == 4, TFT_ORANGE);
    //   screenUtils.drawMenuItem("Menu 6", 10, 2, mainMenu == 5, TFT_PURPLE);

    //   break;
    // }
  }
}

void MenuScreen::update()
{

  if (ClickButton0.clicks == 1)
    mainMenu--;
  else if (ClickButton1.clicks == 1)
    mainMenu++;

  switch (mainMenu)
  {
  case 0:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("rssi");
    break;
  case 1:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("deskLamp");
    break;
  case 2:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("leds");
    break;
  case 3:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("matrix");
    break;
  case 4:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("bedLight");
    break;
  case 5:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("michaelLeds");
    break;
  case 6:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("michaelFan");
    break;
  case 7:
    if (ClickButton0.clicks == 2)
      screenManager.setScreen("livingRoom");
    break;
  }

  mainMenu = constrain(mainMenu, 0, 7);
}