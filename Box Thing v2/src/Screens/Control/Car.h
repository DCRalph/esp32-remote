#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"
#include "IO/Menu.h"

// 80:65:99:4b:3a:d0
static uint8_t car_addr[6] = {0x80, 0x65, 0x99, 0x4b, 0x3a, 0xd0}; // s2 car

// 30:30:f9:2a:05:20
// static uint8_t car_addr[6] = {0x30, 0x30, 0xF9, 0x2A, 0x05, 0x20}; // s3 dev

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

  bool leftIndicatorEffectActive = false;
  bool rightIndicatorEffectActive = false;
  bool startupEffectActive = false;
  bool rgbEffectActive = false;
  bool nightriderEffectActive = false;

  // bool highBeamEffectActive = false;
  // bool lowBeamEffectActive = false;
  bool headLightEffectActive = false;
  bool headLightSplit = false;
  bool headLightR = false;
  bool headLightG = false;
  bool headLightB = false;

  bool brakeEffectActive = false;
  bool reverseLightEffectActive = false;

  ApplicationMode mode = ApplicationMode::NORMAL;

  uint8_t lightMode = 0; // 0 unk, 1 headlights, 2 taillights

  MenuItemToggle connectionItem = MenuItemToggle("Conn", &connected, false);

  std::vector<String> modeItems = {"Norm", "Test", "Rem", "Off"};
  MenuItemSelect modeSelectItem = MenuItemSelect("Mode", modeItems, 0);

  MenuItem lightModeItem = MenuItem("Light Mode");

  MenuItemToggle leftIndicatorEffectItem = MenuItemToggle("Left", &leftIndicatorEffectActive, true);
  MenuItemToggle rightIndicatorEffectItem = MenuItemToggle("Right", &rightIndicatorEffectActive, true);
  MenuItemToggle startupEffectItem = MenuItemToggle("Start", &startupEffectActive, true);
  MenuItemToggle rgbEffectItem = MenuItemToggle("RGB", &rgbEffectActive, true);
  MenuItemToggle nightriderEffectItem = MenuItemToggle("Nrider", &nightriderEffectActive, true);

  // MenuItemToggle highBeamEffectItem = MenuItemToggle("High", &highBeamEffectActive, true);
  // MenuItemToggle lowBeamEffectItem = MenuItemToggle("Low", &lowBeamEffectActive, true);
  MenuItemToggle headLightEffectItem = MenuItemToggle("Headlight", &headLightEffectActive, true);
  MenuItemToggle headLightSplitItem = MenuItemToggle("Split", &headLightSplit, true);
  MenuItemToggle headLightRItem = MenuItemToggle("R", &headLightR, true);
  MenuItemToggle headLightGItem = MenuItemToggle("G", &headLightG, true);
  MenuItemToggle headLightBItem = MenuItemToggle("B", &headLightB, true);

  MenuItemToggle brakeEffectItem = MenuItemToggle("Brake", &brakeEffectActive, true);
  MenuItemToggle reverseLightEffectItem = MenuItemToggle("Rev", &reverseLightEffectActive, true);

  void draw() override;
  void update() override;
  void onEnter() override;

  void sentEffects();
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&connectionItem);
  // menu.addMenuItem(&modeItem);

  menu.addMenuItem(&modeSelectItem);
  menu.addMenuItem(&lightModeItem);

  menu.addMenuItem(&leftIndicatorEffectItem);
  menu.addMenuItem(&rightIndicatorEffectItem);
  menu.addMenuItem(&startupEffectItem);
  menu.addMenuItem(&rgbEffectItem);
  menu.addMenuItem(&nightriderEffectItem);

  // menu.addMenuItem(&highBeamEffectItem);
  // menu.addMenuItem(&lowBeamEffectItem);
  menu.addMenuItem(&headLightEffectItem);
  menu.addMenuItem(&headLightSplitItem);
  menu.addMenuItem(&headLightRItem);
  menu.addMenuItem(&headLightGItem);
  menu.addMenuItem(&headLightBItem);

  menu.addMenuItem(&brakeEffectItem);
  menu.addMenuItem(&reverseLightEffectItem);

  modeSelectItem.setOnChange([&]()
                             {
                               fullPacket fp;
                               memset(&fp, 0, sizeof(fullPacket));

                               fp.direction = PacketDirection::SEND;
                               memcpy(fp.mac, car_addr, 6);
                               fp.p.type = 0xe1;
                               fp.p.len = 1;
                               fp.p.data[0] = static_cast<uint8_t>(modeSelectItem.getCurrentIndex());
                               wireless.send(&fp);
                               //
                             });

  leftIndicatorEffectItem.setOnChange([&]()
                                      { sentEffects(); });

  rightIndicatorEffectItem.setOnChange([&]()
                                       { sentEffects(); });

  startupEffectItem.setOnChange([&]()
                                { sentEffects(); });

  rgbEffectItem.setOnChange([&]()
                            { sentEffects(); });

  nightriderEffectItem.setOnChange([&]()
                                   { sentEffects(); });

  // highBeamEffectItem.setOnChange([&]()
  //                                { sentEffects(); });

  // lowBeamEffectItem.setOnChange([&]()
  //                               { sentEffects(); });

  headLightEffectItem.setOnChange([&]()
                                  { sentEffects(); });

  headLightSplitItem.setOnChange([&]()
                                 { sentEffects(); });

  headLightRItem.setOnChange([&]()
                             { sentEffects(); });

  headLightGItem.setOnChange([&]()
                             { sentEffects(); });

  headLightBItem.setOnChange([&]()
                             { sentEffects(); });

  brakeEffectItem.setOnChange([&]()
                              { sentEffects(); });

  reverseLightEffectItem.setOnChange([&]()
                                     { sentEffects(); });
}

