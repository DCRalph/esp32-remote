// wireless_mesh with Dual Screen: it hosts a group and this board joins it in its
// default JOIN mode (Settings > Mesh). Each side shares a level and can ping the other.
#include "MeshDemo.h"

#include "config.h"
#include "Display.h"
#include "Menu.h"
#include "Popup.h"
#include "IO/Buttons.h"

namespace MeshDemoScreenNamespace
{
  namespace
  {
    namespace Demo = RemoteProtocol::MeshDemo;

    constexpr char kName[] = "oled 3";
    constexpr size_t kMaxPeers = 4;
    constexpr unsigned long kRefreshMs = 250;

    SyncManager &mesh = *SyncManager::getInstance();

    int level = 50;
    int sentLevel = -1;
    bool open = false;
    unsigned long lastRefreshMs = 0;
    std::vector<Demo::Peer> peers;

    String groupText, peerText[kMaxPeers];
    Menu menu{MenuSize::Medium};
    MenuItemBack backItem;
    MenuItemString groupItem{"Group", &groupText};
    MenuItemNumber<int> levelItem{"My level %", &level, 0, 100, 5};
    MenuItemAction pingItem{"Ping", 2, []()
                            {
                              if (mesh.isInGroup())
                                Demo::ping().emit();
                              else
                                screenManager.showPopup(new AutoClosePopup("Mesh", "No group yet", 1200));
                            }};
    MenuItemString peerItems[kMaxPeers] = {{"", &peerText[0]}, {"", &peerText[1]}, {"", &peerText[2]}, {"", &peerText[3]}};
    bool built = false;

    String nameOf(uint32_t deviceId)
    {
      for (const Demo::Peer &p : peers)
        if (p.deviceId == deviceId)
          return p.status.name;
      return String(deviceId, HEX);
    }

    void build()
    {
      built = true;
      menu.addMenuItem(&backItem);
      menu.addMenuItem(&groupItem);
      menu.addMenuItem(&levelItem);
      menu.addMenuItem(&pingItem);
      for (MenuItemString &item : peerItems)
      {
        item.setHidden(true);
        menu.addMenuItem(&item);
      }
      levelItem.setFastUpdate(true);

      Demo::ping().onEvent([](uint32_t from)
                           {
                             if (open)
                               screenManager.showPopup(new AutoClosePopup("Ping", "from " + nameOf(from), 1500)); });
    }

    void refresh()
    {
      peers = Demo::peers();
      if (mesh.isInGroup())
        groupText = String(peers.size()) + (peers.size() == 1 ? " peer" : " peers");
      else if (mesh.getSyncMode() == SyncMode::JOIN || mesh.getSyncMode() == SyncMode::AUTO)
        groupText = "searching";
      else
        groupText = "set Sync to JOIN";

      for (size_t i = 0; i < kMaxPeers; i++)
      {
        peerItems[i].setHidden(i >= peers.size());
        if (i < peers.size())
        {
          peerItems[i].setName(peers[i].status.name);
          peerText[i] = String(peers[i].status.level) + "%";
        }
      }
    }
  }

  void onEnter()
  {
    if (!built)
      build();
    open = true;
    sentLevel = -1;
    lastRefreshMs = 0;
  }

  void onExit()
  {
    open = false;
  }

  void update()
  {
    // set() only takes while in a group; rejoining re-announces it.
    if (mesh.isInGroup() && level != sentLevel)
    {
      Demo::publish(kName, level);
      sentLevel = level;
    }
    if (millis() - lastRefreshMs >= kRefreshMs)
    {
      lastRefreshMs = millis();
      refresh();
    }
    menu.update();
  }

  void draw()
  {
    menu.draw();
  }
} // namespace MeshDemoScreenNamespace

const Screen2 MeshDemoScreen = {
    .name = "Mesh demo",
    .draw = MeshDemoScreenNamespace::draw,
    .update = MeshDemoScreenNamespace::update,
    .onEnter = MeshDemoScreenNamespace::onEnter,
    .onExit = MeshDemoScreenNamespace::onExit,
    .topBarColor = TFT_BLUE,
    .topBarTextColor = TFT_BLACK,
};
