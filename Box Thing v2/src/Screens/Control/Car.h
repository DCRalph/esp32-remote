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
constexpr uint8_t CAR_CMD_TRIGGER_SEQUENCE = 0xe6;
constexpr uint8_t CAR_CMD_GET_STATS = 0xe7;

// Sync management commands
constexpr uint8_t CMD_SYNC_GET_DEVICES = 0xe8;
constexpr uint8_t CMD_SYNC_GET_GROUPS = 0xe9;
constexpr uint8_t CMD_SYNC_GET_GROUP_INFO = 0xea;
constexpr uint8_t CMD_SYNC_JOIN_GROUP = 0xeb;
constexpr uint8_t CMD_SYNC_LEAVE_GROUP = 0xec;
constexpr uint8_t CMD_SYNC_CREATE_GROUP = 0xed;
constexpr uint8_t CMD_SYNC_GET_STATUS = 0xee;
constexpr uint8_t CMD_SYNC_SET_AUTO_JOIN = 0xef;
constexpr uint8_t CMD_SYNC_GET_AUTO_JOIN = 0xf0;
constexpr uint8_t CMD_SYNC_SET_AUTO_CREATE = 0xf1;
constexpr uint8_t CMD_SYNC_GET_AUTO_CREATE = 0xf2;

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

enum class SolidColorPreset
{
  OFF = 0,
  RED,
  GREEN,
  BLUE,
  WHITE,
  YELLOW,
  CYAN,
  MAGENTA,
  ORANGE,
  PURPLE,
  LIME,
  PINK,
  TEAL,
  INDIGO,
  GOLD,
  SILVER,
  CUSTOM
};
struct EffectsCmd
{
  bool leftIndicator;
  bool rightIndicator;

  int headlightMode;
  bool headlightSplit;
  bool headlightR;
  bool headlightG;
  bool headlightB;

  int taillightMode;
  bool taillightSplit;

  bool brake;
  bool reverse;

  bool rgb;
  bool nightrider;
  bool police;
  PoliceMode policeMode;
  bool testEffect1;
  bool testEffect2;
  bool solidColor;
  SolidColorPreset solidColorPreset;
  uint8_t solidColorR;
  uint8_t solidColorG;
  uint8_t solidColorB;
};

struct InputsCmd
{
  bool accOn;
  bool indicatorLeft;
  bool indicatorRight;
  bool headlight;
  bool brake;
  bool reverse;
};

struct TriggerSequenceCmd
{
  uint8_t sequence;
};

struct AppStats
{
  uint32_t loopsPerSecond;
  uint32_t updateInputTime;

  uint32_t updateModeTime;
  uint32_t updateSyncTime;

  uint32_t updateEffectsTime;
  uint32_t drawTime;
};

// Sync management data structures
struct SyncDeviceInfo
{
  uint32_t deviceId;
  uint8_t mac[6];
  uint32_t lastSeen;
  uint32_t timeSinceLastSeen; // Calculated field in milliseconds
  bool inCurrentGroup;
  bool isGroupMaster;
  bool isThisDevice;
};

struct SyncDevicesResponse
{
  uint8_t deviceCount;
  uint32_t currentTime;      // Reference time for lastSeen calculations
  SyncDeviceInfo devices[8]; // Reduced to 8 to accommodate larger struct
};

struct SyncGroupInfo
{
  uint32_t groupId;
  uint32_t masterDeviceId;
  uint8_t masterMac[6];
  uint32_t lastSeen;
  uint32_t timeSinceLastSeen; // Calculated field in milliseconds
  bool isCurrentGroup;
  bool canJoin; // True if we're not in a group or this is not our current group
};

struct SyncGroupsResponse
{
  uint8_t groupCount;
  uint32_t currentTime;    // Reference time for lastSeen calculations
  uint32_t ourGroupId;     // Our current group ID (0 if none)
  SyncGroupInfo groups[4]; // Reduced to 4 to accommodate larger struct
};

struct SyncGroupMemberInfo
{
  uint32_t deviceId;
  uint8_t mac[6];
  bool isGroupMaster;
  bool isThisDevice;
  uint32_t lastHeartbeat; // 0 if unknown, otherwise time since last heartbeat
};

struct SyncCurrentGroupInfo
{
  uint32_t groupId;
  uint32_t masterDeviceId;
  bool isMaster;
  bool timeSynced;
  int32_t timeOffset;
  uint32_t syncedTime;
  uint8_t memberCount;
  uint32_t currentTime;           // Reference time
  SyncGroupMemberInfo members[6]; // Limit to 6 members to fit in packet
};

struct SyncJoinGroupCmd
{
  uint32_t groupId;
};

struct SyncCreateGroupCmd
{
  uint32_t groupId; // 0 = auto-generate, otherwise use specified ID
};

struct SyncDetailedStatus
{
  uint32_t deviceId;
  uint32_t groupId;
  uint32_t masterDeviceId;
  bool isMaster;
  bool timeSynced;
  int32_t timeOffset;
  uint32_t syncedTime;
  uint8_t memberCount;
  uint8_t discoveredDeviceCount;
  uint8_t discoveredGroupCount;
  bool autoJoinEnabled;
  bool autoCreateEnabled;
};

struct SyncAutoJoinCmd
{
  bool enabled;
};

struct SyncAutoCreateCmd
{
  bool enabled;
};

class CarControlScreen : public Screen
{
public:
  CarControlScreen(String _name);

  Menu menu = Menu(MenuSize::Medium);

  MenuItemBack backItem;

  uint64_t lastPing = 0;
  uint64_t lastConfirmedPing = 0;
  bool connected = false;
  bool lastConnected = false;

  bool statsActive = false;
  AppStats stats;

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

  // Stats submenu
  MenuItemSubmenu statsUIItem = MenuItemSubmenu("Stats", &statsMenu);
  Menu statsMenu = Menu(MenuSize::Medium);
  MenuItemBack statsBackItem;
  MenuItemToggle statsActiveItem = MenuItemToggle("Active", &statsActive);
  MenuItemNumber<uint32_t> statsLoopsItem = MenuItemNumber<uint32_t>("Loops/Sec", &stats.loopsPerSecond);
  MenuItemNumber<uint32_t> statsUpdateInputTimeItem = MenuItemNumber<uint32_t>("Input Time", &stats.updateInputTime);
  MenuItemNumber<uint32_t> statsUpdateModeTimeItem = MenuItemNumber<uint32_t>("Mode Time", &stats.updateModeTime);
  MenuItemNumber<uint32_t> statsUpdateSyncTimeItem = MenuItemNumber<uint32_t>("Sync Time", &stats.updateSyncTime);
  MenuItemNumber<uint32_t> statsUpdateEffectsTimeItem = MenuItemNumber<uint32_t>("Effects Time", &stats.updateEffectsTime);
  MenuItemNumber<uint32_t> statsDrawTimeItem = MenuItemNumber<uint32_t>("Draw Time", &stats.drawTime);

  // #########################################################
  // Effects
  // #########################################################

