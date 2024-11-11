#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"
#include "IO/ML.h"
#include "IO/CoProcessor.h"
#include "IO/Experiment.h"
#include "IO/test35.h"

class BactosureMainScreen : public Screen
{
public:
  BactosureMainScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  MenuItemAction initML = MenuItemAction("Init ML", 2, []()
                                         {
                                           ml.init();
                                           //
                                         });

  MenuItemAction ledToggle = MenuItemAction("Toggle LED", 2, []()
                                            {
                                              coProcessor[0]->whiteLED.toggle();
                                              //
                                            });


                                            MenuItemAction startExperiment = MenuItemAction("Start Experiment", 2, []()
                                            {
                                              Experiment *experiment = new test35();

                                              experimentManager.start(experiment, 0);
                                              //
                                            });

                                            MenuItemNavigate experimentScreen = MenuItemNavigate("Experiments", "BactosureExperScreen");

  void draw() override;
  void update() override;
};

BactosureMainScreen::BactosureMainScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);
  menu.addMenuItem(&initML);
  menu.addMenuItem(&ledToggle);
  menu.addMenuItem(&startExperiment);
  menu.addMenuItem(&experimentScreen);
}

void BactosureMainScreen::draw()
{
  // display.noTopBar();
  menu.draw();
}

void BactosureMainScreen::update()
{
  menu.update();
}