#pragma once

#include "config.h"
#include "IO/Display.h"
#include "IO/GPIO.h"
#include "IO/Wireless.h"
#include "IO/Menu.h"

static uint8_t led_controller_addrs[3][8] = {
    {0x30, 0x30, 0xF9, 0x2A, 0x05, 0x20}, // s3 dev 1
    {0x30, 0x30, 0xF9, 0x2A, 0x05, 0x18}, // s3 dev 2
    {0x48, 0x27, 0xe2, 0x46, 0x59, 0xba}  // s2 car 48:27:e2:46:59:ba
};

static std::vector<String> led_controller_names = {"Dev1", "Dev2", "Car"};

static uint8_t led_controller_addr_index = 0;

// Command constants
constexpr uint8_t CAR_CMD_PING = 0xe0;
constexpr uint8_t CAR_CMD_SET_MODE = 0xe1;
constexpr uint8_t CAR_CMD_SET_EFFECTS = 0xe2;
constexpr uint8_t CAR_CMD_GET_EFFECTS = 0xe3;
constexpr uint8_t CAR_CMD_SET_INPUTS = 0xe4;
constexpr uint8_t CAR_CMD_GET_INPUTS = 0xe5;

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

enum class PoliceMode
{
  SLOW,
  FAST
};
struct EffectsCmd
{
  bool testEffect1;
  bool testEffect2;

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
  int startup;
  int headlightStartup;
  bool police;
  PoliceMode policeMode;
};

struct InputsCmd
{
  bool accOn;
  bool indicatorLeft;
  bool indicatorRight;
  bool brake;
  bool reverse;
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

  ApplicationMode mode = ApplicationMode::NORMAL;

  bool isHeadlightEnabled = false;
  bool isTaillightEnabled = false;
  bool isUnderglowEnabled = false;
  bool isInteriorEnabled = false;

  MenuItemSelect ledControllerSelectItem = MenuItemSelect("Ctr", led_controller_names, 0);

  MenuItemToggle connectionItem = MenuItemToggle("Conn", &connected, false);

  std::vector<String> modeItems = {"Norm", "Test", "Rem", "Off"};
  MenuItemSelect modeSelectItem = MenuItemSelect("Mode", modeItems, 0);

  MenuItem lightModeItem = MenuItem("Light M");

  // #########################################################
  // Effects
  // #########################################################
  bool testEffect1Active = false;
  bool testEffect2Active = false;

  bool leftIndicatorEffectActive = false;
  bool rightIndicatorEffectActive = false;
  bool startupEffectActive = false;
  int headlightStartup = 0;
  bool rgbEffectActive = false;
  bool nightriderEffectActive = false;
  bool policeEffectActive = false;
  PoliceMode policeMode = PoliceMode::SLOW;

  bool headLightEffectActive = false;
  bool headLightSplit = false;
  bool headLightR = false;
  bool headLightG = false;
  bool headLightB = false;

  bool brakeEffectActive = false;
  bool reverseLightEffectActive = false;

  MenuItemToggle testEffect1Item = MenuItemToggle("Test 1", &testEffect1Active, true);
  MenuItemToggle testEffect2Item = MenuItemToggle("Test 2", &testEffect2Active, true);

  MenuItemToggle leftIndicatorEffectItem = MenuItemToggle("Left", &leftIndicatorEffectActive, true);
  MenuItemToggle rightIndicatorEffectItem = MenuItemToggle("Right", &rightIndicatorEffectActive, true);
  MenuItemToggle startupEffectItem = MenuItemToggle("Start", &startupEffectActive, true);
  std::vector<String> headlightStartupItems = {"Off", "TOff", "Start", "On"};
  MenuItemSelect headlightStartupItem = MenuItemSelect("Start", headlightStartupItems, 0);

  MenuItemToggle rgbEffectItem = MenuItemToggle("RGB", &rgbEffectActive, true);
  MenuItemToggle nightriderEffectItem = MenuItemToggle("Nrider", &nightriderEffectActive, true);
  MenuItemToggle policeEffectItem = MenuItemToggle("Police", &policeEffectActive, true);
  std::vector<String> policeModeItems = {"Slow", "Fast"};
  MenuItemSelect policeModeItem = MenuItemSelect("Mode", policeModeItems, 0);

  MenuItemToggle headLightEffectItem = MenuItemToggle("Headlight", &headLightEffectActive, true);
  MenuItemToggle headLightSplitItem = MenuItemToggle("Split", &headLightSplit, true);
  MenuItemToggle headLightRItem = MenuItemToggle("R", &headLightR, true);
  MenuItemToggle headLightGItem = MenuItemToggle("G", &headLightG, true);
  MenuItemToggle headLightBItem = MenuItemToggle("B", &headLightB, true);

  MenuItemToggle brakeEffectItem = MenuItemToggle("Brake", &brakeEffectActive, true);
  MenuItemToggle reverseLightEffectItem = MenuItemToggle("Rev", &reverseLightEffectActive, true);

  // #########################################################
  // Inputs
  // #########################################################
  bool accOn = false;
  bool indicatorLeft = false;
  bool indicatorRight = false;
  bool brake = false;
  bool reverse = false;
  bool headlight = false;