  bool leftIndicatorEffectActive = false;
  bool rightIndicatorEffectActive = false;

  int headlightMode = 0;
  bool headLightSplit = false;
  bool headLightR = false;
  bool headLightG = false;
  bool headLightB = false;

  int taillightMode = 0;
  bool taillightSplit = false;

  bool brakeEffectActive = false;
  bool reverseLightEffectActive = false;

  // bool startupEffectActive = false;

  bool rgbEffectActive = false;
  bool nightriderEffectActive = false;
  bool policeEffectActive = false;
  PoliceMode policeMode = PoliceMode::SLOW;
  bool testEffect1Active = false;
  bool testEffect2Active = false;
  bool solidColorEffectActive = false;
  SolidColorPreset solidColorPreset = SolidColorPreset::OFF;
  uint8_t solidColorR = 255;
  uint8_t solidColorG = 255;
  uint8_t solidColorB = 255;

  MenuItemToggle leftIndicatorEffectItem = MenuItemToggle("Left", &leftIndicatorEffectActive, true);
  MenuItemToggle rightIndicatorEffectItem = MenuItemToggle("Right", &rightIndicatorEffectActive, true);

  std::vector<String> headlightModeItems = {"Off", "Srt", "On"};
  MenuItemSelect headlightModeItem = MenuItemSelect("H.Mode", headlightModeItems, 0);
  MenuItemToggle headLightSplitItem = MenuItemToggle("H.Split", &headLightSplit, true);
  MenuItemToggle headLightRItem = MenuItemToggle("H.R", &headLightR, true);
  MenuItemToggle headLightGItem = MenuItemToggle("H.G", &headLightG, true);
  MenuItemToggle headLightBItem = MenuItemToggle("H.B", &headLightB, true);

  std::vector<String> taillightModeItems = {"Off", "Srt", "On", "Dim"};
  MenuItemSelect taillightModeItem = MenuItemSelect("T.Mode", taillightModeItems, 0);
  MenuItemToggle taillightSplitItem = MenuItemToggle("T.Split", &taillightSplit, true);

  MenuItemToggle brakeEffectItem = MenuItemToggle("Brake", &brakeEffectActive, true);
  MenuItemToggle reverseLightEffectItem = MenuItemToggle("Rev", &reverseLightEffectActive, true);

  MenuItemToggle rgbEffectItem = MenuItemToggle("RGB", &rgbEffectActive, true);
  MenuItemToggle nightriderEffectItem = MenuItemToggle("Nrider", &nightriderEffectActive, true);
  MenuItemToggle policeEffectItem = MenuItemToggle("Police", &policeEffectActive, true);
  std::vector<String> policeModeItems = {"Slow", "Fast"};
  MenuItemSelect policeModeItem = MenuItemSelect("Mode", policeModeItems, 0);
  MenuItemToggle testEffect1Item = MenuItemToggle("Test 1", &testEffect1Active, true);
  MenuItemToggle testEffect2Item = MenuItemToggle("Test 2", &testEffect2Active, true);
  MenuItemToggle solidColorEffectItem = MenuItemToggle("Solid", &solidColorEffectActive, true);
  std::vector<String> solidColorPresetItems = {"Off", "Red", "Green", "Blue", "White", "Yellow", "Cyan", "Magenta", "Orange", "Purple", "Lime", "Pink", "Teal", "Indigo", "Gold", "Silver", "Custom"};
  MenuItemSelect solidColorPresetItem = MenuItemSelect("SC Col", solidColorPresetItems, 0);
  MenuItemNumber<uint8_t> solidColorRItem = MenuItemNumber<uint8_t>("SC R", &solidColorR, 0, 255, 5);
  MenuItemNumber<uint8_t> solidColorGItem = MenuItemNumber<uint8_t>("SC G", &solidColorG, 0, 255, 5);
  MenuItemNumber<uint8_t> solidColorBItem = MenuItemNumber<uint8_t>("SC B", &solidColorB, 0, 255, 5);

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
  MenuItemToggle headlightItem = MenuItemToggle("H.Light", &headlight, true);
  MenuItemToggle brakeItem = MenuItemToggle("Brake", &brake, true);
  MenuItemToggle reverseItem = MenuItemToggle("Reverse", &reverse, true);

  MenuItemAction unlockSequenceTriggerItem = MenuItemAction("Unlock Seq", 1, [&]()
                                                            {
                                                              triggerSequence(0);
                                                              //
                                                            });
  MenuItemAction lockSequenceTriggerItem = MenuItemAction("Lock Seq", 1, [&]()
                                                          {
                                                            triggerSequence(1);
                                                            //
                                                          });

  MenuItemAction rgbSequenceTriggerItem = MenuItemAction("RGB Seq", 1, [&]()
                                                         {
                                                           triggerSequence(2);
                                                           //
                                                         });

  MenuItemAction nightRiderSequenceTriggerItem = MenuItemAction("Night Rider Seq", 1, [&]()
                                                                {
                                                                  triggerSequence(3);
                                                                  //
                                                                });

  // #########################################################
  // Sync Management UI - Redesigned
  // #########################################################

  // Main sync menu
  Menu syncMenu = Menu(MenuSize::Medium);
  MenuItemBack syncBackItem;
  MenuItemAction syncRefreshItem = MenuItemAction("Refresh All", 1, [&]()
                                                  { this->syncRefreshData(); });

  // This Device section
  MenuItemSubmenu syncThisDeviceUIItem = MenuItemSubmenu("This Device", &syncThisDeviceMenu);
  Menu syncThisDeviceMenu = Menu(MenuSize::Small);
  MenuItemBack syncThisDeviceBackItem;
  MenuItem thisDeviceIdItem = MenuItem("ID: ---");
  MenuItem thisDeviceGroupItem = MenuItem("Group: ---");
  MenuItem thisDeviceStatusItem = MenuItem("Status: ---");
  MenuItemToggle autoJoinItem = MenuItemToggle("Auto Join", &autoJoinEnabled, true);
  MenuItemToggle autoCreateItem = MenuItemToggle("Auto Create", &autoCreateEnabled, true);
  MenuItemAction leaveGroupItem = MenuItemAction("Leave Group", 1, [&]()
                                                 { this->leaveSyncGroup(); });

  // Current Group section
  MenuItemSubmenu syncCurrentGroupUIItem = MenuItemSubmenu("Current Group", &syncCurrentGroupMenu);
  Menu syncCurrentGroupMenu = Menu(MenuSize::Small);
  MenuItemBack syncCurrentGroupBackItem;
  MenuItem currentGroupIdItem = MenuItem("ID: ---");
  MenuItem currentGroupMasterItem = MenuItem("Master: ---");
  MenuItem currentGroupMembersItem = MenuItem("Members: ---");
  MenuItem currentGroupSyncItem = MenuItem("Sync: ---");

  // Group Members submenu (for current group)
  MenuItemSubmenu groupMembersUIItem = MenuItemSubmenu("View Members", &groupMembersMenu);
  Menu groupMembersMenu = Menu(MenuSize::Small);
  MenuItemBack groupMembersBackItem;
  // Member items will be dynamically added

