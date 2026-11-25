#include "boxThingEncoder.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"

namespace BoxThingEncoderScreenNamespace
{

  namespace
  {
    static int64_t encCount = 0;
    static int8_t lastClicks = 0;
  }

  void draw()
  {
    display.noTopBar();
    display.setTextSize(4);
    display.setTextDatum(TC_DATUM);
    display.setTextColor(TFT_WHITE);
    display.drawString("Encoder", LCD_WIDTH / 2, 60);

    char buf[20];
    sprintf(buf, "%d", lastClicks);
    display.drawString(buf, LCD_WIDTH / 2, 100);

    sprintf(buf, "%lld", static_cast<long long>(encCount));
    display.drawString(buf, LCD_WIDTH / 2, 140);
  }

  void update()
  {
    if (ClickButtonUP.clicks != 0 || ClickButtonDOWN.clicks != 0)
    {
      screenManager.back();
    }
  }

  // DirectChannel has no unregister; the handler only stores values the screen shows.
  void onEnter()
  {
    RemoteProtocol::Encoder::state().onReceive([](const TransportAddress &, const RemoteProtocol::Encoder::State &s)
                                               {
                                                 encCount = s.detents;
                                                 if (s.clicks != 0)
                                                   lastClicks = s.clicks;
                                               });
  }

} // namespace BoxThingEncoderScreenNamespace

const Screen2 BoxThingEncoderScreen = {
    .name = "BoxThingEncoder",
    .draw = BoxThingEncoderScreenNamespace::draw,
    .update = BoxThingEncoderScreenNamespace::update,
    .onEnter = BoxThingEncoderScreenNamespace::onEnter,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
