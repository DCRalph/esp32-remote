#include "CompanionScreen.h"

#include "config.h"
#include "Display.h"
#include "IO/Buttons.h"
#include "ScreenManager.h"
#include "companion/CompanionHttp.h"
#include "companion/CompanionRadio.h"

namespace CompanionScreenNamespace
{

  namespace
  {
    constexpr int kContentTopY = 22;
    constexpr int kWaveformTopY = 58;
    constexpr int kMarginX = 8;
    constexpr int kBeatsPerMeasure = 4;

    enum class CompanionViewPage { Overview, Waveform };

    static CompanionViewPage viewPage = CompanionViewPage::Overview;

    static void truncateStr(char *dest, size_t destSize, const char *src, size_t maxLen)
    {
      if (!src || !dest || destSize == 0)
        return;
      size_t len = strlen(src);
      if (len <= maxLen)
      {
        strncpy(dest, src, destSize - 1);
        dest[destSize - 1] = '\0';
        return;
      }
      size_t copyLen = maxLen < destSize - 4 ? maxLen : destSize - 4;
      strncpy(dest, src, copyLen);
      dest[copyLen] = '\0';
      strcat(dest, "...");
    }

    static uint8_t hd2Height(uint16_t word)
    {
      const uint16_t heightMask = 0x007C;
      return (uint8_t)((word & heightMask) >> 2);
    }

    static void hd2Color(uint16_t word, uint8_t &r, uint8_t &g, uint8_t &b)
    {
      const uint16_t redMask = 0xE000;
      const uint16_t greenMask = 0x1C00;
      const uint16_t blueMask = 0x0380;
      auto extract = [](uint16_t val, uint16_t mask) -> uint8_t {
        uint16_t m = mask;
        uint8_t shift = 0;
        while ((m & 1) == 0)
        {
          m >>= 1;
          shift++;
        }
        return (uint8_t)(((val & mask) >> shift) * 255) / 7;
      };
      r = extract(word, redMask);
      g = extract(word, greenMask);
      b = extract(word, blueMask);
    }

    static void drawBeatIndicator(uint8_t beatInMeasure, uint16_t bar, int x, int y)
    {
      constexpr int kRectW = 22;
      constexpr int kRectH = 14;
      constexpr int kGap = 6;

      for (int i = 0; i < kBeatsPerMeasure; i++)
      {
        const int bx = x + i * (kRectW + kGap);
        const bool active = beatInMeasure > 0 && beatInMeasure == (uint8_t)(i + 1);
        const bool isDownbeat = i == 0;
        const uint16_t color = active ? (isDownbeat ? TFT_YELLOW : TFT_CYAN)
                                      : (isDownbeat ? TFT_ORANGE : TFT_DARKGREY);
        if (active)
          display.fillRect(bx, y, kRectW, kRectH, color);
        else
          display.drawRect(bx, y, kRectW, kRectH, color);
      }

      if (bar > 0)
      {
        char barBuf[12];
        sprintf(barBuf, "%u", (unsigned)bar);
        const int beatsW = kBeatsPerMeasure * kRectW + (kBeatsPerMeasure - 1) * kGap;
        display.setTextDatum(TL_DATUM);
        display.setTextSize(2);
        display.setTextColor(TFT_CYAN);
        display.drawString(barBuf, x + beatsW + 10, y - 1);
      }
    }

    static void drawPageHint()
    {
      display.setTextDatum(TR_DATUM);
      display.setTextSize(1);
      display.setTextColor(TFT_DARKGREY);
      const char *hint = viewPage == CompanionViewPage::Overview ? "Overview 1/2" : "Waveform 2/2";
      display.drawString(hint, LCD_WIDTH - kMarginX, kContentTopY);
    }

