#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "buttons.h"
#include "ha.h"

class HALight : public Screen
{
public:
  HALight(String _id, String _entity_id) : Screen("", _id)
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
  String brightness = "unknown";

  long long prevMillis1 = 0;
  long long prevMillis2 = 0;
  bool firstRun = true;
};

void HALight::draw(void)
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
  display.sprite.drawString(brightness, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 50);
}

void HALight::update(void)
{
  if (ClickButton0.clicks == -1)
    screenManager.setScreen("menu");

  if (ClickButton0.clicks == 1)
    updateState(ha.callService("light", "toggle", entity_id), 0);

  if (ClickButton0.clicks == 2)
    updateState(ha.getState(entity_id));

  if (ClickButton1.clicks == 1)
  {
    DynamicJsonDocument doc(64);
    doc["entity_id"] = entity_id;
    doc["brightness_pct"] = 100;

    String payload;
    serializeJson(doc, payload);

    updateState(ha.callService("light", "turn_on", entity_id, payload), 0);
  }

  if (ClickButton1.clicks == 2)
  {
    DynamicJsonDocument doc(64);
    doc["entity_id"] = entity_id;
    doc["brightness_pct"] = 1;

    String payload;
    serializeJson(doc, payload);

    updateState(ha.callService("light", "turn_on", entity_id, payload), 0);
  }
}

void HALight::init(void)
{
  updateState();
  name = entityName;
}

void HALight::updateState()
{
  updateState(ha.getState(entity_id), -1);
}

void HALight::updateState(DynamicJsonDocument doc)
{
  updateState(doc, -1);
}

void HALight::updateState(DynamicJsonDocument doc, int index)
{
  if (index != -1)
  {
    state = doc[index]["state"].as<String>();

    char buf[10];
    sprintf(buf, "%i%%", map(doc[index]["attributes"]["brightness"].as<float>(), 0, 255, 0, 100));
    brightness = buf;

    entityName = doc[index]["attributes"]["friendly_name"].as<String>();

    return;
  }

  state = doc["state"].as<String>();

  char buf[10];
  sprintf(buf, "%i%%", map(doc["attributes"]["brightness"].as<float>(), 0, 255, 0, 100));
  brightness = buf;

  entityName = doc["attributes"]["friendly_name"].as<String>();
}