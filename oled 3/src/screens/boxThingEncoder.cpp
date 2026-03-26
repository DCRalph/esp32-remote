#include "boxThingEncoder.h"

#include "config.h"
#include "Display.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"
#include "Wireless.h"

namespace BoxThingEncoderScreenNamespace
{

  namespace
  {
    static uint64_t encCount = 0;
    static int8_t clicks = 0;
    static int8_t lastClicks = 0;
  }

  void onRecvPacket(const TransportPacket &pkt)
  {
    memcpy(&encCount, pkt.data, 8);
    memcpy(&clicks, pkt.data + 8, 1);
    if (clicks != 0)
    {
      lastClicks = clicks;
    }
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

    sprintf(buf, "%d", encCount);
    display.drawString(buf, LCD_WIDTH / 2, 140);
  }

  void update()
  {
    if (ClickButtonUP.clicks != 0 || ClickButtonDOWN.clicks != 0)
    {
      screenManager.back();
    }
  }

  void onEnter()
  {
    wireless.addOnReceiveFor(0xa3, [](WirelessFrame *frame)
                               {
                                 if (frame)
                                   BoxThingEncoderScreenNamespace::onRecvPacket(frame->packet);
                               });
  }

  void onExit()
  {
    wireless.removeOnReceiveFor(0xa3);
  }

} // namespace BoxThingEncoderScreenNamespace

const Screen2 BoxThingEncoderScreen = {
    .name = "BoxThingEncoder",
    .draw = BoxThingEncoderScreenNamespace::draw,
    .update = BoxThingEncoderScreenNamespace::update,
    .onEnter = BoxThingEncoderScreenNamespace::onEnter,
    .onExit = BoxThingEncoderScreenNamespace::onExit,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