    static void drawPairingPanel()
    {
      drawPageHint();

      const CompanionPairState pairState = CompanionHttp::getPairState();
      const bool blinkOn = (millis() / 600) % 2 == 0;

      display.setTextDatum(TC_DATUM);

      display.setTextSize(2);
      if (pairState == CompanionPairState::WifiDown)
      {
        display.setTextColor(blinkOn ? TFT_ORANGE : TFT_DARKGREY);
        display.drawString("WiFi connecting...", LCD_WIDTH / 2, kContentTopY + 8);
      }
      else if (pairState == CompanionPairState::Discovering)
      {
        display.setTextColor(blinkOn ? TFT_CYAN : TFT_DARKCYAN);
        display.drawString("Finding host...", LCD_WIDTH / 2, kContentTopY + 8);
      }
      else
      {
        display.setTextColor(blinkOn ? TFT_YELLOW : TFT_ORANGE);
        display.drawString("Awaiting approval", LCD_WIDTH / 2, kContentTopY + 8);
      }

      display.setTextSize(1);
      display.setTextColor(TFT_WHITE);
      display.drawString(CompanionHttp::getDeviceName(), LCD_WIDTH / 2, kContentTopY + 30);

      display.setTextColor(TFT_CYAN);
      display.drawString(CompanionHttp::getMacString(), LCD_WIDTH / 2, kContentTopY + 44);

      display.setTextColor(TFT_DARKGREY);
      display.drawString("prolink-lights.local (HTTP)", LCD_WIDTH / 2, kContentTopY + 58);

      display.setTextColor(TFT_YELLOW);
      display.drawString("Approve in Prolink Lights", LCD_WIDTH / 2, kContentTopY + 72);
      display.drawString("External Control > Display", LCD_WIDTH / 2, kContentTopY + 84);

      display.setTextDatum(BL_DATUM);
      display.setTextSize(1);
      display.setTextColor(TFT_DARKGREY);
      display.drawString("UP: restart pairing  2x: back", kMarginX, LCD_HEIGHT - 4);
    }

    static void drawOverview()
    {
      if (CompanionHttp::getPairState() != CompanionPairState::Ready)
      {
        drawPairingPanel();
        return;
      }

      const CompanionDisplayState &state = CompanionHttp::protocol().display();

      drawPageHint();

      char buf[52];

      display.setTextDatum(TL_DATUM);
      display.setTextSize(3);
      display.setTextColor(TFT_WHITE);
      truncateStr(buf, sizeof(buf), state.pageName[0] ? state.pageName : "(no page)", 18);
      display.drawString(buf, kMarginX, kContentTopY + 4);

      int y = kContentTopY + 36;
      display.setTextSize(2);
      display.setTextColor(TFT_CYAN);

      if (state.profileName[0])
      {
        truncateStr(buf, sizeof(buf), state.profileName, 28);
        display.drawString(buf, kMarginX, y);
        y += 18;
      }

      if (state.pageCount > 0)
      {
        sprintf(buf, "Page %u / %u", (unsigned)(state.pageIndex + 1), (unsigned)state.pageCount);
        display.drawString(buf, kMarginX, y);
        y += 18;
      }

      sprintf(buf, "Bright %.0f%%  %s", state.brightness * 100.0f, state.blackout ? "BLACKOUT" : "Live");
      display.drawString(buf, kMarginX, y);
      y += 18;

      display.setTextColor(TFT_WHITE);
      if (state.activeScene[0])
      {
        char sceneBuf[40];
        truncateStr(sceneBuf, sizeof(sceneBuf), state.activeScene, 28);
        char line[48];
        snprintf(line, sizeof(line), "Scene: %s", sceneBuf);
        display.drawString(line, kMarginX, y);
        y += 18;
      }

      if (state.activeEffect[0])
      {
        truncateStr(buf, sizeof(buf), state.activeEffect, 28);
        char line[48];
        snprintf(line, sizeof(line), "Fx: %s", buf);
        display.drawString(line, kMarginX, y);
        y += 18;
      }

      display.setTextDatum(BL_DATUM);
      display.setTextSize(1);
      display.setTextColor(TFT_GREEN);
      sprintf(buf, "Linked  Ctrl:%s", state.controllerConnected ? "OK" : "--");
      display.drawString(buf, kMarginX, LCD_HEIGHT - 4);
    }

