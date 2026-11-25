#include "Screens/UpdateProgress.h"

#include <Display.h>

#include "IO/BoxThingDisplay.h"

namespace
{
  enum class State
  {
    GettingReady,
    Updating,
    ShowingMessage
  };

  State gState = State::GettingReady;
  uint8_t gProgress = 0;
  String gMessage;

  void draw()
  {
    display.noTopBar();
    display.setTextColor(Color::White);
    display.setTextDatum(Datum::TopCenter);

    const int centreX = display.width() / 2;

    display.setTextSize(U8G2_TEXT_TITLE_XL);
    display.drawString("Updating", centreX, 0);

    switch (gState)
    {
    case State::GettingReady:
      display.setTextSize(U8G2_TEXT_TITLE);
      display.drawString("Getting ready...", centreX, 48);
      break;

    case State::Updating:
      display.setTextSize(U8G2_TEXT_BAR);
      display.drawString(String(gProgress) + "%", centreX, 36);

      display.drawRect(0, 48, 127, 16, Color::White);
      display.fillRect(2, 50, map(gProgress, 0, 100, 0, 123), 12, Color::White);
      break;

    case State::ShowingMessage:
      display.setTextSize(U8G2_TEXT_TITLE);
      display.drawString(gMessage, centreX, 34);
      break;
    }
  }

  void update()
  {
  }
}

void UpdateProgress::setProgress(uint8_t progress)
{
  gState = State::Updating;
  gProgress = progress;
}

void UpdateProgress::setMessage(String message)
{
  gState = State::ShowingMessage;
  gMessage = message;
}

const Screen2 UpdateProgressScreen2 = {
    .name = "Update",
    .draw = draw,
    .update = update,
    .onEnter = nullptr,
    .onExit = nullptr,
};
