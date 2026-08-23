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
  // Rotary character entry: turn to pick a character, click to append.
  constexpr char kWiFiPasswordCharset[] =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      "!@#$%^&*()-_=+[]{};:,.<>/?~ ";
  constexpr int kWiFiPasswordCharsetLen = sizeof(kWiFiPasswordCharset) - 1;
  constexpr int kWiFiPasswordMaxLen = 63;
  /** How many characters of the ribbon sit either side of the selected one. */
  constexpr int kWiFiPasswordRibbonSpan = 5;

  String gWiFiPassword;
  int gWiFiPasswordCharIndex = 0;

  void wifiPasswordDraw()
  {
    display.setTextColor(TFT_WHITE);

    display.setTextSize(U8G2_TEXT_SMALL);
    display.setTextDatum(TL_DATUM);
    display.drawString(WiFiProvision::pendingSsid, 0, TopBar::kHeight);

    display.setTextSize(U8G2_TEXT_MONO12);
    String shown = gWiFiPassword;
    if (shown.length() > 20)
      shown = shown.substring(shown.length() - 20);
    display.drawString(shown + "_", 0, TopBar::kHeight + 12);

    // Character ribbon, selected character boxed in the middle.
    const int centreX = display.width() / 2;
    const int ribbonY = 40;
    const int cellWidth = 10;

    display.setTextSize(U8G2_TEXT_MONO12);
    display.setTextDatum(TC_DATUM);

    for (int offset = -kWiFiPasswordRibbonSpan; offset <= kWiFiPasswordRibbonSpan; offset++)
    {
      int index = (gWiFiPasswordCharIndex + offset) % kWiFiPasswordCharsetLen;
      if (index < 0)
        index += kWiFiPasswordCharsetLen;

      const int x = centreX + offset * cellWidth;
      const char c[2] = {kWiFiPasswordCharset[index], '\0'};

      if (offset == 0)
      {
        display.fillRect(x - cellWidth / 2, ribbonY - 2, cellWidth, 14, TFT_WHITE);
        display.setTextColor(TFT_BLACK, TFT_WHITE);
      }
      else
      {
        display.setTextColor(TFT_WHITE, TFT_BLACK);
      }

      display.drawString(c, x, ribbonY);
    }

    display.setTextColor(TFT_WHITE);
    display.setTextSize(U8G2_TEXT_SMALL);
    display.setTextDatum(TC_DATUM);
    display.drawString("1 add  2 del  3 save", centreX, 56);
  }

  void wifiPasswordSubmit()
  {
    debugI("Saving credentials for %s", WiFiProvision::pendingSsid.c_str());

    if (netLink.saveCredentials(WiFiProvision::pendingSsid.c_str(),
                                gWiFiPassword.c_str(),
                                WiFiProvision::pendingHidden))
    {
      netLink.connect();
    }
    else
    {
      debugE("Could not save credentials");
    }

    gWiFiPassword = "";
    screenManager.setScreen(&WiFiInfoScreen2);
  }

  void wifiPasswordUpdate()
  {
    const int64_t delta = encoderGetCount();
    if (delta != 0)
    {
      encoderClearCount();
      gWiFiPasswordCharIndex += (delta > 0 ? 1 : -1);
      if (gWiFiPasswordCharIndex < 0)
        gWiFiPasswordCharIndex += kWiFiPasswordCharsetLen;
      gWiFiPasswordCharIndex %= kWiFiPasswordCharsetLen;
      haptic.playEffect(4);
    }

    switch (ClickButtonEnc.clicks)
    {
    case 1:
      if (gWiFiPassword.length() < kWiFiPasswordMaxLen)
      {
        gWiFiPassword += kWiFiPasswordCharset[gWiFiPasswordCharIndex];
        haptic.playEffect(40);
      }
      break;

    case 2:
      if (gWiFiPassword.length() > 0)
      {
        gWiFiPassword.remove(gWiFiPassword.length() - 1);
        haptic.playEffect(40);
      }
      break;

    case 3:
      haptic.playEffect(40);
      wifiPasswordSubmit();
      break;

    case -1:
      gWiFiPassword = "";
      screenManager.back();
      break;

    default:
      break;
    }
  }

  void wifiPasswordOnEnter()
  {
    gWiFiPassword = "";
    gWiFiPasswordCharIndex = 0;
    encoderClearCount();
  }
}

const Screen2 WiFiPasswordScreen2 = {
    .name = "Password",
    .draw = wifiPasswordDraw,
    .update = wifiPasswordUpdate,
    .onEnter = wifiPasswordOnEnter,
    .onExit = nullptr,
};
