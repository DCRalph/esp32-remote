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

// Command constants
constexpr uint8_t CAR_CMD_PING = 0xe0;
constexpr uint8_t CAR_CMD_SET_MODE = 0xe1;
constexpr uint8_t CAR_CMD_SET_EFFECTS = 0xe2;
constexpr uint8_t CAR_CMD_GET_EFFECTS = 0xe3;

enum class ApplicationMode
{
  NORMAL,
  TEST,
  REMOTE,
  OFF
};

// Struct definitions for wireless communication
struct PingCmd
{
  ApplicationMode mode;
  bool headlight;
  bool taillight;
  bool underglow;
  bool interior;
};

struct SetModeCmd
{
  ApplicationMode mode;
};

struct EffectsCmd
{
  bool leftIndicator;
  bool rightIndicator;

  bool headlight;
  bool headlightSplit;
  bool headlightR;
  bool headlightG;
  bool headlightB;

  bool brake;
  bool reverse;

  bool rgb;
  bool nightrider;
  bool startup;
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

  bool headLightEffectActive = false;
  bool headLightSplit = false;
  bool headLightR = false;
  bool headLightG = false;
  bool headLightB = false;

  bool brakeEffectActive = false;
  bool reverseLightEffectActive = false;

  ApplicationMode mode = ApplicationMode::NORMAL;

  bool isHeadlightEnabled = false;
  bool isTaillightEnabled = false;
  bool isUnderglowEnabled = false;
  bool isInteriorEnabled = false;

  MenuItemToggle connectionItem = MenuItemToggle("Conn", &connected, false);

  std::vector<String> modeItems = {"Norm", "Test", "Rem", "Off"};
  MenuItemSelect modeSelectItem = MenuItemSelect("Mode", modeItems, 0);

  MenuItem lightModeItem = MenuItem("Light Mode");

  MenuItemToggle leftIndicatorEffectItem = MenuItemToggle("Left", &leftIndicatorEffectActive, true);
  MenuItemToggle rightIndicatorEffectItem = MenuItemToggle("Right", &rightIndicatorEffectActive, true);
  MenuItemToggle startupEffectItem = MenuItemToggle("Start", &startupEffectActive, true);
  MenuItemToggle rgbEffectItem = MenuItemToggle("RGB", &rgbEffectActive, true);
  MenuItemToggle nightriderEffectItem = MenuItemToggle("Nrider", &nightriderEffectActive, true);

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
  void getEffects();
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&connectionItem);

  menu.addMenuItem(&modeSelectItem);
  menu.addMenuItem(&lightModeItem);

  menu.addMenuItem(&leftIndicatorEffectItem);
  menu.addMenuItem(&rightIndicatorEffectItem);
  menu.addMenuItem(&startupEffectItem);
  menu.addMenuItem(&rgbEffectItem);
  menu.addMenuItem(&nightriderEffectItem);

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
                               fp.p.type = CAR_CMD_SET_MODE;
                               fp.p.len = 1;
                               fp.p.data[0] = static_cast<uint8_t>(modeSelectItem.getCurrentIndex());
                               wireless.send(&fp); });

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

  String lightModeText = "";

  if (isHeadlightEnabled)
    lightModeText += "H";
  if (isTaillightEnabled)
    lightModeText += "T";
  if (isUnderglowEnabled)
    lightModeText += "U";
  if (isInteriorEnabled)
    lightModeText += "I";

  if (lightModeText.length() == 0)
    lightModeText = "None";

  lightModeItem.setName("Lights: " + lightModeText);

  headLightEffectItem.setHidden(!isHeadlightEnabled);
  headLightSplitItem.setHidden(!isHeadlightEnabled);
  headLightRItem.setHidden(!isHeadlightEnabled);
  headLightGItem.setHidden(!isHeadlightEnabled);
  headLightBItem.setHidden(!isHeadlightEnabled);

  brakeEffectItem.setHidden(!isTaillightEnabled);
  reverseLightEffectItem.setHidden(!isTaillightEnabled);

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

      isHeadlightEnabled = false;
      isTaillightEnabled = false;
      isUnderglowEnabled = false;
      isInteriorEnabled = false;
    }
  }

  if (millis() - lastPing > 500)
  {
    lastPing = millis();

    fullPacket fp;
    memset(&fp, 0, sizeof(fullPacket));
    fp.direction = PacketDirection::SEND;
    memcpy(fp.mac, car_addr, 6);
    fp.p.type = CAR_CMD_PING;
    fp.p.len = 0;

    wireless.send(&fp);
  }
}

void CarControlScreen::onEnter()
{
  // Ping response handler
  wireless.addOnReceiveFor(CAR_CMD_PING, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();

                             PingCmd pCmd;
                             memcpy(&pCmd, fp->p.data, sizeof(PingCmd));

                             mode = pCmd.mode;
                             modeSelectItem.setCurrentIndex((uint8_t)mode);

                             isHeadlightEnabled = pCmd.headlight;
                             isTaillightEnabled = pCmd.taillight;
                             isUnderglowEnabled = pCmd.underglow;
                             isInteriorEnabled = pCmd.interior;

                             // Request current effects
                             getEffects();
                             //
                           });

  // Set mode response handler
  wireless.addOnReceiveFor(CAR_CMD_SET_MODE, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();

                             mode = static_cast<ApplicationMode>(fp->p.data[0]);
                             modeSelectItem.setCurrentIndex((uint8_t)mode);
                             //
                           });

  // Get effects response handler
  wireless.addOnReceiveFor(CAR_CMD_GET_EFFECTS, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();

                             EffectsCmd eCmd;
                             memcpy(&eCmd, fp->p.data, sizeof(EffectsCmd));

                             leftIndicatorEffectActive = eCmd.leftIndicator;
                             rightIndicatorEffectActive = eCmd.rightIndicator;
                             startupEffectActive = eCmd.startup;
                             rgbEffectActive = eCmd.rgb;
                             nightriderEffectActive = eCmd.nightrider;

                             headLightEffectActive = eCmd.headlight;
                             headLightSplit = eCmd.headlightSplit;
                             headLightR = eCmd.headlightR;
                             headLightG = eCmd.headlightG;
                             headLightB = eCmd.headlightB;

                             brakeEffectActive = eCmd.brake;
                             reverseLightEffectActive = eCmd.reverse;
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
  fp.p.type = CAR_CMD_SET_EFFECTS;

  EffectsCmd eCmd = {0};
  eCmd.leftIndicator = leftIndicatorEffectActive;
  eCmd.rightIndicator = rightIndicatorEffectActive;
  eCmd.startup = startupEffectActive;
  eCmd.rgb = rgbEffectActive;
  eCmd.nightrider = nightriderEffectActive;
  eCmd.headlight = headLightEffectActive;
  eCmd.headlightSplit = headLightSplit;
  eCmd.headlightR = headLightR;
  eCmd.headlightG = headLightG;
  eCmd.headlightB = headLightB;
  eCmd.brake = brakeEffectActive;
  eCmd.reverse = reverseLightEffectActive;

  fp.p.len = sizeof(eCmd);
  memcpy(fp.p.data, &eCmd, sizeof(eCmd));

  wireless.send(&fp);
}

void CarControlScreen::getEffects()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, car_addr, 6);
  fp.p.type = CAR_CMD_GET_EFFECTS;
  fp.p.len = 0;

  wireless.send(&fp);
}