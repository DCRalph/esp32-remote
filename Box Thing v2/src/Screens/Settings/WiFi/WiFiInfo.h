#pragma once

#include <Display.h>
#include <ScreenManager.h>
#include <Wireless.h>
#include <dcr_NetLink.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/TopBar.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"

namespace
{
  /** Two big centred lines, used for every non-connected state. */
  void wifiInfoDrawStatus(const char *line1, const char *line2, const String &footer)
  {
    const int centreX = display.width() / 2;

    display.setTextSize(U8G2_TEXT_TITLE);
    display.setTextDatum(TC_DATUM);
    display.drawString(line1, centreX, 14);
    display.drawString(line2, centreX, 32);

    display.setTextSize(U8G2_TEXT_MONO12);
    display.setTextDatum(TL_DATUM);
    display.drawString(footer, 0, 55);
  }

  void wifiInfoDraw()
  {
    display.setTextColor(TFT_WHITE);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "ch: %d status: %d", ESP_NOW_CHANNEL,
             static_cast<int>(wireless.getLastStatus()));

    if (WiFi.status() == WL_CONNECTED)
    {
      display.setTextSize(U8G2_TEXT_MONO12);
      display.setTextDatum(TL_DATUM);

      display.drawString("SSID: " + WiFi.SSID(), 0, TopBar::kHeight);
      display.drawString("IP: " + WiFi.localIP().toString(), 0, TopBar::kHeight + 10);
      display.drawString("RSSI: " + String(WiFi.RSSI()), 0, TopBar::kHeight + 20);
      display.drawString(WiFi.macAddress(), 0, TopBar::kHeight + 30);
      return;
    }

    if (wireless.isSetupDone())
    {
      wifiInfoDrawStatus("ESP-NOW", "Active", buffer);
      return;
    }

    switch (netLink.state())
    {
    case WiFiState::WiFiConnecting:
      wifiInfoDrawStatus("Connecting", "...", netLink.getStatus().offMessage);
      break;

    case WiFiState::WiFiError:
      wifiInfoDrawStatus("WiFi", "Error", netLink.getStatus().errorMessage);
      break;

    default:
      wifiInfoDrawStatus("Not", "Connected", buffer);
      break;
    }
  }

  void wifiInfoUpdate()
  {
    if (ClickButtonEnc.clicks == 1)
      screenManager.back();
  }
}

const Screen2 WiFiInfoScreen2 = {
    .name = "WiFi Info",
    .draw = wifiInfoDraw,
    .update = wifiInfoUpdate,
    .onEnter = nullptr,
    .onExit = nullptr,
};
