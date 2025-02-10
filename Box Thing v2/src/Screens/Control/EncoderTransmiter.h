#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
// #include "IO/myespnow.h"
#include "IO/Wireless.h"

class EncoderTransmiterScreen : public Screen
{
public:
  EncoderTransmiterScreen(String _name);

  // 84:fc:e6:64:9b:f0
  uint8_t peer_addr[6] = {0x84, 0xfc, 0xe6, 0x64, 0x9b, 0xf0};

  uint64_t lastSend = 0;
  int8_t nextClicks = 0;

  void draw() override;
  void update() override;
  void onEnter() override;
  void onExit() override;
};

EncoderTransmiterScreen::EncoderTransmiterScreen(String _name) : Screen(_name)
{
}

void EncoderTransmiterScreen::draw()
{
  display.u8g2.setFont(u8g2_font_profont10_tf);
  display.u8g2.drawStr(0, 30, "Encoder Transmiter");
  // press encoder 2 times and hold to exit
  display.u8g2.drawStr(0, 50, "Press encoder 2 times");
  display.u8g2.drawStr(0, 60, "and hold to exit");
}

void EncoderTransmiterScreen::update()
{

  if (ClickButtonEnc.clicks == -2)
  {
    screenManager.back();
  }

  if (ClickButtonEnc.clicks != 0)
  {
    nextClicks = ClickButtonEnc.clicks;
  }

  if (millis() - lastSend > 10)
  {
    lastSend = millis();

    uint64_t encCount = encoderGetCount();
    uint8_t encCountArr[8];
    memcpy(encCountArr, &encCount, 8);

    fullPacket fp;
    memcpy(fp.mac, peer_addr, 6);
    fp.direction = PacketDirection::SEND;
    fp.p.type = 0xa3;
    fp.p.len = 9;

    memcpy(fp.p.data, encCountArr, 8);
    fp.p.data[8] = nextClicks;

    wireless.send(&fp);
    nextClicks = 0;
  }
}

void EncoderTransmiterScreen::onEnter()
{
  encoderClearCount();
}

void EncoderTransmiterScreen::onExit()
{
}
