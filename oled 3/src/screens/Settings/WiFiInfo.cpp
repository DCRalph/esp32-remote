#include "WiFiInfo.h"

#include "config.h"
#include <WiFi.h>
#include "Display.h"
#include "IO/Buttons.h"
#include "ScreenManager.h"

namespace WiFiInfoScreenNamespace
{

  void draw()
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
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0 || ClickButtonUP.clicks != 0)
      screenManager.back();
  }

} // namespace WiFiInfoScreenNamespace

const Screen2 WiFiInfoScreen = {
    .name = "Wi-Fi info",
    .draw = WiFiInfoScreenNamespace::draw,
    .update = WiFiInfoScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
