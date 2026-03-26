#pragma once

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"
#include "Mesh.h"
#include "Wireless.h"

#include <algorithm>
#include <utility>
#include <vector>

class MeshScreen : public Screen
{
public:
  explicit MeshScreen(String name);

  void draw() override;
  void update() override;
  void onEnter() override;

private:
  static constexpr unsigned long kRefreshMs = 300;
  static constexpr int kMaxPeerAdvertIndex = 63;
  static constexpr size_t kMaxPropsInPopup = 20;

  SyncManager *syncMgr_;

  unsigned long lastRefreshMs_ = 0;

  String sDevice_;
  String sMode_;
  String sGroup_;
  String sTime_;
  String sRadio_;
  String sDiscCounts_;
  String sPropCount_;

  bool relayUi_ = true;
  uint32_t joinGroupId_ = 0;
  int peerIndex_ = 0;
  int advertIndex_ = 0;

  std::vector<DiscoveredDevice> peersCached_;
  std::vector<GroupAdvert> advertsCached_;

  std::vector<String> modeSelectOptions_ = {"SOLO", "JOIN", "HOST", "AUTO"};

  Menu menu_{MenuSize::Medium};
  Menu menuMode_{MenuSize::Medium};
  Menu menuGroup_{MenuSize::Medium};
  Menu menuDiscovery_{MenuSize::Medium};
  Menu menuProps_{MenuSize::Medium};
  Menu menuTools_{MenuSize::Large};

  MenuItemBack rootBack_;
  MenuItemString strDevice_{"Device", &sDevice_};
  MenuItemString strMode_{"Sync", &sMode_};
  MenuItemString strGroup_{"Group", &sGroup_};
  MenuItemString strTime_{"Time", &sTime_};
  MenuItemString strRadio_{"Radio", &sRadio_};
  MenuItemSubmenu subMode_{"Mode & relay", &menuMode_};
  MenuItemSubmenu subGroup_{"Group", &menuGroup_};
  MenuItemSubmenu subDiscovery_{"Discovery", &menuDiscovery_};
  MenuItemSubmenu subProps_{"Properties", &menuProps_};
  MenuItemSubmenu subTools_{"Tools", &menuTools_};

  MenuItemBack modeBack_;
  MenuItemSelect modeSelect_{"Sync mode", modeSelectOptions_, 0};
  MenuItemToggle relayToggle_{"Mesh relay", &relayUi_};

  MenuItemBack groupBack_;
  MenuItemAction groupCreate_;
  MenuItemNumber<uint32_t> joinIdNum_{"Join grp ID (dec)", &joinGroupId_, 0u, 0xFFFFFFFFu, 1u};
  MenuItemAction groupJoinId_;
  MenuItemAction groupJoinFirst_;
  MenuItemAction groupLeave_;
  MenuItemAction groupTimeSync_;

  MenuItemBack discBack_;
  MenuItemString strDiscCounts_{"Counts", &sDiscCounts_};
  MenuItemNumber<int> peerIdx_{"Peer index", &peerIndex_, 0, kMaxPeerAdvertIndex};
  MenuItemAction peerDetails_;
  MenuItemNumber<int> advertIdx_{"Advert index", &advertIndex_, 0, kMaxPeerAdvertIndex};
  MenuItemAction advertDetails_;

  MenuItemBack propsBack_;
  MenuItemString strPropCount_{"Property count", &sPropCount_};
  MenuItemAction propsSummary_;

  MenuItemBack toolsBack_;
  MenuItemAction toolHeartbeat_;
  MenuItemAction toolAnnounce_;
  MenuItemAction toolGroupInfo_;
  MenuItemAction toolDump_;

  static String fmtHex32(uint32_t v);
  static String fmtMac(const uint8_t mac[6]);
  static String ageMs(uint32_t lastSeenMs);
  static bool comparePeerId(const DiscoveredDevice &a, const DiscoveredDevice &b);