    static void drawWaveform()
    {
      if (CompanionHttp::getPairState() != CompanionPairState::Ready)
      {
        drawPairingPanel();
        return;
      }

      const CompanionDisplayState &state = CompanionHttp::protocol().display();
      const CompanionWaveformState &wf = CompanionHttp::protocol().waveform().state();

      drawPageHint();

      display.setTextDatum(TL_DATUM);
      display.setTextSize(3);
      display.setTextColor(TFT_WHITE);

      char buf[32];
      if (state.masterBpm > 0)
        sprintf(buf, "BPM %.1f", state.masterBpm);
      else
        strcpy(buf, "BPM --");
      display.drawString(buf, kMarginX, kContentTopY + 2);

      display.setTextDatum(TR_DATUM);
      display.setTextSize(2);
      display.setTextColor(state.blackout ? TFT_ORANGE : TFT_CYAN);
      if (state.blackout)
        strcpy(buf, "BLK");
      else
        sprintf(buf, "%.0f%%", state.brightness * 100.0f);
      display.drawString(buf, LCD_WIDTH - kMarginX, kContentTopY + 6);
      display.setTextDatum(TL_DATUM);

      if (state.beatInMeasure > 0 || state.bar > 0)
        drawBeatIndicator(state.beatInMeasure, state.bar, kMarginX, kContentTopY + 28);

      const int waveX = kMarginX;
      const int waveY = kWaveformTopY;
      const int waveW = LCD_WIDTH - kMarginX * 2;
      const int waveH = LCD_HEIGHT - waveY - 6;

      display.drawRect(waveX, waveY, waveW, waveH, TFT_DARKGREY);

      if (!wf.valid || !wf.data || wf.segmentCount == 0)
      {
        display.setTextDatum(MC_DATUM);
        display.setTextSize(2);
        display.setTextColor(TFT_DARKGREY);
        display.drawString("No waveform", waveX + waveW / 2, waveY + waveH / 2);
        return;
      }

      const int barStep = max(1, (int)waveW / (int)wf.segmentCount);
      const int maxBarH = waveH - 4;
      int x = waveX + 1;

      for (uint16_t i = 0; i < wf.segmentCount && x < waveX + waveW - 1; i++)
      {
        const uint16_t word = (uint16_t)((wf.data[i * 2] << 8) | wf.data[i * 2 + 1]);
        const uint8_t h = hd2Height(word);
        const int barH = map(h, 0, 31, 1, maxBarH);
        uint8_t r, g, b;
        hd2Color(word, r, g, b);
        const uint16_t color = display.color565(r, g, b);
        const int barW = max(1, barStep - 1);
        display.fillRect(x, waveY + waveH - barH, barW, barH, color);
        x += barStep;
      }

      if (state.waveformWindowEndMs > state.waveformWindowStartMs)
      {
        const float pct = (float)(state.playheadMs - state.waveformWindowStartMs) /
                          (float)(state.waveformWindowEndMs - state.waveformWindowStartMs);
        const int px = waveX + (int)(pct * (float)(waveW - 2));
        display.drawLine(px, waveY, px, waveY + waveH - 1, TFT_WHITE);
      }
    }

    static void nextPage()
    {
      viewPage = viewPage == CompanionViewPage::Overview ? CompanionViewPage::Waveform
                                                         : CompanionViewPage::Overview;
    }

    static void prevPage()
    {
      viewPage = viewPage == CompanionViewPage::Waveform ? CompanionViewPage::Overview
                                                         : CompanionViewPage::Waveform;
    }
  }

  void onEnter()
  {
    viewPage = CompanionViewPage::Overview;
    CompanionRadio::suspendMeshForCompanion();
    CompanionHttp::begin();
  }

  void onExit()
  {
    CompanionHttp::shutdown();
    CompanionRadio::resumeMeshAfterCompanion();
  }

  void draw()
  {
    if (viewPage == CompanionViewPage::Overview)
      drawOverview();
    else
      drawWaveform();
  }

  void update()
  {
    if (ClickButtonUP.clicks == 2 || ClickButtonDOWN.clicks == 2)
    {
      screenManager.back();
      return;
    }

    if (CompanionHttp::getPairState() != CompanionPairState::Ready)
    {
      if (ClickButtonUP.clicks == 1)
        CompanionHttp::restartPairing();
      return;
    }

    if (ClickButtonUP.clicks == 1) {
      CompanionHttp::sendPagePrev();
      prevPage();
    } else if (ClickButtonDOWN.clicks == 1) {
      CompanionHttp::sendPageNext();
      nextPage();
    }
  }

} // namespace CompanionScreenNamespace

const Screen2 CompanionScreen = {
    .name = "Companion",
    .draw = CompanionScreenNamespace::draw,
    .update = CompanionScreenNamespace::update,
    .onEnter = CompanionScreenNamespace::onEnter,
    .onExit = CompanionScreenNamespace::onExit,
    .topBarColor = TFT_PURPLE,
    .topBarTextColor = TFT_WHITE,
};