  // Discovered Devices section
  MenuItemSubmenu syncDevicesUIItem = MenuItemSubmenu("Devices", &syncDevicesMenu);
  Menu syncDevicesMenu = Menu(MenuSize::Medium);
  MenuItemBack syncDevicesBackItem;
  // Device items will be statically created (max 8)
  MenuItem deviceItems[8] = {
      MenuItem("Device 1: ---"),
      MenuItem("Device 2: ---"),
      MenuItem("Device 3: ---"),
      MenuItem("Device 4: ---"),
      MenuItem("Device 5: ---"),
      MenuItem("Device 6: ---"),
      MenuItem("Device 7: ---"),
      MenuItem("Device 8: ---")};

  // Device Detail submenu
  MenuItemSubmenu deviceDetailUIItem = MenuItemSubmenu("Device Detail", &deviceDetailMenu);
  Menu deviceDetailMenu = Menu(MenuSize::Small);
  MenuItemBack deviceDetailBackItem;
  MenuItem deviceDetailIdItem = MenuItem("ID: ---");
  MenuItem deviceDetailMacItem = MenuItem("MAC: ---");
  MenuItem deviceDetailLastSeenItem = MenuItem("Last: ---");
  MenuItem deviceDetailStatusItem = MenuItem("Status: ---");
  MenuItem deviceDetailGroupItem = MenuItem("Group: ---");

  // Discovered Groups section
  MenuItemSubmenu syncGroupsUIItem = MenuItemSubmenu("Groups", &syncGroupsMenu);
  Menu syncGroupsMenu = Menu(MenuSize::Medium);
  MenuItemBack syncGroupsBackItem;
  // Group items will be statically created (max 4)
  MenuItem groupItems[4] = {
      MenuItem("Group 1: ---"),
      MenuItem("Group 2: ---"),
      MenuItem("Group 3: ---"),
      MenuItem("Group 4: ---")};

  // Group Detail submenu
  MenuItemSubmenu groupDetailUIItem = MenuItemSubmenu("Group Detail", &groupDetailMenu);
  Menu groupDetailMenu = Menu(MenuSize::Small);
  MenuItemBack groupDetailBackItem;
  MenuItem groupDetailIdItem = MenuItem("ID: ---");
  MenuItem groupDetailMasterItem = MenuItem("Master: ---");
  MenuItem groupDetailMacItem = MenuItem("MAC: ---");
  MenuItem groupDetailLastSeenItem = MenuItem("Last: ---");
  MenuItem groupDetailStatusItem = MenuItem("Status: ---");
  MenuItemAction groupJoinItem = MenuItemAction("Join Group", 1, [&]()
                                                { this->joinSelectedGroup(); });

  // Group Management
  MenuItemAction syncCreateGroupItem = MenuItemAction("Create Group", 1, [&]()
                                                      { this->syncCreateGroup(); });

  MenuItemSubmenu syncUIItem = MenuItemSubmenu("Sync", &syncMenu);

  // Sync data storage
  SyncDevicesResponse syncDevices;
  SyncGroupsResponse syncGroups;
  SyncCurrentGroupInfo syncCurrentGroup;
  SyncDetailedStatus syncStatus;

  // Selection tracking and state
  uint32_t selectedDeviceId = 0;
  uint32_t selectedGroupId = 0;
  bool autoJoinEnabled = false;
  bool autoCreateEnabled = false;

  // timing
  uint64_t lastSyncUpdate = 0;
  uint64_t lastSyncAutoRefresh = 0;

  void draw() override;
  void update() override;
  void onEnter() override;

  void sendEffects();
  void getEffects();

  void sendInputs();
  void getInputs();

  void triggerSequence(uint8_t seq);

  // Sync management methods
  void syncRefreshData(bool showNotification = true);
  void syncCreateGroup(uint32_t groupId = 0);

  void requestSyncDevices();
  void requestSyncGroups();
  void requestSyncGroupInfo();
  void requestSyncStatus();
  void joinSyncGroup(uint32_t groupId);
  void joinSelectedGroup();
  void leaveSyncGroup();

  // Auto join/create methods
  void setAutoJoin(bool enabled);
  void setAutoCreate(bool enabled);
  void requestAutoJoinStatus();
  void requestAutoCreateStatus();

  // UI update methods
  void updateThisDeviceDisplay();
  void updateCurrentGroupDisplay();
  void updateDevicesDisplay();
  void updateGroupsDisplay();
  void updateGroupMembersDisplay();
  void showDeviceDetail(uint32_t deviceId);
  void showGroupDetail(uint32_t groupId);

  // Utility methods
  String formatMacAddress(uint8_t *mac);
  String formatTimestamp(uint32_t timestamp);
  String formatTimeDuration(uint32_t milliseconds);
};