  void rebuildPeerAdvertCaches();
  void refreshStatus();
  void wireActions();
};

inline String MeshScreen::fmtHex32(uint32_t v)
{
  String s = String(v, HEX);
  s.toUpperCase();
  return "0x" + s;
}

inline String MeshScreen::fmtMac(const uint8_t mac[6])
{
  String macStr;
  for (int j = 0; j < 6; j++)
  {
    if (mac[j] < 16)
      macStr += "0";
    macStr += String(mac[j], HEX);
    if (j < 5)
      macStr += ":";
  }
  macStr.toUpperCase();
  return macStr;
}

inline String MeshScreen::ageMs(uint32_t lastSeenMs)
{
  const uint32_t now = millis();
  if (now >= lastSeenMs)
    return String(now - lastSeenMs) + "ms ago";
  return "just now";
}

inline bool MeshScreen::comparePeerId(const DiscoveredDevice &a, const DiscoveredDevice &b)
{
  return a.deviceId < b.deviceId;
}

inline MeshScreen::MeshScreen(String name)
    : Screen(std::move(name)),
      syncMgr_(SyncManager::getInstance()),
      groupCreate_{"Create group (auto ID)", 2, [this]()
                   {
                     syncMgr_->createGroup(0);
                     screenManager.showPopup(new AutoClosePopup("Mesh", "createGroup sent", 1200));
                   }},
      groupJoinId_{"Join entered ID", 2, [this]()
                   {
                     syncMgr_->joinGroup(joinGroupId_);
                     screenManager.showPopup(
                         new AutoClosePopup("Mesh", "joinGroup " + String(joinGroupId_), 1200));
                   }},
      groupJoinFirst_{"Join first advertised", 2, [this]()
                      {
                        auto g = syncMgr_->getDiscoveredGroups();
                        if (g.empty())
                        {
                          screenManager.showPopup(new AutoClosePopup("Mesh", "No advertised groups", 1500));
                          return;
                        }
                        syncMgr_->joinGroup(g[0].groupId);
                        screenManager.showPopup(new AutoClosePopup(
                            "Mesh", "Joining " + fmtHex32(g[0].groupId), 1500));
                      }},
      groupLeave_{"Leave group", 2, [this]()
                  {
                    syncMgr_->leaveGroup();
                    screenManager.showPopup(new AutoClosePopup("Mesh", "leaveGroup", 1200));
                  }},
      groupTimeSync_{"Request time sync", 2, [this]()
                     {
                       syncMgr_->requestTimeSync();
                       screenManager.showPopup(new AutoClosePopup("Mesh", "Time sync requested", 1200));
                     }},
      peerDetails_{"Peer details", 2, [this]()
                   {
                     if (peersCached_.empty())
                     {
                       screenManager.showPopup(new AutoClosePopup("Mesh", "No peers", 1200));
                       return;
                     }
                     int i = peerIndex_;
                     if (i < 0 || i >= (int)peersCached_.size())
                       i = 0;
                     const DiscoveredDevice &d = peersCached_[(size_t)i];
                     String msg = "ID " + fmtHex32(d.deviceId) + "\n";
                     msg += "MAC " + fmtMac(d.mac) + "\n";
                     msg += syncMgr_->getSyncModeString(d.syncMode) + "\n";
                     msg += ageMs(d.lastSeen);
                     screenManager.showPopup(new Popup("Peer", msg));
                   }},
      advertDetails_{"Advert details", 2, [this]()
                     {
                       if (advertsCached_.empty())
                       {
                         screenManager.showPopup(new AutoClosePopup("Mesh", "No adverts", 1200));
                         return;
                       }
                       int i = advertIndex_;
                       if (i < 0 || i >= (int)advertsCached_.size())
                         i = 0;
                       const GroupAdvert &g = advertsCached_[(size_t)i];
                       String msg = "Grp " + fmtHex32(g.groupId) + "\n";
                       msg += "Master " + fmtHex32(g.masterDeviceId) + "\n";
                       msg += "MAC " + fmtMac(g.masterMac) + "\n";
                       msg += ageMs(g.lastSeen);
                       screenManager.showPopup(new Popup("Advert", msg));
                     }},
      propsSummary_{"Summary popup", 2, [this]()
                    {
                      auto props = syncMgr_->getDiscoveredProperties();
                      if (props.empty())
                      {
                        screenManager.showPopup(new AutoClosePopup("Mesh", "No properties", 1200));
                        return;
                      }
                      String msg;
                      const size_t n = std::min(props.size(), kMaxPropsInPopup);
                      for (size_t i = 0; i < n; i++)
                      {
                        const PropertySnapshot &p = props[i];
                        msg += String(p.key, HEX) + "@" + fmtHex32(p.ownerDeviceId);
                        msg += " r" + String(p.revision) + " l" + String(p.payload.size());
                        if (i + 1 < n)
                          msg += "\n";
                      }
                      if (props.size() > kMaxPropsInPopup)
                        msg += "\n+" + String(props.size() - kMaxPropsInPopup) + " more";
                      screenManager.showPopup(new Popup("Properties", msg));
                    }},
      toolHeartbeat_{"Send heartbeat", 2, [this]()
                     {
                       syncMgr_->sendHeartbeat();
                       screenManager.showPopup(new AutoClosePopup("Mesh", "Heartbeat", 800));
                     }},
      toolAnnounce_{"Send group announce", 2, [this]()
                    {
                      syncMgr_->sendGroupAnnounce();
                      screenManager.showPopup(new AutoClosePopup("Mesh", "Announce", 800));
                    }},
      toolGroupInfo_{"Send group info", 2, [this]()
                     {
                       syncMgr_->sendGroupInfo();
                       screenManager.showPopup(new AutoClosePopup("Mesh", "Group info", 800));
                     }},
      toolDump_{"Dump state to Serial", 2, [this]()
                {
                  syncMgr_->printDeviceInfo();
                  syncMgr_->printGroupInfo();
                  syncMgr_->printSyncModeInfo();
                  screenManager.showPopup(new AutoClosePopup("Mesh", "Serial dump done", 1200));
                }}
{

  strTime_.setBgColor(TFT_RED);
  strTime_.setTextColor(TFT_RED);
  strTime_.setActiveTextColor(TFT_BLUE);

  menu_.addMenuItem(&rootBack_);
  menu_.addMenuItem(&strDevice_);
  menu_.addMenuItem(&strMode_);
  menu_.addMenuItem(&strGroup_);
  menu_.addMenuItem(&strTime_);
  menu_.addMenuItem(&strRadio_);
  menu_.addMenuItem(&subMode_);
  menu_.addMenuItem(&subGroup_);
  menu_.addMenuItem(&subDiscovery_);
  menu_.addMenuItem(&subProps_);
  menu_.addMenuItem(&subTools_);

  menuMode_.addMenuItem(&modeBack_);
  menuMode_.addMenuItem(&modeSelect_);
  menuMode_.addMenuItem(&relayToggle_);

  menuGroup_.addMenuItem(&groupBack_);
  menuGroup_.addMenuItem(&groupCreate_);
  menuGroup_.addMenuItem(&joinIdNum_);
  menuGroup_.addMenuItem(&groupJoinId_);
  menuGroup_.addMenuItem(&groupJoinFirst_);
  menuGroup_.addMenuItem(&groupLeave_);
  menuGroup_.addMenuItem(&groupTimeSync_);

  menuDiscovery_.addMenuItem(&discBack_);
  menuDiscovery_.addMenuItem(&strDiscCounts_);
  menuDiscovery_.addMenuItem(&peerIdx_);
  menuDiscovery_.addMenuItem(&peerDetails_);
  menuDiscovery_.addMenuItem(&advertIdx_);
  menuDiscovery_.addMenuItem(&advertDetails_);

  menuProps_.addMenuItem(&propsBack_);
  menuProps_.addMenuItem(&strPropCount_);
  menuProps_.addMenuItem(&propsSummary_);

  menuTools_.addMenuItem(&toolsBack_);
  menuTools_.addMenuItem(&toolHeartbeat_);
  menuTools_.addMenuItem(&toolAnnounce_);
  menuTools_.addMenuItem(&toolGroupInfo_);
  menuTools_.addMenuItem(&toolDump_);

  wireActions();
}

