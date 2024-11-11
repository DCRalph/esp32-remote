#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/ML.h"
#include "IO/CoProcessor.h"
#include "IO/Experiment.h"
#include "IO/test35.h"

class BactosureExperScreen : public Screen
{
public:
  BactosureExperScreen(String _name);

  void draw() override;
  void update() override;
};

BactosureExperScreen::BactosureExperScreen(String _name) : Screen(_name)
{
}

void BactosureExperScreen::draw()
{
  display.noTopBar();

  // display current experiment

  display.sprite.setTextSize(2);
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.setTextDatum(TL_DATUM);



  std::vector<Experiment *> expers = experimentManager.getExperiments();

  if (expers.size() == 0)
  {
    display.sprite.drawString("No experiments running", 10, 10);

    return;
  }

  for (int i = 0; i < expers.size(); i++)
  {
    display.sprite.drawString(expers[i]->name, 10, 10 + i * 20);

    if (expers[i]->done)
    {
      display.sprite.drawString("Done", 100, 10 + i * 20);
    }
    else
    {
      display.sprite.drawString("Running", 100, 10 + i * 20);
    }
  }
}

void BactosureExperScreen::update()
{

  if (ClickButtonUP.clicks == 1)
  {
    screenManager.back();
  }
}