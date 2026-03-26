#include "MeshScreen.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "ScreenManager.h"
#include "IO/Buttons.h"
#include "Mesh.h"
#include "Wireless.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace MeshScreenNamespace
{
  namespace
  {

    static constexpr unsigned long kRefreshMs = 300;
    static constexpr int kMaxPeerAdvertIndex = 63;
    static constexpr size_t kMaxPropsInPopup = 20;

    static SyncManager *syncMgr_ = SyncManager::getInstance();

    static unsigned long lastRefreshMs_ = 0;

    static String sDevice_;
    static String sMode_;
    static String sGroup_;
    static String sTime_;
    static String sRadio_;
    static String sDiscCounts_;
    static String sPropCount_;

    static bool relayUi_ = true;
    static uint32_t joinGroupId_ = 0;
    static int peerIndex_ = 0;
    static int advertIndex_ = 0;

    static std::vector<DiscoveredDevice> peersCached_;
    static std::vector<GroupAdvert> advertsCached_;

    static std::vector<String> modeSelectOptions_ = {"SOLO", "JOIN", "HOST", "AUTO"};

    static Menu menu_{MenuSize::Medium};
    static Menu menuMode_{MenuSize::Medium};
    static Menu menuGroup_{MenuSize::Medium};
    static Menu menuDiscovery_{MenuSize::Medium};
    static Menu menuProps_{MenuSize::Medium};
    static Menu menuTools_{MenuSize::Large};

    static MenuItemBack rootBack_;
    static MenuItemString strDevice_{"Device", &sDevice_};
    static MenuItemString strMode_{"Sync", &sMode_};
    static MenuItemString strGroup_{"Group", &sGroup_};
    static MenuItemString strTime_{"Time", &sTime_};
    static MenuItemString strRadio_{"Radio", &sRadio_};
    static MenuItemSubmenu subMode_{"Mode & relay", &menuMode_};
    static MenuItemSubmenu subGroup_{"Group", &menuGroup_};
    static MenuItemSubmenu subDiscovery_{"Discovery", &menuDiscovery_};
    static MenuItemSubmenu subProps_{"Properties", &menuProps_};
    static MenuItemSubmenu subTools_{"Tools", &menuTools_};

    static MenuItemBack modeBack_;
    static MenuItemSelect modeSelect_{"Sync mode", modeSelectOptions_, 0};
    static MenuItemToggle relayToggle_{"Mesh relay", &relayUi_};

    static MenuItemBack groupBack_;
    static std::unique_ptr<MenuItemAction> groupCreate_;
    static MenuItemNumber<uint32_t> joinIdNum_{"Join grp ID (dec)", &joinGroupId_, 0u, 0xFFFFFFFFu, 1u};
    static std::unique_ptr<MenuItemAction> groupJoinId_;
    static std::unique_ptr<MenuItemAction> groupJoinFirst_;
    static std::unique_ptr<MenuItemAction> groupLeave_;
    static std::unique_ptr<MenuItemAction> groupTimeSync_;

    static MenuItemBack discBack_;
    static MenuItemString strDiscCounts_{"Counts", &sDiscCounts_};
    static MenuItemNumber<int> peerIdx_{"Peer index", &peerIndex_, 0, kMaxPeerAdvertIndex};
    static std::unique_ptr<MenuItemAction> peerDetails_;
    static MenuItemNumber<int> advertIdx_{"Advert index", &advertIndex_, 0, kMaxPeerAdvertIndex};
    static std::unique_ptr<MenuItemAction> advertDetails_;

    static MenuItemBack propsBack_;
    static MenuItemString strPropCount_{"Property count", &sPropCount_};
    static std::unique_ptr<MenuItemAction> propsSummary_;

    static MenuItemBack toolsBack_;
    static std::unique_ptr<MenuItemAction> toolHeartbeat_;
    static std::unique_ptr<MenuItemAction> toolAnnounce_;
    static std::unique_ptr<MenuItemAction> toolGroupInfo_;
    static std::unique_ptr<MenuItemAction> toolDump_;

    static bool menusWired_ = false;

    static String fmtHex32(uint32_t v)
    {
      String s = String(v, HEX);
      s.toUpperCase();
      return "0x" + s;
    }

    static String fmtMac(const uint8_t mac[6])
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

    static String ageMs(uint32_t lastSeenMs)
    {
      const uint32_t now = millis();
      if (now >= lastSeenMs)
        return String(now - lastSeenMs) + "ms ago";
      return "just now";
    }

    static bool comparePeerId(const DiscoveredDevice &a, const DiscoveredDevice &b)
    {
      return a.deviceId < b.deviceId;
    }

    static void wireActions();

    static void rebuildPeerAdvertCaches()
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

    static void refreshStatus()
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
      {
        sTime_ = "Time not locked";
        strTime_.setBgColor(TFT_RED);
        strTime_.setTextColor(TFT_RED);
      }
      else
      {
        sTime_ = "Off " + String(syncMgr_->getTimeOffset()) + "ms";
        sTime_ += " t" + String(syncMgr_->getSyncedTime());

        strTime_.setBgColor(TFT_GREEN);
        strTime_.setTextColor(TFT_GREEN);
      }

      sRadio_ = wireless.isSetupDone() ? "ESP-NOW ready" : "ESP-NOW off";

      sDiscCounts_ = "P" + String(peersCached_.size()) + " G" + String(advertsCached_.size());

      sPropCount_ = "Props " + String(syncMgr_->getDiscoveredProperties().size());
    }

    static void wireActions()
    {
      modeSelect_.setOnChange([]()
                              { syncMgr_->setSyncMode(static_cast<SyncMode>(modeSelect_.getCurrentIndex())); });

      relayToggle_.setOnChange([]()
                               { syncMgr_->setRelayEnabled(relayUi_); });
    }

    static void ensureMenusWired()
    {
      if (menusWired_)
        return;
      menusWired_ = true;

      groupCreate_ = std::make_unique<MenuItemAction>("Create group (auto ID)", 2, []()
                                                      {
                                                    syncMgr_->createGroup(0);
                                                    screenManager.showPopup(new AutoClosePopup("Mesh", "createGroup sent", 1200)); });
      groupJoinId_ = std::make_unique<MenuItemAction>("Join entered ID", 2, []()
                                                      {
                                                    syncMgr_->joinGroup(joinGroupId_);
                                                    screenManager.showPopup(
                                                        new AutoClosePopup("Mesh", "joinGroup " + String(joinGroupId_), 1200)); });
      groupJoinFirst_ = std::make_unique<MenuItemAction>("Join first advertised", 2, []()
                                                         {
                                                       auto g = syncMgr_->getDiscoveredGroups();
                                                       if (g.empty())
                                                       {
                                                         screenManager.showPopup(new AutoClosePopup("Mesh", "No advertised groups", 1500));
                                                         return;
                                                       }
                                                       syncMgr_->joinGroup(g[0].groupId);
                                                       screenManager.showPopup(new AutoClosePopup(
                                                           "Mesh", "Joining " + fmtHex32(g[0].groupId), 1500)); });
      groupLeave_ = std::make_unique<MenuItemAction>("Leave group", 2, []()
                                                     {
                                                   syncMgr_->leaveGroup();
                                                   screenManager.showPopup(new AutoClosePopup("Mesh", "leaveGroup", 1200)); });
      groupTimeSync_ = std::make_unique<MenuItemAction>("Request time sync", 2, []()
                                                        {
                                                      syncMgr_->requestTimeSync();
                                                      screenManager.showPopup(new AutoClosePopup("Mesh", "Time sync requested", 1200)); });
      peerDetails_ = std::make_unique<MenuItemAction>("Peer details", 2, []()
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
                                                    screenManager.showPopup(new Popup("Peer", msg)); });
      advertDetails_ = std::make_unique<MenuItemAction>("Advert details", 2, []()
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
                                                       screenManager.showPopup(new Popup("Advert", msg)); });
      propsSummary_ = std::make_unique<MenuItemAction>("Summary popup", 2, []()
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
                                                     screenManager.showPopup(new Popup("Properties", msg)); });
      toolHeartbeat_ = std::make_unique<MenuItemAction>("Send heartbeat", 2, []()
                                                        {
                                                    syncMgr_->sendHeartbeat();
                                                    screenManager.showPopup(new AutoClosePopup("Mesh", "Heartbeat", 800)); });
      toolAnnounce_ = std::make_unique<MenuItemAction>("Send group announce", 2, []()
                                                       {
                                                     syncMgr_->sendGroupAnnounce();
                                                     screenManager.showPopup(new AutoClosePopup("Mesh", "Announce", 800)); });
      toolGroupInfo_ = std::make_unique<MenuItemAction>("Send group info", 2, []()
                                                        {
                                                      syncMgr_->sendGroupInfo();
                                                      screenManager.showPopup(new AutoClosePopup("Mesh", "Group info", 800)); });
      toolDump_ = std::make_unique<MenuItemAction>("Dump state to Serial", 2, []()
                                                   {
                                                 syncMgr_->printDeviceInfo();
                                                 syncMgr_->printGroupInfo();
                                                 syncMgr_->printSyncModeInfo();
                                                 screenManager.showPopup(new AutoClosePopup("Mesh", "Serial dump done", 1200)); });

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
      menuGroup_.addMenuItem(groupCreate_.get());
      menuGroup_.addMenuItem(&joinIdNum_);
      menuGroup_.addMenuItem(groupJoinId_.get());
      menuGroup_.addMenuItem(groupJoinFirst_.get());
      menuGroup_.addMenuItem(groupLeave_.get());
      menuGroup_.addMenuItem(groupTimeSync_.get());

      menuDiscovery_.addMenuItem(&discBack_);
      menuDiscovery_.addMenuItem(&strDiscCounts_);
      menuDiscovery_.addMenuItem(&peerIdx_);
      menuDiscovery_.addMenuItem(peerDetails_.get());
      menuDiscovery_.addMenuItem(&advertIdx_);
      menuDiscovery_.addMenuItem(advertDetails_.get());

      menuProps_.addMenuItem(&propsBack_);
      menuProps_.addMenuItem(&strPropCount_);
      menuProps_.addMenuItem(propsSummary_.get());

      menuTools_.addMenuItem(&toolsBack_);
      menuTools_.addMenuItem(toolHeartbeat_.get());
      menuTools_.addMenuItem(toolAnnounce_.get());
      menuTools_.addMenuItem(toolGroupInfo_.get());
      menuTools_.addMenuItem(toolDump_.get());

      wireActions();
    }

    static void meshOnEnter()
    {
      ensureMenusWired();
      relayUi_ = syncMgr_->isRelayEnabled();
      modeSelect_.setCurrentIndex(static_cast<int>(syncMgr_->getSyncMode()));
      lastRefreshMs_ = 0;
      rebuildPeerAdvertCaches();
      refreshStatus();
    }

    static void meshDraw()
    {
      ensureMenusWired();
      menu_.draw();
    }

    static void meshUpdate()
    {
      ensureMenusWired();
      const unsigned long now = millis();
      if (now - lastRefreshMs_ >= kRefreshMs)
      {
        lastRefreshMs_ = now;
        rebuildPeerAdvertCaches();
        refreshStatus();
      }
      menu_.update();
    }

  } // namespace

  void draw()
  {
    meshDraw();
  }
  void update()
  {
    meshUpdate();
  }
  void onEnter()
  {
    meshOnEnter();
  }

} // namespace MeshScreenNamespace

const Screen2 MeshScreen = {
    .name = "Mesh",
    .draw = MeshScreenNamespace::draw,
    .update = MeshScreenNamespace::update,
    .onEnter = MeshScreenNamespace::onEnter,
    .onExit = nullptr,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