inline void MeshScreen::wireActions()
{
  modeSelect_.setOnChange([this]()
                          { syncMgr_->setSyncMode(static_cast<SyncMode>(modeSelect_.getCurrentIndex())); });

  relayToggle_.setOnChange([this]()
                           { syncMgr_->setRelayEnabled(relayUi_); });
}

inline void MeshScreen::rebuildPeerAdvertCaches()
{
  peersCached_.clear();
  const auto &devMap = syncMgr_->getDiscoveredDevices();
  peersCached_.reserve(devMap.size());
  for (const auto &kv : devMap)
    peersCached_.push_back(kv.second);
  std::sort(peersCached_.begin(), peersCached_.end(), comparePeerId);

  advertsCached_ = syncMgr_->getDiscoveredGroups();
  std::sort(advertsCached_.begin(), advertsCached_.end(),
            [](const GroupAdvert &a, const GroupAdvert &b)
            { return a.groupId < b.groupId; });

  if (peersCached_.empty())
    peerIndex_ = 0;
  else if (peerIndex_ >= (int)peersCached_.size())
    peerIndex_ = (int)peersCached_.size() - 1;

  if (advertsCached_.empty())
    advertIndex_ = 0;
  else if (advertIndex_ >= (int)advertsCached_.size())
    advertIndex_ = (int)advertsCached_.size() - 1;
}