CarControlScreen::CarControlScreen(String _name) : Screen(_name)
{
  // general items
  menu.addMenuItem(&backItem);

  menu.addMenuItem(&ledControllerSelectItem);

  menu.addMenuItem(&connectionItem);

  menu.addMenuItem(&modeSelectItem);
  menu.addMenuItem(&lightModeItem);

  // sync management
  menu.addMenuItem(&syncUIItem);

  // stats submenu
  menu.addMenuItem(&statsUIItem);

  menu.addMenuItem(&leftIndicatorEffectItem);
  menu.addMenuItem(&rightIndicatorEffectItem);

  // headlight
  menu.addMenuItem(&headlightModeItem);
  menu.addMenuItem(&headLightSplitItem);
  menu.addMenuItem(&headLightRItem);
  menu.addMenuItem(&headLightGItem);
  menu.addMenuItem(&headLightBItem);

  // taillight
  menu.addMenuItem(&taillightModeItem);
  menu.addMenuItem(&taillightSplitItem);

  // taillight
  menu.addMenuItem(&brakeEffectItem);
  menu.addMenuItem(&reverseLightEffectItem);

  menu.addMenuItem(&rgbEffectItem);
  menu.addMenuItem(&nightriderEffectItem);
  menu.addMenuItem(&policeEffectItem);
  menu.addMenuItem(&policeModeItem);
  menu.addMenuItem(&testEffect1Item);
  menu.addMenuItem(&testEffect2Item);
  menu.addMenuItem(&solidColorEffectItem);
  menu.addMenuItem(&solidColorPresetItem);
  menu.addMenuItem(&solidColorRItem);
  menu.addMenuItem(&solidColorGItem);
  menu.addMenuItem(&solidColorBItem);
  solidColorRItem.setFastUpdate(true);
  solidColorGItem.setFastUpdate(true);
  solidColorBItem.setFastUpdate(true);

  // inputs
  menu.addMenuItem(&accOnItem);
  menu.addMenuItem(&indicatorLeftItem);
  menu.addMenuItem(&indicatorRightItem);
  menu.addMenuItem(&headlightItem);
  menu.addMenuItem(&brakeItem);
  menu.addMenuItem(&reverseItem);

  // sequences
  menu.addMenuItem(&unlockSequenceTriggerItem);
  menu.addMenuItem(&lockSequenceTriggerItem);
  menu.addMenuItem(&rgbSequenceTriggerItem);
  menu.addMenuItem(&nightRiderSequenceTriggerItem);

  // Setup main sync menu
  syncMenu.addMenuItem(&syncBackItem);
  syncMenu.addMenuItem(&syncRefreshItem);
  syncMenu.addMenuItem(&syncThisDeviceUIItem);
  syncMenu.addMenuItem(&syncCurrentGroupUIItem);
  syncMenu.addMenuItem(&syncDevicesUIItem);
  syncMenu.addMenuItem(&syncGroupsUIItem);
  syncMenu.addMenuItem(&syncCreateGroupItem);

  // Setup This Device submenu
  syncThisDeviceMenu.addMenuItem(&syncThisDeviceBackItem);
  syncThisDeviceMenu.addMenuItem(&thisDeviceIdItem);
  syncThisDeviceMenu.addMenuItem(&thisDeviceGroupItem);
  syncThisDeviceMenu.addMenuItem(&thisDeviceStatusItem);
  syncThisDeviceMenu.addMenuItem(&autoJoinItem);
  syncThisDeviceMenu.addMenuItem(&autoCreateItem);
  syncThisDeviceMenu.addMenuItem(&leaveGroupItem);
  syncThisDeviceMenu.setParentMenu(&syncMenu);

  // Setup Current Group submenu
  syncCurrentGroupMenu.addMenuItem(&syncCurrentGroupBackItem);
  syncCurrentGroupMenu.addMenuItem(&currentGroupIdItem);
  syncCurrentGroupMenu.addMenuItem(&currentGroupMasterItem);
  syncCurrentGroupMenu.addMenuItem(&currentGroupMembersItem);
  syncCurrentGroupMenu.addMenuItem(&currentGroupSyncItem);
  syncCurrentGroupMenu.addMenuItem(&groupMembersUIItem);
  syncCurrentGroupMenu.setParentMenu(&syncMenu);

  // Setup Group Members submenu
  groupMembersMenu.addMenuItem(&groupMembersBackItem);
  groupMembersMenu.setParentMenu(&syncCurrentGroupMenu);

  // Setup devices submenu
  syncDevicesMenu.addMenuItem(&syncDevicesBackItem);
  for (int i = 0; i < 8; i++)
  {
    syncDevicesMenu.addMenuItem(&deviceItems[i]);
  }
  syncDevicesMenu.setParentMenu(&syncMenu);

  // Setup device detail submenu
  deviceDetailMenu.addMenuItem(&deviceDetailBackItem);
  deviceDetailMenu.addMenuItem(&deviceDetailIdItem);
  deviceDetailMenu.addMenuItem(&deviceDetailMacItem);
  deviceDetailMenu.addMenuItem(&deviceDetailLastSeenItem);
  deviceDetailMenu.addMenuItem(&deviceDetailStatusItem);
  deviceDetailMenu.addMenuItem(&deviceDetailGroupItem);
  deviceDetailMenu.setParentMenu(&syncDevicesMenu);

  // Setup groups submenu
  syncGroupsMenu.addMenuItem(&syncGroupsBackItem);
  for (int i = 0; i < 4; i++)
  {
    syncGroupsMenu.addMenuItem(&groupItems[i]);
  }
  syncGroupsMenu.setParentMenu(&syncMenu);

  // Setup group detail submenu
  groupDetailMenu.addMenuItem(&groupDetailBackItem);
  groupDetailMenu.addMenuItem(&groupDetailIdItem);
  groupDetailMenu.addMenuItem(&groupDetailMasterItem);
  groupDetailMenu.addMenuItem(&groupDetailMacItem);
  groupDetailMenu.addMenuItem(&groupDetailLastSeenItem);
  groupDetailMenu.addMenuItem(&groupDetailStatusItem);
  groupDetailMenu.addMenuItem(&groupJoinItem);
  groupDetailMenu.setParentMenu(&syncGroupsMenu);

  // Setup stats submenu
  statsMenu.addMenuItem(&statsBackItem);
  statsMenu.addMenuItem(&statsActiveItem);
  statsMenu.addMenuItem(&statsLoopsItem);
  statsMenu.addMenuItem(&statsUpdateInputTimeItem);
  statsMenu.addMenuItem(&statsUpdateModeTimeItem);
  statsMenu.addMenuItem(&statsUpdateSyncTimeItem);
  statsMenu.addMenuItem(&statsUpdateEffectsTimeItem);
  statsMenu.addMenuItem(&statsDrawTimeItem);
  statsMenu.setParentMenu(&menu);

  // Set up sync menu enter callback to refresh data
  syncUIItem.addFunc(1, [&]()
                     { syncRefreshData(); });

  // Set up callbacks for entering submenus
  syncThisDeviceUIItem.addFunc(1, [&]()
                               { 
                                 requestSyncStatus();
                                 requestAutoJoinStatus();
                                 requestAutoCreateStatus();
                                 updateThisDeviceDisplay(); });

  syncCurrentGroupUIItem.addFunc(1, [&]()
                                 { 
                                   requestSyncGroupInfo();
                                   updateCurrentGroupDisplay(); });

  syncDevicesUIItem.addFunc(1, [&]()
                            { 
                             requestSyncDevices(); 
                             updateDevicesDisplay(); });

  syncGroupsUIItem.addFunc(1, [&]()
                           { 
                            requestSyncGroups(); 
                            updateGroupsDisplay(); });

  groupMembersUIItem.addFunc(1, [&]()
                             { updateGroupMembersDisplay(); });

  // Set up auto join/create callbacks
  autoJoinItem.setOnChange([&]()
                           { setAutoJoin(autoJoinEnabled); });

  autoCreateItem.setOnChange([&]()
                             { setAutoCreate(autoCreateEnabled); });

  // Set up device selection callbacks
  for (int i = 0; i < 8; i++)
  {
    deviceItems[i].addFunc(1, [this, i]()
                           { 
                             if (i < syncDevices.deviceCount && !deviceItems[i].isHidden()) {
                               showDeviceDetail(syncDevices.devices[i].deviceId);
                             } });
  }

  // Set up group selection callbacks
  for (int i = 0; i < 4; i++)
  {
    groupItems[i].addFunc(1, [this, i]()
                          { 
                            if (i < syncGroups.groupCount && !groupItems[i].isHidden()) {
                              showGroupDetail(syncGroups.groups[i].groupId);
                            } });
  }

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

  headlightModeItem.setOnChange([&]()
                                {
                                  headlightMode = headlightModeItem.getCurrentIndex();
                                  sendEffects(); });

  headLightSplitItem.setOnChange([&]()
                                 { sendEffects(); });

  headLightRItem.setOnChange([&]()
                             { sendEffects(); });

  headLightGItem.setOnChange([&]()
                             { sendEffects(); });

  headLightBItem.setOnChange([&]()
                             { sendEffects(); });

  taillightModeItem.setOnChange([&]()
                                {
                                  taillightMode = taillightModeItem.getCurrentIndex();
                                  sendEffects(); });
  taillightSplitItem.setOnChange([&]()
                                 { sendEffects(); });

  rgbEffectItem.setOnChange([&]()
                            { sendEffects(); });

  nightriderEffectItem.setOnChange([&]()
                                   { sendEffects(); });

  policeEffectItem.setOnChange([&]()
                               { sendEffects(); });

  policeModeItem.setOnChange([&]()
                             {
                               policeMode = static_cast<PoliceMode>(policeModeItem.getCurrentIndex());
                               sendEffects(); });

  brakeEffectItem.setOnChange([&]()
                              { sendEffects(); });

  reverseLightEffectItem.setOnChange([&]()
                                     { sendEffects(); });

  accOnItem.setOnChange([&]()
                        { sendInputs(); });

  indicatorLeftItem.setOnChange([&]()
                                { sendInputs(); });

  indicatorRightItem.setOnChange([&]()
                                 { sendInputs(); });

  headlightItem.setOnChange([&]()
                            { sendInputs(); });

  brakeItem.setOnChange([&]()
                        { sendInputs(); });

  reverseItem.setOnChange([&]()
                          { sendInputs(); });

  solidColorEffectItem.setOnChange([&]()
                                   { sendEffects(); });

  solidColorPresetItem.setOnChange([&]()
                                   {
                                     solidColorPreset = static_cast<SolidColorPreset>(solidColorPresetItem.getCurrentIndex());
                                     sendEffects(); });

  solidColorRItem.setOnChange([&]()
                              { sendEffects(); });

  solidColorGItem.setOnChange([&]()
                              { sendEffects(); });

  solidColorBItem.setOnChange([&]()
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

  // Stats items visibility is now handled within the stats submenu
  statsLoopsItem.setHidden(!statsActive);
  statsUpdateInputTimeItem.setHidden(!statsActive);
  statsUpdateModeTimeItem.setHidden(!statsActive);
  statsUpdateSyncTimeItem.setHidden(!statsActive);
  statsUpdateEffectsTimeItem.setHidden(!statsActive);
  statsDrawTimeItem.setHidden(!statsActive);

  bool testMode = mode == ApplicationMode::TEST;

  // effects

  leftIndicatorEffectItem.setHidden(testMode);
  rightIndicatorEffectItem.setHidden(testMode);

  // mode specific effects
  headlightModeItem.setHidden(!isHeadlightEnabled || testMode);
  headLightSplitItem.setHidden(!isHeadlightEnabled || testMode);
  headLightRItem.setHidden(!isHeadlightEnabled || testMode);
  headLightGItem.setHidden(!isHeadlightEnabled || testMode);
  headLightBItem.setHidden(!isHeadlightEnabled || testMode);

  taillightModeItem.setHidden(!isTaillightEnabled || testMode);
  taillightSplitItem.setHidden(!isTaillightEnabled || testMode);

  brakeEffectItem.setHidden(!isTaillightEnabled || testMode);
  reverseLightEffectItem.setHidden(!isTaillightEnabled || testMode);

  rgbEffectItem.setHidden(testMode);
  nightriderEffectItem.setHidden(testMode);
  policeEffectItem.setHidden(testMode);
  policeModeItem.setHidden(testMode);
  testEffect1Item.setHidden(testMode);
  testEffect2Item.setHidden(testMode);
  solidColorEffectItem.setHidden(testMode);
  solidColorPresetItem.setHidden(testMode);
  solidColorRItem.setHidden(testMode);
  solidColorGItem.setHidden(testMode);
  solidColorBItem.setHidden(testMode);

  // inputs
  accOnItem.setHidden(!testMode);
  indicatorLeftItem.setHidden(!testMode);
  indicatorRightItem.setHidden(!testMode);
  headlightItem.setHidden(!testMode);
  brakeItem.setHidden(!testMode);
  reverseItem.setHidden(!testMode);

  // sequences
  unlockSequenceTriggerItem.setHidden(!testMode);
  lockSequenceTriggerItem.setHidden(!testMode);
  rgbSequenceTriggerItem.setHidden(!testMode);
  nightRiderSequenceTriggerItem.setHidden(!testMode);

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

  // Auto-refresh sync data when sync menus are active
  if (connected && millis() - lastSyncAutoRefresh > 2000)
  {
    lastSyncAutoRefresh = millis();
    // Check if any sync-related menu is currently active
    Menu *currentMenu = menu.getActiveSubmenu();
    bool syncMenuActive = (currentMenu == &syncMenu);

    if (syncMenuActive)
    {
      syncRefreshData(false); // Don't show notification for auto-refresh
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

    if (statsActive)
    {
      fullPacket fp2;
      memset(&fp2, 0, sizeof(fullPacket));
      fp2.direction = PacketDirection::SEND;
      memcpy(fp2.mac, led_controller_addrs[led_controller_addr_index], 6);
      fp2.p.type = CAR_CMD_GET_STATS;
      fp2.p.len = 0;

      wireless.send(&fp2);
    }
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
                             getInputs();
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

                             headlightModeItem.setCurrentIndex(headlightMode);
                             headlightMode = eCmd.headlightMode;
                             headLightSplit = eCmd.headlightSplit;
                             headLightR = eCmd.headlightR;
                             headLightG = eCmd.headlightG;
                             headLightB = eCmd.headlightB;

                             taillightModeItem.setCurrentIndex(taillightMode);
                             taillightMode = eCmd.taillightMode;
                             taillightSplit = eCmd.taillightSplit;

                             brakeEffectActive = eCmd.brake;
                             reverseLightEffectActive = eCmd.reverse;

                             rgbEffectActive = eCmd.rgb;
                             nightriderEffectActive = eCmd.nightrider;
                             policeEffectActive = eCmd.police;
                             policeMode = eCmd.policeMode;
                             policeModeItem.setCurrentIndex((uint8_t)policeMode);
                             testEffect1Active = eCmd.testEffect1;
                             testEffect2Active = eCmd.testEffect2;
                             solidColorEffectActive = eCmd.solidColor;
                             solidColorPreset = eCmd.solidColorPreset;
                             solidColorPresetItem.setCurrentIndex((uint8_t)solidColorPreset);
                             solidColorR = eCmd.solidColorR;
                             solidColorG = eCmd.solidColorG;
                             solidColorB = eCmd.solidColorB;

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
                             headlight = iCmd.headlight;
                             brake = iCmd.brake;
                             reverse = iCmd.reverse;

                             //
                           });

  // Get stats response handler
  wireless.addOnReceiveFor(CAR_CMD_GET_STATS, [&](fullPacket *fp)
                           {
                             memcpy(&stats, fp->p.data, sizeof(AppStats));

                             //
                           });

  // Sync management response handlers
  wireless.addOnReceiveFor(CMD_SYNC_GET_DEVICES, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             memcpy(&syncDevices, fp->p.data, sizeof(SyncDevicesResponse)); 
                             
                             // Log received devices data
                             String logMsg = "=== SYNC DEVICES RECEIVED ===\n";
                             logMsg += "Device count: " + String(syncDevices.deviceCount) + "\n";
                             logMsg += "Current time: " + String(syncDevices.currentTime) + "\n";
                             for (int i = 0; i < syncDevices.deviceCount; i++) {
                               SyncDeviceInfo &device = syncDevices.devices[i];
                               logMsg += "Device " + String(i) + ":\n";
                               logMsg += "  ID: " + String(device.deviceId) + "\n";
                               logMsg += "  MAC: " + formatMacAddress(device.mac) + "\n";
                               logMsg += "  Last seen: " + String(device.lastSeen) + "\n";
                               logMsg += "  Time since: " + String(device.timeSinceLastSeen) + "ms\n";
                               logMsg += "  In group: " + String(device.inCurrentGroup) + "\n";
                               logMsg += "  Is master: " + String(device.isGroupMaster) + "\n";
                               logMsg += "  Is me: " + String(device.isThisDevice) + "\n";
                             }
                             logMsg += "=============================";
                             Serial.println(logMsg);
                             
                             updateDevicesDisplay(); });

  wireless.addOnReceiveFor(CMD_SYNC_GET_GROUPS, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             memcpy(&syncGroups, fp->p.data, sizeof(SyncGroupsResponse));
                             
                             // Log received groups data
                             String logMsg = "=== SYNC GROUPS RECEIVED ===\n";
                             logMsg += "Group count: " + String(syncGroups.groupCount) + "\n";
                             logMsg += "Current time: " + String(syncGroups.currentTime) + "\n";
                             logMsg += "Our group ID: " + String(syncGroups.ourGroupId) + "\n";
                             for (int i = 0; i < syncGroups.groupCount; i++) {
                               SyncGroupInfo &group = syncGroups.groups[i];
                               logMsg += "Group " + String(i) + ":\n";
                               logMsg += "  ID: " + String(group.groupId) + "\n";
                               logMsg += "  Master ID: " + String(group.masterDeviceId) + "\n";
                               logMsg += "  Master MAC: " + formatMacAddress(group.masterMac) + "\n";
                               logMsg += "  Last seen: " + String(group.lastSeen) + "\n";
                               logMsg += "  Time since: " + String(group.timeSinceLastSeen) + "ms\n";
                               logMsg += "  Is current: " + String(group.isCurrentGroup) + "\n";
                               logMsg += "  Can join: " + String(group.canJoin) + "\n";
                             }
                             logMsg += "============================";
                             Serial.println(logMsg);
                             
                             updateGroupsDisplay(); });

  wireless.addOnReceiveFor(CMD_SYNC_GET_GROUP_INFO, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             memcpy(&syncCurrentGroup, fp->p.data, sizeof(SyncCurrentGroupInfo));
                             
                             // Log received group info data
                             String logMsg = "=== SYNC GROUP INFO RECEIVED ===\n";
                             logMsg += "Group ID: " + String(syncCurrentGroup.groupId) + "\n";
                             logMsg += "Master ID: " + String(syncCurrentGroup.masterDeviceId) + "\n";
                             logMsg += "Is master: " + String(syncCurrentGroup.isMaster) + "\n";
                             logMsg += "Time synced: " + String(syncCurrentGroup.timeSynced) + "\n";
                             logMsg += "Time offset: " + String(syncCurrentGroup.timeOffset) + "\n";
                             logMsg += "Synced time: " + String(syncCurrentGroup.syncedTime) + "\n";
                             logMsg += "Member count: " + String(syncCurrentGroup.memberCount) + "\n";
                             logMsg += "Current time: " + String(syncCurrentGroup.currentTime) + "\n";
                             for (int i = 0; i < syncCurrentGroup.memberCount; i++) {
                               SyncGroupMemberInfo &member = syncCurrentGroup.members[i];
                               logMsg += "Member " + String(i) + ":\n";
                               logMsg += "  ID: " + String(member.deviceId) + "\n";
                               logMsg += "  MAC: " + formatMacAddress(member.mac) + "\n";
                               logMsg += "  Is master: " + String(member.isGroupMaster) + "\n";
                               logMsg += "  Is me: " + String(member.isThisDevice) + "\n";
                               logMsg += "  Last heartbeat: " + String(member.lastHeartbeat) + "\n";
                             }
                             logMsg += "=================================";
                             Serial.println(logMsg);
                             
                             updateCurrentGroupDisplay();
                             updateGroupMembersDisplay(); });

  wireless.addOnReceiveFor(CMD_SYNC_GET_STATUS, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             memcpy(&syncStatus, fp->p.data, sizeof(SyncDetailedStatus));
                             
                             // Log received status data
                             String logMsg = "=== SYNC STATUS RECEIVED ===\n";
                             logMsg += "Device ID: " + String(syncStatus.deviceId) + "\n";
                             logMsg += "Group ID: " + String(syncStatus.groupId) + "\n";
                             logMsg += "Master ID: " + String(syncStatus.masterDeviceId) + "\n";
                             logMsg += "Is master: " + String(syncStatus.isMaster) + "\n";
                             logMsg += "Time synced: " + String(syncStatus.timeSynced) + "\n";
                             logMsg += "Time offset: " + String(syncStatus.timeOffset) + "\n";
                             logMsg += "Synced time: " + String(syncStatus.syncedTime) + "\n";
                             logMsg += "Member count: " + String(syncStatus.memberCount) + "\n";
                             logMsg += "Discovered devices: " + String(syncStatus.discoveredDeviceCount) + "\n";
                             logMsg += "Discovered groups: " + String(syncStatus.discoveredGroupCount) + "\n";
                             logMsg += "Auto join enabled: " + String(syncStatus.autoJoinEnabled) + "\n";
                             logMsg += "Auto create enabled: " + String(syncStatus.autoCreateEnabled) + "\n";
                             logMsg += "============================";
                             Serial.println(logMsg);
                             
                             updateThisDeviceDisplay(); });

  wireless.addOnReceiveFor(CMD_SYNC_JOIN_GROUP, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             display.showNotification("Joined group!", 1500);
                             // Refresh all sync data
                             requestSyncStatus();
                             requestSyncGroupInfo(); });

  wireless.addOnReceiveFor(CMD_SYNC_LEAVE_GROUP, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             display.showNotification("Left group!", 1500);
                             // Clear current group and refresh status
                             memset(&syncCurrentGroup, 0, sizeof(syncCurrentGroup));
                             requestSyncStatus(); });

  wireless.addOnReceiveFor(CMD_SYNC_CREATE_GROUP, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             display.showNotification("Group created!", 1500);
                             // Refresh all sync data
                             requestSyncStatus();
                             requestSyncGroupInfo(); });

  wireless.addOnReceiveFor(CMD_SYNC_SET_AUTO_JOIN, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             display.showNotification("Auto Join updated", 1000); });

  wireless.addOnReceiveFor(CMD_SYNC_GET_AUTO_JOIN, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             autoJoinEnabled = fp->p.data[0] != 0;
                             
                             // Log received auto join status
                             String logMsg = "=== AUTO JOIN STATUS RECEIVED ===\n";
                             logMsg += "Auto join enabled: " + String(autoJoinEnabled) + "\n";
                             logMsg += "=================================";
                             Serial.println(logMsg); });

  wireless.addOnReceiveFor(CMD_SYNC_SET_AUTO_CREATE, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             display.showNotification("Auto Create updated", 1000); });

  wireless.addOnReceiveFor(CMD_SYNC_GET_AUTO_CREATE, [&](fullPacket *fp)
                           {
                             lastConfirmedPing = millis();
                             autoCreateEnabled = fp->p.data[0] != 0;
                             
                             // Log received auto create status
                             String logMsg = "=== AUTO CREATE STATUS RECEIVED ===\n";
                             logMsg += "Auto create enabled: " + String(autoCreateEnabled) + "\n";
                             logMsg += "===================================";
                             Serial.println(logMsg); });

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

  eCmd.leftIndicator = leftIndicatorEffectActive;
  eCmd.rightIndicator = rightIndicatorEffectActive;

  eCmd.headlightMode = headlightMode;
  eCmd.headlightSplit = headLightSplit;
  eCmd.headlightR = headLightR;
  eCmd.headlightG = headLightG;
  eCmd.headlightB = headLightB;

  eCmd.taillightMode = taillightMode;
  eCmd.taillightSplit = taillightSplit;

  eCmd.brake = brakeEffectActive;
  eCmd.reverse = reverseLightEffectActive;

  eCmd.testEffect1 = testEffect1Active;
  eCmd.testEffect2 = testEffect2Active;
  eCmd.rgb = rgbEffectActive;
  eCmd.nightrider = nightriderEffectActive;
  eCmd.police = policeEffectActive;
  eCmd.policeMode = policeMode;
  eCmd.solidColor = solidColorEffectActive;
  eCmd.solidColorPreset = solidColorPreset;
  eCmd.solidColorR = solidColorR;
  eCmd.solidColorG = solidColorG;
  eCmd.solidColorB = solidColorB;

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
  iCmd.headlight = headlight;
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

