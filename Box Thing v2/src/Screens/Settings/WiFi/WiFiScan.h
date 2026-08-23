#pragma once

#include <Display.h>
#include <ScreenManager.h>
#include <dcr_NetLink.h>

#include "config.h"
#include "IO/GPIO.h"
#include "IO/Haptic.h"
#include "IO/TopBar.h"
#include "IO/U8g2DisplayDriver.h"
#include "Screens/Screens.h"
#include "Screens/Settings/WiFi/WiFiProvision.h"

namespace
{
  enum class WiFiScanState
  {
    Idle,
    Scanning,
    Results,
    Failed
  };

  constexpr int kWiFiScanRows = 4;
  constexpr int kWiFiScanRowHeight = 12;

  WiFiScanState gWiFiScanState = WiFiScanState::Idle;
  int gWiFiScanCount = 0;
  int gWiFiScanActive = 0;
  int gWiFiScanTop = 0;

  void wifiScanStart()
  {
    WiFi.scanDelete();
    gWiFiScanState = WiFiScanState::Scanning;
    gWiFiScanCount = 0;
    gWiFiScanActive = 0;
    gWiFiScanTop = 0;

    // beginScan pauses auto-reconnect and hands the radio to us.
    netLink.beginScan();
    WiFi.scanNetworks(true /* async */, true /* show hidden */);
  }

  /** Poll the async scan; returns once results are in or the scan failed. */
  void wifiScanPoll()
  {
    if (gWiFiScanState != WiFiScanState::Scanning)
      return;

    const int16_t result = WiFi.scanComplete();
    if (result == WIFI_SCAN_RUNNING)
      return;

    if (result < 0)
    {
      debugW("Scan failed (%d)", result);
      gWiFiScanState = WiFiScanState::Failed;
      netLink.endScan();
      return;
    }

    gWiFiScanCount = result;
    gWiFiScanState = result > 0 ? WiFiScanState::Results : WiFiScanState::Failed;
    debugI("Scan found %d networks", result);
    netLink.endScan();
  }

  void wifiScanDraw()
  {
    display.setTextColor(TFT_WHITE);
    display.setTextDatum(TL_DATUM);

    if (gWiFiScanState == WiFiScanState::Scanning)
    {
      display.setTextSize(U8G2_TEXT_TITLE);
      display.setTextDatum(TC_DATUM);
      display.drawString("Scanning...", display.width() / 2, 24);
      return;
    }

    if (gWiFiScanState == WiFiScanState::Failed)
    {
      display.setTextSize(U8G2_TEXT_TITLE);
      display.setTextDatum(TC_DATUM);
      display.drawString("No networks", display.width() / 2, 16);
      display.setTextSize(U8G2_TEXT_MONO10);
      display.drawString("Click to rescan", display.width() / 2, 44);
      return;
    }

    display.setTextSize(U8G2_TEXT_SMALL);

    for (int row = 0; row < kWiFiScanRows; row++)
    {
      const int index = gWiFiScanTop + row;
      if (index >= gWiFiScanCount)
        break;

      const int y = TopBar::kHeight + row * kWiFiScanRowHeight;
      const bool active = index == gWiFiScanActive;

      if (active)
      {
        display.fillRoundRect(0, y, display.width() - 4, kWiFiScanRowHeight, 2, TFT_WHITE);
        display.setTextColor(TFT_BLACK, TFT_WHITE);
      }
      else
      {
        display.setTextColor(TFT_WHITE, TFT_BLACK);
      }

      String ssid = WiFi.SSID(index);
      if (ssid.isEmpty())
        ssid = "<hidden>";

      display.setTextDatum(TL_DATUM);
      display.drawString(ssid, 2, y + 2);

      display.setTextDatum(TR_DATUM);
      display.drawString(String(WiFi.RSSI(index)), display.width() - 6, y + 2);
    }
  }

  void wifiScanUpdate()
  {
    wifiScanPoll();

    if (ClickButtonEnc.clicks == -1)
    {
      screenManager.back();
      return;
    }

    if (gWiFiScanState == WiFiScanState::Failed && ClickButtonEnc.clicks == 1)
    {
      wifiScanStart();
      return;
    }

    if (gWiFiScanState != WiFiScanState::Results)
      return;

    const int64_t delta = encoderGetCount();
    if (delta != 0)
    {
      encoderClearCount();
      gWiFiScanActive = constrain(gWiFiScanActive + (delta > 0 ? 1 : -1), 0, gWiFiScanCount - 1);

      if (gWiFiScanActive < gWiFiScanTop)
        gWiFiScanTop = gWiFiScanActive;
      else if (gWiFiScanActive >= gWiFiScanTop + kWiFiScanRows)
        gWiFiScanTop = gWiFiScanActive - kWiFiScanRows + 1;

      haptic.playEffect(4);
    }

    if (ClickButtonEnc.clicks == 1)
    {
      WiFiProvision::pendingSsid = WiFi.SSID(gWiFiScanActive);
      WiFiProvision::pendingHidden = WiFiProvision::pendingSsid.isEmpty();
      haptic.playEffect(40);
      screenManager.setScreen(&WiFiPasswordScreen2);
    }
  }

  void wifiScanOnEnter()
  {
    wifiScanStart();
  }

  void wifiScanOnExit()
  {
    if (gWiFiScanState == WiFiScanState::Scanning)
      netLink.endScan();
    WiFi.scanDelete();
    gWiFiScanState = WiFiScanState::Idle;
  }
}

const Screen2 WiFiScanScreen2 = {
    .name = "Scan",
    .draw = wifiScanDraw,
    .update = wifiScanUpdate,
    .onEnter = wifiScanOnEnter,
    .onExit = wifiScanOnExit,
};