inline void MeshScreen::refreshStatus()
{
  sDevice_ = "Dev " + fmtHex32(syncMgr_->getDeviceId());

  sMode_ = syncMgr_->getSyncModeString();
  if (syncMgr_->isRelayEnabled())
    sMode_ += " RLY";
  else
    sMode_ += " noRLY";

  if (!syncMgr_->isInGroup())
    sGroup_ = "No group";
  else
  {
    const GroupInfo &g = syncMgr_->getGroupInfo();
    sGroup_ = fmtHex32(g.groupId) + " ";
    sGroup_ += syncMgr_->isGroupMaster() ? "M" : "S";
    sGroup_ += " m" + fmtHex32(g.masterDeviceId);
    sGroup_ += " n" + String(g.members.size());
  }

  if (!syncMgr_->isTimeSynced())
    sTime_ = "Time not locked";
  else
  {
    sTime_ = "Off " + String(syncMgr_->getTimeOffset()) + "ms";
    sTime_ += " t" + String(syncMgr_->getSyncedTime());
  }

  sRadio_ = wireless.isSetupDone() ? "ESP-NOW ready" : "ESP-NOW off";

  sDiscCounts_ = "P" + String(peersCached_.size()) + " G" + String(advertsCached_.size());

  sPropCount_ = "Props " + String(syncMgr_->getDiscoveredProperties().size());
}

inline void MeshScreen::onEnter()
{
  relayUi_ = syncMgr_->isRelayEnabled();
  modeSelect_.setCurrentIndex(static_cast<int>(syncMgr_->getSyncMode()));
  lastRefreshMs_ = 0;
  rebuildPeerAdvertCaches();
  refreshStatus();
}

inline void MeshScreen::draw()
{
  menu_.draw();
}

inline void MeshScreen::update()
{
  const unsigned long now = millis();
  if (now - lastRefreshMs_ >= kRefreshMs)
  {
    lastRefreshMs_ = now;
    rebuildPeerAdvertCaches();
    refreshStatus();
  }
  menu_.update();
}