  MenuItemToggle accOnItem = MenuItemToggle("Acc", &accOn, true);
  MenuItemToggle indicatorLeftItem = MenuItemToggle("Left", &indicatorLeft, true);
  MenuItemToggle indicatorRightItem = MenuItemToggle("Right", &indicatorRight, true);
  MenuItemToggle brakeItem = MenuItemToggle("Brake", &brake, true);
  MenuItemToggle reverseItem = MenuItemToggle("Reverse", &reverse, true);
  MenuItemToggle headlightItem = MenuItemToggle("H.Light", &headlight, true);

  void draw() override;
  void update() override;
  void onEnter() override;

  void sendEffects();
  void getEffects();

  void sendInputs();
  void getInputs();
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
  // general items
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&ledControllerSelectItem);

  menu.addMenuItem(&connectionItem);

  menu.addMenuItem(&modeSelectItem);
  menu.addMenuItem(&lightModeItem);

  // effects
  menu.addMenuItem(&testEffect1Item);
  menu.addMenuItem(&testEffect2Item);

  menu.addMenuItem(&leftIndicatorEffectItem);
  menu.addMenuItem(&rightIndicatorEffectItem);
  menu.addMenuItem(&startupEffectItem);
  menu.addMenuItem(&headlightStartupItem);
  menu.addMenuItem(&rgbEffectItem);
  menu.addMenuItem(&nightriderEffectItem);
  menu.addMenuItem(&policeEffectItem);
  menu.addMenuItem(&policeModeItem);

  // mode specific effects
  // headlight
  menu.addMenuItem(&headLightEffectItem);
  menu.addMenuItem(&headLightSplitItem);
  menu.addMenuItem(&headLightRItem);
  menu.addMenuItem(&headLightGItem);
  menu.addMenuItem(&headLightBItem);

  // taillight
  menu.addMenuItem(&brakeEffectItem);
  menu.addMenuItem(&reverseLightEffectItem);

  // inputs
  menu.addMenuItem(&accOnItem);
  menu.addMenuItem(&indicatorLeftItem);
  menu.addMenuItem(&indicatorRightItem);
  menu.addMenuItem(&brakeItem);
  menu.addMenuItem(&reverseItem);
  menu.addMenuItem(&headlightItem);

  ledControllerSelectItem.setOnChange([&]()
                                      {
                                        led_controller_addr_index = ledControllerSelectItem.getCurrentIndex();
                                        preferences.putUInt("ctrlr_addr_idx", led_controller_addr_index);
                                        setTopBarText(led_controller_names[led_controller_addr_index]);
                                        //
                                      });

  modeSelectItem.setOnChange([&]()
                             {
                               fullPacket fp;
                               memset(&fp, 0, sizeof(fullPacket));

                               fp.direction = PacketDirection::SEND;
                               memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
                               fp.p.type = CAR_CMD_SET_MODE;
                               fp.p.len = 1;
                               fp.p.data[0] = static_cast<uint8_t>(modeSelectItem.getCurrentIndex());
                               wireless.send(&fp); });

  testEffect1Item.setOnChange([&]()
                              { sendEffects(); });

  testEffect2Item.setOnChange([&]()
                              { sendEffects(); });

  leftIndicatorEffectItem.setOnChange([&]()
                                      { sendEffects(); });

  rightIndicatorEffectItem.setOnChange([&]()
                                       { sendEffects(); });

  startupEffectItem.setOnChange([&]()
                                { sendEffects(); });

  headlightStartupItem.setOnChange([&]()
                                   {
                                     headlightStartup = headlightStartupItem.getCurrentIndex();
                                    sendEffects(); });

  rgbEffectItem.setOnChange([&]()
                            { sendEffects(); });

  nightriderEffectItem.setOnChange([&]()
                                   { sendEffects(); });

  policeEffectItem.setOnChange([&]()
                               { sendEffects(); });

  policeModeItem.setOnChange([&]()
                             {
                               policeMode = static_cast<PoliceMode>(policeModeItem.getCurrentIndex());
                               sendEffects();
                               //
                             });

  headLightEffectItem.setOnChange([&]()
                                  { sendEffects(); });

  headLightSplitItem.setOnChange([&]()
                                 { sendEffects(); });

  headLightRItem.setOnChange([&]()
                             { sendEffects(); });

  headLightGItem.setOnChange([&]()
                             { sendEffects(); });

  headLightBItem.setOnChange([&]()
                             { sendEffects(); });

  brakeEffectItem.setOnChange([&]()
                              { sendEffects(); });

  reverseLightEffectItem.setOnChange([&]()
                                     { sendEffects(); });
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
    lightModeItem.setName("LM: NA");
  else
    lightModeItem.setName("LM: " + lightModeText);

  bool testMode = mode == ApplicationMode::TEST;

  // effects
  testEffect1Item.setHidden(testMode);
  testEffect2Item.setHidden(testMode);

  leftIndicatorEffectItem.setHidden(testMode);
  rightIndicatorEffectItem.setHidden(testMode);
  startupEffectItem.setHidden(testMode);
  headlightStartupItem.setHidden(testMode);
  rgbEffectItem.setHidden(testMode);
  nightriderEffectItem.setHidden(testMode);
  policeEffectItem.setHidden(testMode);
  policeModeItem.setHidden(testMode);

  // mode specific effects
  headLightEffectItem.setHidden(!isHeadlightEnabled || testMode);
  headLightSplitItem.setHidden(!isHeadlightEnabled || testMode);
  headLightRItem.setHidden(!isHeadlightEnabled || testMode);
  headLightGItem.setHidden(!isHeadlightEnabled || testMode);
  headLightBItem.setHidden(!isHeadlightEnabled || testMode);

  brakeEffectItem.setHidden(!isTaillightEnabled || testMode);
  reverseLightEffectItem.setHidden(!isTaillightEnabled || testMode);

  // inputs
  accOnItem.setHidden(!testMode);
  indicatorLeftItem.setHidden(!testMode);
  indicatorRightItem.setHidden(!testMode);
  brakeItem.setHidden(!testMode);
  reverseItem.setHidden(!testMode);
  headlightItem.setHidden(!testMode);

  // Draw connection status icon in top bar

  String connTxt = connected ? "Y" : "N";
  int iconWidth = display.u8g2.getStrWidth(connTxt.c_str());
  int iconX = display.getCustomIconX(iconWidth);
  display.u8g2.drawStr(iconX, 9, connTxt.c_str());

  String modeShortTxt = "";
  if (mode == ApplicationMode::NORMAL)
    modeShortTxt = "N";
  else if (mode == ApplicationMode::TEST)
    modeShortTxt = "T";
  else if (mode == ApplicationMode::REMOTE)
    modeShortTxt = "R";
  else if (mode == ApplicationMode::OFF)
    modeShortTxt = "O";

  modeShortTxt += "-";

  iconWidth = display.u8g2.getStrWidth(modeShortTxt.c_str());
  iconX = display.getCustomIconX(iconWidth);
  display.u8g2.drawStr(iconX, 9, modeShortTxt.c_str());

  // Set current controller name in top bar
  setTopBarText(led_controller_names[led_controller_addr_index]);

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
    memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
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
                             headlightStartup = eCmd.headlightStartup;
                             headlightStartupItem.setCurrentIndex(headlightStartup);
                             rgbEffectActive = eCmd.rgb;
                             nightriderEffectActive = eCmd.nightrider;
                             policeEffectActive = eCmd.police;
                             policeMode = eCmd.policeMode;
                             policeModeItem.setCurrentIndex((uint8_t)policeMode);

                             headLightEffectActive = eCmd.headlight;
                             headLightSplit = eCmd.headlightSplit;
                             headLightR = eCmd.headlightR;
                             headLightG = eCmd.headlightG;
                             headLightB = eCmd.headlightB;

                             brakeEffectActive = eCmd.brake;
                             reverseLightEffectActive = eCmd.reverse;
                             //
                           });

  // Get inputs response handler
  wireless.addOnReceiveFor(CAR_CMD_GET_INPUTS, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();

                             InputsCmd iCmd;
                             memcpy(&iCmd, fp->p.data, sizeof(InputsCmd));

                             accOn = iCmd.accOn;
                             indicatorLeft = iCmd.indicatorLeft;
                             indicatorRight = iCmd.indicatorRight;
                             brake = iCmd.brake;
                             reverse = iCmd.reverse;

                             //
                           });

  led_controller_addr_index = preferences.getUInt("ctrlr_addr_idx", 0);
  ledControllerSelectItem.setCurrentIndex(led_controller_addr_index);

  // Initialize top bar with current device name
  setTopBarText(led_controller_names[led_controller_addr_index]);

  connected = false;
  mode = ApplicationMode::OFF;
}

