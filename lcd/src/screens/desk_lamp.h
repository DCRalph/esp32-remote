#pragma once

#include <Arduino.h>
#include "driver/Display.h"

#include "buttons.h"
#include "ha.h"

class DeskLamp : public Screen
{
public:
  DeskLamp(String _name, String _id) : Screen(_name, _id){};

  void draw() override;
  void update() override;

  void updateState();
  void updateState(DynamicJsonDocument doc);
  void updateState(DynamicJsonDocument doc, int index);

  String state = "unknown";
  String brightness = "unknown";

  long long prevMillis1 = 0;
  long long prevMillis2 = 0;
  bool firstRun = true;
};

void DeskLamp::draw()
{

  firstRun = millis() - prevMillis1 > 1000;
  prevMillis1 = millis();

  if (!firstRun && millis() - prevMillis2 > 2000)
  {
    prevMillis2 = millis();
    updateState();
  }

  display.sprite.setTextSize(3);
  display.sprite.setTextDatum(MC_DATUM);
  display.sprite.setTextColor(TFT_VIOLET);
  display.sprite.drawString("Desk Lamp", LCD_WIDTH / 2, LCD_HEIGHT / 2 - 50);

  display.sprite.setTextSize(5);
  display.sprite.setTextDatum(MC_DATUM);
  display.sprite.setTextColor(TFT_WHITE);
  display.sprite.drawString(state, LCD_WIDTH / 2, LCD_HEIGHT / 2);
  display.sprite.drawString(brightness, LCD_WIDTH / 2, LCD_HEIGHT / 2 + 50);
}

void DeskLamp::update()
{
  if (ClickButton0.clicks == -1)
    screenManager.setScreen("menu");

  if (ClickButton0.clicks == 1)
    updateState(ha.callService("light", "toggle", "light.midesklamp1s_9479"), 0);

  if (ClickButton0.clicks == 2)
    updateState(ha.getState("light.midesklamp1s_9479"));

  if (ClickButton1.clicks == 1)
  {
    DynamicJsonDocument doc(64);
    doc["entity_id"] = "light.midesklamp1s_9479";
    doc["brightness_pct"] = 100;

    String payload;
    serializeJson(doc, payload);

    updateState(ha.callService("light", "turn_on", "light.midesklamp1s_9479", payload), 0);
  }

  if (ClickButton1.clicks == 2)
  {
    DynamicJsonDocument doc(64);
    doc["entity_id"] = "light.midesklamp1s_9479";
    doc["brightness_pct"] = 1;

    String payload;
    serializeJson(doc, payload);

    updateState(ha.callService("light", "turn_on", "light.midesklamp1s_9479", payload), 0);
  }
}

void DeskLamp::updateState()
{
  updateState(ha.getState("light.midesklamp1s_9479"), -1);
}

void DeskLamp::updateState(DynamicJsonDocument doc)
{
  updateState(doc, -1);
}

void DeskLamp::updateState(DynamicJsonDocument doc, int index)
{
  if (index != -1)
  {
    state = doc[index]["state"].as<String>();

    char buf[10];
    sprintf(buf, "%i%%", map(doc[index]["attributes"]["brightness"].as<float>(), 0, 255, 0, 100));
    brightness = buf;

    return;
  }

  state = doc["state"].as<String>();

  char buf[10];
  sprintf(buf, "%i%%", map(doc["attributes"]["brightness"].as<float>(), 0, 255, 0, 100));
  brightness = buf;
}