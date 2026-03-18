#pragma once

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"

class WiFiInfoScreen : public Screen
{
public:
  WiFiInfoScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void WiFiInfoScreen::draw()
{
  display.setTextDatum(TL_DATUM);

  display.setTextSize(3);
  display.setTextColor(TFT_ORANGE);
  display.drawString("Wi-Fi", 10, 30);

  String ssid = WiFi.SSID();
  String ip = WiFi.localIP().toString();
  String mac = WiFi.macAddress();
  String rssi = String(WiFi.RSSI());

  if (WiFi.isConnected())
  {
    display.setTextSize(2);
    display.setTextDatum(TL_DATUM);
    display.setTextColor(TFT_WHITE);

    display.drawString(rssi, 105, 33);

    display.setTextColor(TFT_BLUE);
    display.drawString(ssid, 10, 60);

    display.setTextColor(TFT_WHITE);
    display.drawString(ip, 10, 80);
    display.drawString(mac, 10, 100);
  }
  else
  {
    display.setTextSize(4);
    display.setTextDatum(TC_DATUM);
    display.setTextColor(TFT_RED);
    display.drawString("No Wi-Fi", LCD_WIDTH / 2, 80);
  }

  char buf[20];
}

void WiFiInfoScreen::update()
{
  if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
    screenManager.back();
}