void CarControlScreen::sendEffects()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CAR_CMD_SET_EFFECTS;

  EffectsCmd eCmd = {0};

  eCmd.testEffect1 = testEffect1Active;
  eCmd.testEffect2 = testEffect2Active;

  eCmd.leftIndicator = leftIndicatorEffectActive;
  eCmd.rightIndicator = rightIndicatorEffectActive;
  eCmd.startup = startupEffectActive;
  eCmd.headlightStartup = headlightStartup;
  eCmd.rgb = rgbEffectActive;
  eCmd.nightrider = nightriderEffectActive;
  eCmd.police = policeEffectActive;
  eCmd.policeMode = policeMode;

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
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CAR_CMD_GET_EFFECTS;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::sendInputs()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CAR_CMD_SET_INPUTS;

  InputsCmd iCmd = {0};
  iCmd.accOn = accOn;
  iCmd.indicatorLeft = indicatorLeft;
  iCmd.indicatorRight = indicatorRight;
  iCmd.brake = brake;
  iCmd.reverse = reverse;

  fp.p.len = sizeof(iCmd);
  memcpy(fp.p.data, &iCmd, sizeof(iCmd));

  wireless.send(&fp);
}

void CarControlScreen::getInputs()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CAR_CMD_GET_INPUTS;
  fp.p.len = 0;

  wireless.send(&fp);
}