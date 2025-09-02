#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/Buttons.h"

class EngineScreen : public Screen
{
public:
  EngineScreen(String _name) : Screen(_name) {}

  // State
  bool testMode = true;
  unsigned long lastUpdate = 0;

  // Values
  float speedKph = 0.0f;  // 0..240
  float rpm = 0.0f;       // 0..8000
  float coolantC = 70.0f; // 40..120

  // Ranges
  const float speedMin = 0.0f, speedMax = 240.0f;
  const float rpmMin = 0.0f, rpmMax = 8000.0f;
  const float coolMin = 40.0f, coolMax = 120.0f;

  // Gauge geometry and sweep rotated -90°: 45° -> 315° clockwise (270° sweep)
  const int gaugeStart = 45;
  const int gaugeEnd = 315;

  void drawGauge(int cx, int cy, int rOuter, int rInner, int startAngle, int endAngle,
                 float value, float vMin, float vMax, uint32_t color, const char *label, const char *unit)
  {
    // Background arc
    display.sprite.drawSmoothArc(cx, cy, rOuter, rInner, startAngle, endAngle, TFT_DARKGREY, TFT_BLACK, false);

    // Value arc
    float t = constrain((value - vMin) / (vMax - vMin), 0.0f, 1.0f);
    int sweepCW = (endAngle + 360 - startAngle) % 360; // clockwise sweep length
    int valueAngle = (startAngle + (int)(t * sweepCW)) % 360;
    display.sprite.drawSmoothArc(cx, cy, rOuter, rInner, startAngle, valueAngle, color, TFT_BLACK, true);

    // Needle
    float theta = valueAngle * DEG_TO_RAD;
    // Convert TFT_eSPI angle to Cartesian: 0 at 6 o'clock, clockwise positive
    float nx = -sinf(theta);
    float ny = cosf(theta);
    int needleLen = rOuter - 4;
    int nx0 = cx + (int)(nx * (rInner + 2));
    int ny0 = cy + (int)(ny * (rInner + 2));
    int nx1 = cx + (int)(nx * needleLen);
    int ny1 = cy + (int)(ny * needleLen);
    display.sprite.drawLine(nx0, ny0, nx1, ny1, TFT_WHITE);

    // Label and value
    display.sprite.setTextDatum(MC_DATUM);
    display.sprite.setTextSize(2);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString(label, cx, cy + rOuter + 12);

    display.sprite.setTextSize(3);
    char buf[16];
    if (unit && unit[0] != '\0')
      snprintf(buf, sizeof(buf), "%.0f %s", value, unit);
    else
      snprintf(buf, sizeof(buf), "%.0f", value);
    display.sprite.drawString(buf, cx, cy - 6);
  }

  void drawRpmGauge(int cx, int cy, int rOuter, int rInner)
  {
    // Draw background with redline zone overlay
    const float redlineStartFrac = 0.80f; // top 20%
    int sweepCW = (gaugeEnd + 360 - gaugeStart) % 360;
    int redStartAngle = (gaugeStart + (int)(sweepCW * redlineStartFrac)) % 360;

    // Full arc base
    display.sprite.drawSmoothArc(cx, cy, rOuter, rInner, gaugeStart, gaugeEnd, TFT_DARKGREY, TFT_BLACK, false);
    // Redline background zone
    display.sprite.drawSmoothArc(cx, cy, rOuter, rInner, redStartAngle, gaugeEnd, TFT_RED, TFT_BLACK, false);

    // Value arc with dynamic color: normal cyan, red if in redline fraction
    float t = constrain((rpm - rpmMin) / (rpmMax - rpmMin), 0.0f, 1.0f);
    int valueAngle = (gaugeStart + (int)(t * sweepCW)) % 360;
    uint32_t valueColor = (t >= redlineStartFrac) ? TFT_RED : TFT_CYAN;
    display.sprite.drawSmoothArc(cx, cy, rOuter, rInner, gaugeStart, valueAngle, valueColor, TFT_BLACK, true);

    // Needle
    float theta = valueAngle * DEG_TO_RAD;
    float nx = -sinf(theta);
    float ny = cosf(theta);
    int needleLen = rOuter - 4;
    int nx0 = cx + (int)(nx * (rInner + 2));
    int ny0 = cy + (int)(ny * (rInner + 2));
    int nx1 = cx + (int)(nx * needleLen);
    int ny1 = cy + (int)(ny * needleLen);
    display.sprite.drawLine(nx0, ny0, nx1, ny1, TFT_WHITE);

    // Label and value
    display.sprite.setTextDatum(MC_DATUM);
    display.sprite.setTextSize(2);
    display.sprite.setTextColor(TFT_WHITE);
    display.sprite.drawString("RPM", cx, cy + rOuter + 12);

    display.sprite.setTextSize(3);
    char buf[16];
    snprintf(buf, sizeof(buf), "%.0f", rpm);
    display.sprite.drawString(buf, cx, cy - 6);
  }

  void draw() override
  {
    // Layout
    const int centerY = 120;
    // RPM (left), Speed (right), Coolant (bottom center)
    drawRpmGauge(120, centerY, 70, 50);
    drawGauge(360, centerY, 70, 50, gaugeStart, gaugeEnd, speedKph, speedMin, speedMax, TFT_CYAN, "SPEED", "km/h");

    // Coolant: smaller gauge, moved up so it isn't cut off
    drawGauge(LCD_WIDTH / 2, 170, 50, 36, gaugeStart, gaugeEnd, coolantC, coolMin, coolMax, TFT_SKYBLUE, "COOLANT", "C");

    // Test mode indicator
    if (testMode)
    {
      display.sprite.setTextDatum(TR_DATUM);
      display.sprite.setTextSize(2);
      display.sprite.setTextColor(TFT_YELLOW);
      display.sprite.drawString("TEST", LCD_WIDTH - 6, 6);
    }
  }

  void update() override
  {
    if (ClickButtonDOWN.clicks != 0)
      screenManager.back();

    if (ClickButtonUP.clicks == 1)
      testMode = !testMode;

    if (testMode)
    {
      // Sweep each value across its range smoothly
      unsigned long now = millis();
      // Speed and rpm sweep in 4s cycles, coolant in 6s
      float t1 = (now % 4000UL) / 4000.0f; // 0..1
      float t2 = (now % 6000UL) / 6000.0f; // 0..1

      speedKph = speedMin + t1 * (speedMax - speedMin);
      rpm = rpmMin + t1 * (rpmMax - rpmMin);
      coolantC = coolMin + t2 * (coolMax - coolMin);
    }
  }

  void onEnter() override
  {
    topBarColor = TFT_BLUE;
    topBarTextColor = TFT_WHITE;
  }
};