void CarControlScreen::triggerSequence(uint8_t seq)
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CAR_CMD_TRIGGER_SEQUENCE;

  TriggerSequenceCmd tCmd = {0};
  tCmd.sequence = seq;

  fp.p.len = sizeof(tCmd);
  memcpy(fp.p.data, &tCmd, sizeof(tCmd));

  wireless.send(&fp);
}

// Sync management methods
void CarControlScreen::syncRefreshData(bool showNotification)
{
  requestSyncStatus();
  requestSyncDevices();
  requestSyncGroups();
  requestSyncGroupInfo();
  requestAutoJoinStatus();
  requestAutoCreateStatus();

  if (showNotification)
  {
    display.showNotification("Refreshing...", 1000);
  }
}

void CarControlScreen::leaveSyncGroup()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_LEAVE_GROUP;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::syncCreateGroup(uint32_t groupId)
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_CREATE_GROUP;

  SyncCreateGroupCmd cmd = {0};
  cmd.groupId = groupId; // 0 = auto-generate

  fp.p.len = sizeof(cmd);
  memcpy(fp.p.data, &cmd, sizeof(cmd));

  wireless.send(&fp);
}

void CarControlScreen::requestSyncDevices()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_GET_DEVICES;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::requestSyncGroups()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_GET_GROUPS;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::requestSyncGroupInfo()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_GET_GROUP_INFO;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::requestSyncStatus()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_GET_STATUS;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::joinSyncGroup(uint32_t groupId)
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_JOIN_GROUP;

  SyncJoinGroupCmd cmd = {0};
  cmd.groupId = groupId;

  fp.p.len = sizeof(cmd);
  memcpy(fp.p.data, &cmd, sizeof(cmd));

  wireless.send(&fp);
}

