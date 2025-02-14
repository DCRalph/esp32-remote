#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"
#include "IO/Menu.h"

static uint8_t car_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

enum class ApplicationMode
{
  NORMAL,
  TEST,
  REMOTE,
  OFF
};

class CarControlScreen : public Screen
{
public:
  CarControlScreen(String _name);

  Menu menu = Menu();

  MenuItemBack backItem;

  uint64_t lastPing = 0;
  uint64_t lastConfirmedPing = 0;
  bool connected = false;
  bool lastConnected = false;

  bool brakeEffectActive = false;
  bool leftIndicatorEffectActive = false;
  bool rightIndicatorEffectActive = false;
  bool reverseLightEffectActive = false;
  bool startupEffectActive = false;
  bool rgbEffectActive = false;
  bool nightriderEffectActive = false;

  ApplicationMode mode = ApplicationMode::NORMAL;

  MenuItemToggle connectionItem = MenuItemToggle("Conn", &connected, false);

  MenuItem modeItem = MenuItem("Mode");

  // MenuItemAction modeNormalItem = MenuItemAction("Normal", 1, [&]()
  //                                               {
  //                                                 fullPacket fp;
  //                                                 fp.direction = PacketDirection::SEND;
  //                                                 memcpy(fp.mac, car_addr, 6);
  //                                                 fp.p.type = 0xe1;
  //                                                 fp.p.len = 1;
  //                                                 fp.p.data[0] = static_cast<uint8_t>(ApplicationMode::NORMAL);
  //                                                 wireless.send(&fp);
  //                                               });

  // MenuItemAction modeRemoteItem = MenuItemAction("Remote", 1, [&]()
  //                                               {
  //                                                 fullPacket fp;
  //                                                 fp.direction = PacketDirection::SEND;
  //                                                 memcpy(fp.mac, car_addr, 6);
  //                                                 fp.p.type = 0xe1;
  //                                                 fp.p.len = 1;
  //                                                 fp.p.data[0] = static_cast<uint8_t>(ApplicationMode::REMOTE);
  //                                                 wireless.send(&fp);
  //                                               });

  std::vector<String> modeItems = {"Red", "Green", "Blue", "Orange"};
  MenuItemSelect modeSelectItem = MenuItemSelect("Mode", modeItems, 0);

  MenuItemToggle brakeEffectItem = MenuItemToggle("Brake", &brakeEffectActive, true);
  MenuItemToggle leftIndicatorEffectItem = MenuItemToggle("Left", &leftIndicatorEffectActive, true);
  MenuItemToggle rightIndicatorEffectItem = MenuItemToggle("Right", &rightIndicatorEffectActive, true);
  MenuItemToggle reverseLightEffectItem = MenuItemToggle("Rev", &reverseLightEffectActive, true);
  MenuItemToggle startupEffectItem = MenuItemToggle("Start", &startupEffectActive, true);
  MenuItemToggle rgbEffectItem = MenuItemToggle("RGB", &rgbEffectActive, true);
  MenuItemToggle nightriderEffectItem = MenuItemToggle("Nrider", &nightriderEffectActive, true);

  void draw() override;
  void update() override;
  void onEnter() override;

  void sentEffects();
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&connectionItem);
  menu.addMenuItem(&modeItem);

  menu.addMenuItem(&modeSelectItem);

  // menu.addMenuItem(&modeNormalItem);
  // menu.addMenuItem(&modeRemoteItem);

  menu.addMenuItem(&brakeEffectItem);
  menu.addMenuItem(&leftIndicatorEffectItem);
  menu.addMenuItem(&rightIndicatorEffectItem);
  menu.addMenuItem(&reverseLightEffectItem);
  menu.addMenuItem(&startupEffectItem);
  menu.addMenuItem(&rgbEffectItem);
  menu.addMenuItem(&nightriderEffectItem);

  wireless.addOnReceiveFor(0xe0, [&](fullPacket *fp) // ping cmd
                           {
                             lastConfirmedPing = millis();

                             mode = static_cast<ApplicationMode>(fp->p.data[0]);

                             brakeEffectActive = fp->p.data[1];
                             leftIndicatorEffectActive = fp->p.data[2];
                             rightIndicatorEffectActive = fp->p.data[3];
                             reverseLightEffectActive = fp->p.data[4];
                             startupEffectActive = fp->p.data[5];
                             rgbEffectActive = fp->p.data[6];
                             nightriderEffectActive = fp->p.data[7]; });

  wireless.addOnReceiveFor(0xe1, [&](fullPacket *fp) // set mode
                           {
                             lastConfirmedPing = millis();

                             mode = static_cast<ApplicationMode>(fp->p.data[0]); //
                           });

  wireless.addOnReceiveFor(0xe2, [&](fullPacket *fp) // set effects
                           {
                             lastConfirmedPing = millis();

                             brakeEffectActive = fp->p.data[0];
                             leftIndicatorEffectActive = fp->p.data[1];
                             rightIndicatorEffectActive = fp->p.data[2];
                             reverseLightEffectActive = fp->p.data[3];
                             startupEffectActive = fp->p.data[4];
                             rgbEffectActive = fp->p.data[5];
                             nightriderEffectActive = fp->p.data[6];
                             //
                           });

  modeSelectItem.setOnChange([&]()
                             {
                               fullPacket fp;
                               fp.direction = PacketDirection::SEND;
                               memcpy(fp.mac, car_addr, 6);
                               fp.p.type = 0xe1;
                               fp.p.len = 1;
                               fp.p.data[0] = static_cast<uint8_t>(modeSelectItem.getCurrentIndex());
                               wireless.send(&fp);
                               //
                             });

  brakeEffectItem.setOnChange([&]()
                              { sentEffects(); });

  leftIndicatorEffectItem.setOnChange([&]()
                                      { sentEffects(); });

  rightIndicatorEffectItem.setOnChange([&]()
                                       { sentEffects(); });

  reverseLightEffectItem.setOnChange([&]()
                                     { sentEffects(); });

  startupEffectItem.setOnChange([&]()
                                { sentEffects(); });

  rgbEffectItem.setOnChange([&]()
                            { sentEffects(); });

  nightriderEffectItem.setOnChange([&]()
                                   { sentEffects(); });
}

void CarControlScreen::draw()
{
  menu.draw();
}

void CarControlScreen::update()
{
  menu.update();

  connected = millis() - lastConfirmedPing < 1000;

  if (connected != lastConnected)
  {
    lastConnected = connected;

    if (!connected)
    {
      mode = ApplicationMode::OFF;
    }
  }

  if (millis() - lastPing > 200)
  {
    lastPing = millis();

    fullPacket fp;
    fp.direction = PacketDirection::SEND;
    memcpy(fp.mac, car_addr, 6);
    fp.p.type = 0xe0;
    fp.p.len = 0;

    wireless.send(&fp);
  }
}

void CarControlScreen::onEnter()
{
}

void CarControlScreen::sentEffects()
{
  fullPacket fp;
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, car_addr, 6);
  fp.p.type = 0xe2;
  fp.p.len = 7;

  fp.p.data[0] = brakeEffectActive;
  fp.p.data[1] = leftIndicatorEffectActive;
  fp.p.data[2] = rightIndicatorEffectActive;
  fp.p.data[3] = reverseLightEffectActive;
  fp.p.data[4] = startupEffectActive;
  fp.p.data[5] = rgbEffectActive;
  fp.p.data[6] = nightriderEffectActive;

  wireless.send(&fp);
}