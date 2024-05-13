#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"

class WiFiInfoScreen : public Screen
{
public:
  WiFiInfoScreen(String _name) : Screen(_name){};

  void draw() override;
  void update() override;
};

void WiFiInfoScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont12_tf);
  display.u8g2.setDrawColor(1);
  display.u8g2.drawStr(0, 8, "WiFi Info");
  display.u8g2.drawLine(0, 10, DISPLAY_WIDTH, 10);

  char buffer[32];

  if (WiFi.status() == WL_CONNECTED)
  {
    display.u8g2.setFont(u8g2_font_profont12_tf);

    sprintf(buffer, "SSID: %s", WiFi.SSID().c_str());
    display.u8g2.drawStr(0, 20, buffer);

    sprintf(buffer, "IP: %s", WiFi.localIP().toString().c_str());
    display.u8g2.drawStr(0, 30, buffer);

    sprintf(buffer, "RSSI: %d", WiFi.RSSI());
    display.u8g2.drawStr(0, 40, buffer);

    sprintf(buffer, "%s", WiFi.macAddress().c_str());
    display.u8g2.drawStr(0, 50, buffer);
  }
  else if (wm.getConfigPortalActive() || wm.getWebPortalActive())
  {

    display.u8g2.setFont(u8g2_font_logisoso16_tf);

    display.drawCenteredText(30, "Config Portal");
    display.drawCenteredText(48, "Active");

    display.u8g2.setFont(u8g2_font_profont12_tf);

    sprintf(buffer, "SSID: %s", wm.getConfigPortalSSID().c_str());
    display.u8g2.drawStr(0, 64, buffer);
  }
  else
  {
    display.u8g2.setFont(u8g2_font_profont12_tf);

    display.drawCenteredText(20, "Not");
    display.drawCenteredText(30, "Connected");

    sprintf(buffer, "ch: %d", WiFi.channel());
    display.u8g2.drawStr(0, 40, buffer);
  }
}

void WiFiInfoScreen::update()
{
  if (ClickButtonEnc.clicks == 1)
  {
    screenManager.back();
  }
}