void CarControlScreen::joinSelectedGroup()
{
  if (selectedGroupId != 0)
  {
    joinSyncGroup(selectedGroupId);
    display.showNotification("Joining group...", 1500);
  }
  else
  {
    display.showNotification("No group selected", 1500);
  }
}

// Auto join/create methods
void CarControlScreen::setAutoJoin(bool enabled)
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_SET_AUTO_JOIN;

  SyncAutoJoinCmd cmd = {0};
  cmd.enabled = enabled;

  fp.p.len = sizeof(cmd);
  memcpy(fp.p.data, &cmd, sizeof(cmd));

  wireless.send(&fp);
}

void CarControlScreen::setAutoCreate(bool enabled)
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_SET_AUTO_CREATE;

  SyncAutoCreateCmd cmd = {0};
  cmd.enabled = enabled;

  fp.p.len = sizeof(cmd);
  memcpy(fp.p.data, &cmd, sizeof(cmd));

  wireless.send(&fp);
}

void CarControlScreen::requestAutoJoinStatus()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_GET_AUTO_JOIN;
  fp.p.len = 0;

  wireless.send(&fp);
}

void CarControlScreen::requestAutoCreateStatus()
{
  fullPacket fp;
  memset(&fp, 0, sizeof(fullPacket));
  fp.direction = PacketDirection::SEND;
  memcpy(fp.mac, led_controller_addrs[led_controller_addr_index], 6);
  fp.p.type = CMD_SYNC_GET_AUTO_CREATE;
  fp.p.len = 0;

  wireless.send(&fp);
}