void CarControlScreen::draw()
{

  if (lightMode == 0)
  {
    lightModeItem.setName("Light Mode: UNK");
    // highBeamEffectItem.setHidden(true);
    // lowBeamEffectItem.setHidden(true);
    headLightEffectItem.setHidden(true);
    headLightSplitItem.setHidden(true);
    headLightRItem.setHidden(true);
    headLightGItem.setHidden(true);
    headLightBItem.setHidden(true);
    brakeEffectItem.setHidden(true);
    reverseLightEffectItem.setHidden(true);
  }
  else if (lightMode == 1)
  {
    lightModeItem.setName("Headlights");
    // highBeamEffectItem.setHidden(false);
    // lowBeamEffectItem.setHidden(false);
    headLightEffectItem.setHidden(false);
    headLightSplitItem.setHidden(false);
    headLightRItem.setHidden(false);
    headLightGItem.setHidden(false);
    headLightBItem.setHidden(false);
    brakeEffectItem.setHidden(true);
    reverseLightEffectItem.setHidden(true);
  }
  else if (lightMode == 2)
  {
    lightModeItem.setName("Taillights");
    // highBeamEffectItem.setHidden(true);
    // lowBeamEffectItem.setHidden(true);
    headLightEffectItem.setHidden(false);
    headLightSplitItem.setHidden(false);
    headLightRItem.setHidden(false);
    headLightGItem.setHidden(false);
    headLightBItem.setHidden(false);
    brakeEffectItem.setHidden(false);
    reverseLightEffectItem.setHidden(false);
  }

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
      modeSelectItem.setCurrentIndex((uint8_t)mode);
    }
  }

  if (millis() - lastPing > 500)
  {
    lastPing = millis();

    fullPacket fp;
    memset(&fp, 0, sizeof(fullPacket));
    fp.direction = PacketDirection::SEND;
    memcpy(fp.mac, car_addr, 6);
    fp.p.type = 0xe0;
    fp.p.len = 0;

    wireless.send(&fp);
  }
}

void CarControlScreen::onEnter()
{

  wireless.addOnReceiveFor(0xe0, [&](fullPacket *fp) // ping cmd
                           {
                             lastConfirmedPing = millis();

                             /*
                              * 0: mode
                              * 1: headlight/taillight mode
                              * 2.0: left indicator
                              * 3.0: right indicator
                              * 4.0: startup
                              * 5.0: rgb
                              * 6.0: nightrider
                              * 7.0: headlight / brake
                              * 7.1: split
                              * 7.2: red
                              * 7.3: green
                              * 7.4: blue
                              * 8.0: brake
                              * 8.1: reverse
                              */

                             mode = static_cast<ApplicationMode>(fp->p.data[0]);
                             modeSelectItem.setCurrentIndex((uint8_t)mode);

                             lightMode = fp->p.data[1];

                             leftIndicatorEffectActive = fp->p.data[2];
                             rightIndicatorEffectActive = fp->p.data[3];
                             startupEffectActive = fp->p.data[4];
                             rgbEffectActive = fp->p.data[5];
                             nightriderEffectActive = fp->p.data[6];

                             if (lightMode == 1)
                             {
                               //  highBeamEffectActive = fp->p.data[7];
                               //  lowBeamEffectActive = fp->p.data[8];

                               headLightEffectActive = checkBit(fp->p.data[7], 0);
                               headLightSplit = checkBit(fp->p.data[7], 1);
                               headLightR = checkBit(fp->p.data[7], 2);
                               headLightG = checkBit(fp->p.data[7], 3);
                               headLightB = checkBit(fp->p.data[7], 4);

                               brakeEffectActive = false;
                               reverseLightEffectActive = false;
                             }
                             else if (lightMode == 2)
                             {
                               //  highBeamEffectActive = false;
                               //  lowBeamEffectActive = false;

                               headLightEffectActive = false;
                               headLightSplit = false;
                               headLightR = false;
                               headLightG = false;
                               headLightB = false;

                               brakeEffectActive = fp->p.data[7];
                               reverseLightEffectActive = fp->p.data[8];
                             }
                             //
                           });

  wireless.addOnReceiveFor(0xe1, [&](fullPacket *fp) // set mode
                           {
                             lastConfirmedPing = millis();

                             mode = static_cast<ApplicationMode>(fp->p.data[0]);
                             modeSelectItem.setCurrentIndex((uint8_t)mode);
                             //
                           });

  connected = false;
  mode = ApplicationMode::OFF;
}

void CarControlScreen::sentEffects()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, car_addr, 6);
  fp.p.type = 0xe2;
  fp.p.len = 7;

  fp.p.data[0] = leftIndicatorEffectActive;
  fp.p.data[1] = rightIndicatorEffectActive;
  fp.p.data[2] = startupEffectActive;
  fp.p.data[3] = rgbEffectActive;
  fp.p.data[4] = nightriderEffectActive;

  if (lightMode == 1)
  {
    // fp.p.data[5] = highBeamEffectActive;
    // fp.p.data[6] = lowBeamEffectActive;

    fp.p.data[5] = headLightEffectActive;
    fp.p.data[5] |= headLightSplit << 1;
    fp.p.data[5] |= headLightR << 2;
    fp.p.data[5] |= headLightG << 3;
    fp.p.data[5] |= headLightB << 4;

    fp.p.data[6] = 0;
  }
  else if (lightMode == 2)
  {
    fp.p.data[5] = brakeEffectActive;
    fp.p.data[5] |= reverseLightEffectActive << 1;
    fp.p.data[6] = reverseLightEffectActive;
  }

  wireless.send(&fp);
}