#include "Engine.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#endif

namespace EngineScreenNamespace
{

  namespace
  {
    static bool testMode = true;
    static unsigned long lastUpdate = 0;

    static float speedKph = 0.0f;
    static float rpm = 0.0f;
    static float coolantC = 70.0f;

    static constexpr float speedMin = 0.0f, speedMax = 240.0f;
    static constexpr float rpmMin = 0.0f, rpmMax = 8000.0f;
    static constexpr float coolMin = 40.0f, coolMax = 120.0f;

    static constexpr int gaugeStart = 45;
    static constexpr int gaugeEnd = 315;

    static void drawGauge(int cx, int cy, int rOuter, int rInner, int startAngle, int endAngle,
                          float value, float vMin, float vMax, uint32_t color, const char *label, const char *unit)
    {
      display.drawSmoothArc(cx, cy, rOuter, rInner, startAngle, endAngle, TFT_DARKGREY, TFT_BLACK, false);

      float t = constrain((value - vMin) / (vMax - vMin), 0.0f, 1.0f);
      int sweepCW = (endAngle + 360 - startAngle) % 360;
      int valueAngle = (startAngle + (int)(t * sweepCW)) % 360;
      display.drawSmoothArc(cx, cy, rOuter, rInner, startAngle, valueAngle, color, TFT_BLACK, true);

      float theta = valueAngle * DEG_TO_RAD;
      float nx = -sinf(theta);
      float ny = cosf(theta);
      int needleLen = rOuter - 4;
      int nx0 = cx + (int)(nx * (rInner + 2));
      int ny0 = cy + (int)(ny * (rInner + 2));
      int nx1 = cx + (int)(nx * needleLen);
      int ny1 = cy + (int)(ny * needleLen);
      display.drawLine(nx0, ny0, nx1, ny1, TFT_WHITE);

      display.setTextDatum(MC_DATUM);
      display.setTextSize(2);
      display.setTextColor(TFT_WHITE);
      display.drawString(label, cx, cy + rOuter + 12);

      display.setTextSize(3);
      char buf[16];
      if (unit && unit[0] != '\0')
        snprintf(buf, sizeof(buf), "%.0f %s", value, unit);
      else
        snprintf(buf, sizeof(buf), "%.0f", value);
      display.drawString(buf, cx, cy - 6);
    }

    static void drawRpmGauge(int cx, int cy, int rOuter, int rInner)
    {
      const float redlineStartFrac = 0.80f;
      int sweepCW = (gaugeEnd + 360 - gaugeStart) % 360;
      int redStartAngle = (gaugeStart + (int)(sweepCW * redlineStartFrac)) % 360;

      display.drawSmoothArc(cx, cy, rOuter, rInner, gaugeStart, gaugeEnd, TFT_DARKGREY, TFT_BLACK, false);
      display.drawSmoothArc(cx, cy, rOuter, rInner, redStartAngle, gaugeEnd, TFT_RED, TFT_BLACK, false);

      float t = constrain((rpm - rpmMin) / (rpmMax - rpmMin), 0.0f, 1.0f);
      int valueAngle = (gaugeStart + (int)(t * sweepCW)) % 360;
      uint32_t valueColor = (t >= redlineStartFrac) ? TFT_RED : TFT_CYAN;
      display.drawSmoothArc(cx, cy, rOuter, rInner, gaugeStart, valueAngle, valueColor, TFT_BLACK, true);

      float theta = valueAngle * DEG_TO_RAD;
      float nx = -sinf(theta);
      float ny = cosf(theta);
      int needleLen = rOuter - 4;
      int nx0 = cx + (int)(nx * (rInner + 2));
      int ny0 = cy + (int)(ny * (rInner + 2));
      int nx1 = cx + (int)(nx * needleLen);
      int ny1 = cy + (int)(ny * needleLen);
      display.drawLine(nx0, ny0, nx1, ny1, TFT_WHITE);

      display.setTextDatum(MC_DATUM);
      display.setTextSize(2);
      display.setTextColor(TFT_WHITE);
      display.drawString("RPM", cx, cy + rOuter + 12);

      display.setTextSize(3);
      char buf[16];
      snprintf(buf, sizeof(buf), "%.0f", rpm);
      display.drawString(buf, cx, cy - 6);
    }
  }

  void draw()
  {
    const int centerY = 120;
    drawRpmGauge(120, centerY, 70, 50);
    drawGauge(360, centerY, 70, 50, gaugeStart, gaugeEnd, speedKph, speedMin, speedMax, TFT_CYAN, "SPEED", "km/h");

    drawGauge(LCD_WIDTH / 2, 170, 50, 36, gaugeStart, gaugeEnd, coolantC, coolMin, coolMax, TFT_SKYBLUE, "COOLANT", "C");

    if (testMode)
    {
      display.setTextDatum(TR_DATUM);
      display.setTextSize(2);
      display.setTextColor(TFT_YELLOW);
      display.drawString("TEST", LCD_WIDTH - 6, 6);
    }
  }

  void update()
  {
    if (ClickButtonDOWN.clicks != 0)
      screenManager.back();

    if (ClickButtonUP.clicks == 1)
      testMode = !testMode;

    if (testMode)
    {
      unsigned long now = millis();
      float t1 = (now % 4000UL) / 4000.0f;
      float t2 = (now % 6000UL) / 6000.0f;

      speedKph = speedMin + t1 * (speedMax - speedMin);
      rpm = rpmMin + t1 * (rpmMax - rpmMin);
      coolantC = coolMin + t2 * (coolMax - coolMin);
    }
  }

} // namespace EngineScreenNamespace

const Screen2 EngineScreen = {
    .name = "Engine",
    .draw = EngineScreenNamespace::draw,
    .update = EngineScreenNamespace::update,
    .onEnter = nullptr,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_WHITE,
};