// UI update methods
void CarControlScreen::updateThisDeviceDisplay()
{
  // Only show valid device info if we're connected
  if (syncStatus.deviceId != 0)
  {
    thisDeviceIdItem.setName("ID: " + String(syncStatus.deviceId));

    if (syncStatus.groupId == 0)
    {
      thisDeviceGroupItem.setName("Group: None");
      thisDeviceStatusItem.setName("Status: Solo");
    }
    else
    {
      thisDeviceGroupItem.setName("Group: " + String(syncStatus.groupId));
      String statusText = syncStatus.isMaster ? "Master" : "Member";
      if (syncStatus.timeSynced)
      {
        statusText += " (Synced)";
      }
      thisDeviceStatusItem.setName("Status: " + statusText);
    }
  }
  else
  {
    // No connection - show default values
    thisDeviceIdItem.setName("ID: ---");
    thisDeviceGroupItem.setName("Group: ---");
    thisDeviceStatusItem.setName("Status: Disconnected");
  }
}

void CarControlScreen::updateCurrentGroupDisplay()
{
  if (syncCurrentGroup.groupId == 0)
  {
    currentGroupIdItem.setName("ID: None");
    currentGroupMasterItem.setName("Master: ---");
    currentGroupMembersItem.setName("Members: 0");
    currentGroupSyncItem.setName("Sync: ---");
    // Hide members submenu when not in a group
    groupMembersUIItem.setHidden(true);
  }
  else
  {
    currentGroupIdItem.setName("ID: " + String(syncCurrentGroup.groupId));
    currentGroupMasterItem.setName("Master: " + String(syncCurrentGroup.masterDeviceId));
    currentGroupMembersItem.setName("Members: " + String(syncCurrentGroup.memberCount));

    String syncText = syncCurrentGroup.timeSynced ? "OK" : "No";
    if (syncCurrentGroup.timeSynced && syncCurrentGroup.timeOffset != 0)
    {
      syncText += " (" + String(syncCurrentGroup.timeOffset) + "ms)";
    }
    currentGroupSyncItem.setName("Sync: " + syncText);
    // Show members submenu only when we have members
    groupMembersUIItem.setHidden(syncCurrentGroup.memberCount == 0);
  }
}

