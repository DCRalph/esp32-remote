#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "Buttons.h"
#include "ha.h"

class HASwitch : public Screen
{
public:
  HASwitch(String _id, String _entity_id) : Screen("", _id)
  {
    entity_id = _entity_id;
  };

  void draw(void) override;
  void update(void) override;

  void init(void);

  void updateState();
  void updateState(DynamicJsonDocument doc);
  void updateState(DynamicJsonDocument doc, int index);

  String entity_id;
  String entityName;

  String state = "unknown";

  long long prevMillis1 = 0;
  long long prevMillis2 = 0;
  bool firstRun = true;
};

void HASwitch::draw(void)
{

  firstRun = millis() - prevMillis1 > 1000;
  prevMillis1 = millis();

  if (!firstRun && millis() - prevMillis2 > 2000)
  {
    prevMillis2 = millis();
    updateState();
  }

  display.sprite.setTextSize(2);
  display.sprite.setTextDatum(MC_DATUM);
  display.sprite.setTextColor(TFT_SKYBLUE);
  display.sprite.drawString("HA Light", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 80);

  display.sprite.setTextSize(3);
  display.sprite.setTextDatum(MC_DATUM);
  display.sprite.setTextColor(TFT_VIOLET);
  display.sprite.drawString(entityName, LCD_WIDTH / 2, LCD_HEIGHT / 2 - 50);

  display.sprite.setTextSize(5);
  display.sprite.setTextDatum(MC_DATUM);
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.drawString(state, LCD_WIDTH / 2, LCD_HEIGHT / 2);
}

void HASwitch::update(void)
{
  if (ClickButtonDOWN.clicks == -1)
    screenManager.setScreen("menu");

  if (ClickButtonDOWN.clicks == 1)
  {
    ha.callService("switch", "toggle", entity_id);
    updateState();
  }

  if (ClickButtonDOWN.clicks == 2)
    updateState();
}

void HASwitch::init(void)
{
  updateState();
  name = entityName;
}

void HASwitch::updateState()
{
  updateState(ha.getState(entity_id), -1);
}

void HASwitch::updateState(DynamicJsonDocument doc)
{
  updateState(doc, -1);
}

void HASwitch::updateState(DynamicJsonDocument doc, int index)
{
  if (index != -1)
  {
    state = doc[index]["state"].as<String>();

    entityName = doc[index]["attributes"]["friendly_name"].as<String>();

    return;
  }

  state = doc["state"].as<String>();

  entityName = doc["attributes"]["friendly_name"].as<String>();
}