void CarControlScreen::updateDevicesDisplay()
{
  // Update device items - only show populated ones
  for (int i = 0; i < 8; i++)
  {
    if (i < syncDevices.deviceCount)
    {
      SyncDeviceInfo &device = syncDevices.devices[i];
      String deviceText = String(device.deviceId);

      if (device.isThisDevice)
      {
        deviceText += " (Me)";
      }
      else if (device.inCurrentGroup)
      {
        deviceText += device.isGroupMaster ? " (GM)" : " (G)";
      }

      deviceText += " - " + formatTimeDuration(device.timeSinceLastSeen);
      deviceItems[i].setName(deviceText);
      deviceItems[i].setHidden(false);
    }
    else
    {
      // Hide unpopulated items completely
      deviceItems[i].setHidden(true);
    }
  }
}

void CarControlScreen::updateGroupsDisplay()
{
  // Update group items - only show populated ones
  for (int i = 0; i < 4; i++)
  {
    if (i < syncGroups.groupCount)
    {
      SyncGroupInfo &group = syncGroups.groups[i];
      String groupText = "Group " + String(group.groupId);

      if (group.isCurrentGroup)
      {
        groupText += " (Current)";
      }
      else if (group.canJoin)
      {
        groupText += " (Available)";
      }
      else
      {
        groupText += " (Busy)";
      }

      groupText += " - " + formatTimeDuration(group.timeSinceLastSeen);
      groupItems[i].setName(groupText);
      groupItems[i].setHidden(false);
    }
    else
    {
      // Hide unpopulated items completely
      groupItems[i].setHidden(true);
    }
  }
}

void CarControlScreen::updateGroupMembersDisplay()
{
  // Clear existing member items (except back button)
  while (groupMembersMenu.items.size() > 1)
  {
    groupMembersMenu.items.pop_back();
  }

  // Only show member details if we're actually in a group
  if (syncCurrentGroup.groupId == 0 || syncCurrentGroup.memberCount == 0)
  {
    // No group or no members - could add a "No members" info item
    return;
  }

  // Add member items dynamically for current group
  for (int i = 0; i < syncCurrentGroup.memberCount && i < 6; i++)
  {
    SyncGroupMemberInfo &member = syncCurrentGroup.members[i];
    String memberText = "Dev " + String(member.deviceId);

    if (member.isThisDevice)
    {
      memberText += " (Me)";
    }
    if (member.isGroupMaster)
    {
      memberText += " (Master)";
    }

    // Note: Creating dynamic menu items here would require proper memory management
    // For now, we'll keep it simple and just update the display when needed
  }
}

void CarControlScreen::showDeviceDetail(uint32_t deviceId)
{
  selectedDeviceId = deviceId;

  // Find the device in our list
  SyncDeviceInfo *device = nullptr;
  for (uint8_t i = 0; i < syncDevices.deviceCount; i++)
  {
    if (syncDevices.devices[i].deviceId == deviceId)
    {
      device = &syncDevices.devices[i];
      break;
    }
  }

  if (device != nullptr)
  {
    deviceDetailIdItem.setName("ID: " + String(device->deviceId));
    deviceDetailMacItem.setName("MAC: " + formatMacAddress(device->mac));
    deviceDetailLastSeenItem.setName("Last: " + formatTimeDuration(device->timeSinceLastSeen));

    String statusText = "Unknown";
    if (device->isThisDevice)
    {
      statusText = "This Device";
    }
    else if (device->inCurrentGroup)
    {
      statusText = device->isGroupMaster ? "Group Master" : "Group Member";
    }
    else
    {
      statusText = "Other Device";
    }
    deviceDetailStatusItem.setName("Status: " + statusText);

    if (device->inCurrentGroup)
    {
      deviceDetailGroupItem.setName("Group: " + String(syncCurrentGroup.groupId));
    }
    else
    {
      deviceDetailGroupItem.setName("Group: None");
    }

    // Activate the device detail submenu
    syncDevicesMenu.setActiveSubmenu(&deviceDetailMenu);
  }
}

void CarControlScreen::showGroupDetail(uint32_t groupId)
{
  selectedGroupId = groupId;

  // Find the group in our list
  SyncGroupInfo *group = nullptr;
  for (uint8_t i = 0; i < syncGroups.groupCount; i++)
  {
    if (syncGroups.groups[i].groupId == groupId)
    {
      group = &syncGroups.groups[i];
      break;
    }
  }

  if (group != nullptr)
  {
    groupDetailIdItem.setName("ID: " + String(group->groupId));
    groupDetailMasterItem.setName("Master: " + String(group->masterDeviceId));
    groupDetailMacItem.setName("MAC: " + formatMacAddress(group->masterMac));
    groupDetailLastSeenItem.setName("Last: " + formatTimeDuration(group->timeSinceLastSeen));

    String statusText = "Unknown";
    if (group->isCurrentGroup)
    {
      statusText = "Current Group";
    }
    else if (group->canJoin)
    {
      statusText = "Available";
    }
    else
    {
      statusText = "Busy";
    }
    groupDetailStatusItem.setName("Status: " + statusText);

    // Show/hide join button based on availability
    groupJoinItem.setHidden(group->isCurrentGroup || !group->canJoin);

    // Activate the group detail submenu
    syncGroupsMenu.setActiveSubmenu(&groupDetailMenu);
  }
}

String CarControlScreen::formatMacAddress(uint8_t *mac)
{
  String result = "";
  for (int i = 0; i < 6; i++)
  {
    if (i > 0)
      result += ":";
    if (mac[i] < 16)
      result += "0";
    result += String(mac[i], HEX);
  }
  result.toUpperCase();
  return result;
}

String CarControlScreen::formatTimestamp(uint32_t timestamp)
{
  // Convert timestamp to seconds ago
  uint32_t now = millis() / 1000;
  uint32_t ago = now - timestamp;

  if (ago < 60)
    return String(ago) + "s ago";
  else if (ago < 3600)
    return String(ago / 60) + "m ago";
  else
    return String(ago / 3600) + "h ago";
}

String CarControlScreen::formatTimeDuration(uint32_t milliseconds)
{
  if (milliseconds < 1000)
    return String(milliseconds) + "ms";
  else if (milliseconds < 60000)
    return String(milliseconds / 1000) + "s";
  else if (milliseconds < 3600000)
    return String(milliseconds / 60000) + "m";
  else
    return String(milliseconds / 3600000) + "h";
}