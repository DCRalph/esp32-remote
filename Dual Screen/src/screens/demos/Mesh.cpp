// wireless_mesh with oled 3: this board hosts a group, oled 3 joins it (its
// default JOIN mode). Each side shares a level and can ping the other. The radio
// is on ESP-NOW only while the demo is open; Wi-Fi reconnects on exit.
#include "screens/Screens.h"

#include <RemoteProtocol.h>
#include <dcr_NetLink.h>

#undef LOG_TAG
#define LOG_TAG "MESH"
#include <dcr_Logger.h>

#include "config.h"
#include "Displays.h"

namespace
{
  namespace Demo = RemoteProtocol::MeshDemo;

  constexpr char kName[] = "Dual Screen";
  constexpr size_t kMaxPeers = 4;
  constexpr uint32_t kRefreshMs = 250;

  SyncManager &mesh = *SyncManager::getInstance();

  int gLevel = 50;
  int sentLevel = -1;
  bool open = false;
  uint32_t lastRefreshMs = 0;
  std::vector<Demo::Peer> peers;

  void ping();

  String groupText, peerText[kMaxPeers];
  Menu menu(MenuSize::Medium);
  MenuItemBack backItem;
  MenuItemString groupItem("Group", &groupText);
  MenuItemNumber<int> levelItem("My level %", &gLevel, 0, 100, 5);
  MenuItemAction pingItem("Ping", 1, ping);
  MenuItemString peerItems[kMaxPeers] = {{"", &peerText[0]}, {"", &peerText[1]}, {"", &peerText[2]}, {"", &peerText[3]}};

  Slider levelSlider("My level", &gLevel, 0, 100, 5);
  Button pingButton("Ping", ping, Button::Style::Primary);
  Button backButton("Back", []()
                    { screenManager.back(); });
  FocusGroup group;
  bool built = false;

  String nameOf(uint32_t deviceId)
  {
    for (const Demo::Peer &p : peers)
      if (p.deviceId == deviceId)
        return p.status.name;
    return String(deviceId, HEX);
  }

  void ping()
  {
    if (!mesh.isInGroup())
    {
      touchScreens.showPopup(new AutoClosePopup("Mesh", "No group yet", 1200));
      return;
    }
    Demo::ping().emit();
  }

  void build()
  {
    built = true;
    for (MenuItem *item : {static_cast<MenuItem *>(&backItem), static_cast<MenuItem *>(&groupItem),
                           static_cast<MenuItem *>(&levelItem), static_cast<MenuItem *>(&pingItem)})
      menu.addMenuItem(item);
    for (MenuItemString &item : peerItems)
    {
      item.setHidden(true);
      menu.addMenuItem(&item);
    }
    levelItem.setFastUpdate(true);

    levelSlider.suffix = "%";
    levelSlider.bounds = {10, 32, TOUCH_WIDTH - 20, 52};
    pingButton.bounds = {10, 92, (TOUCH_WIDTH - 26) / 2, 36};
    backButton.bounds = {static_cast<int16_t>(16 + (TOUCH_WIDTH - 26) / 2), 92, (TOUCH_WIDTH - 26) / 2, 36};
    group.add(levelSlider);
    group.add(pingButton);
    group.add(backButton);

    mesh.setDeviceIdProvider([]()
                             {
                               const uint64_t mac = ESP.getEfuseMac();
                               return static_cast<uint32_t>(mac ^ (mac >> 32)); });
    Demo::ping().onEvent([](uint32_t from)
                         {
                           if (open)
                             touchScreens.showPopup(new AutoClosePopup("Ping", "from " + nameOf(from), 1500)); });
  }

  void refresh()
  {
    peers = Demo::peers();
    groupText = mesh.isInGroup() ? String(peers.size()) + " joined" : String("starting");
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

  void lcdEnter()
  {
    if (!built)
      build();
    netLink.prepareForExternalTransport();
    mesh.setTransport(Wireless::getInstance());
    mesh.begin();
    if (!Wireless::getInstance()->isReady())
      debugE("ESP-NOW did not start");
    mesh.setSyncMode(SyncMode::HOST);
    open = true;
    sentLevel = -1;
    lastRefreshMs = 0;
    touchScreens.setScreen(&Demos::MeshTouch);
  }

  void lcdExit()
  {
    open = false;
    mesh.setSyncMode(SyncMode::SOLO); // leaves the group
    Wireless::getInstance()->end();
    mesh.setTransport(nullptr);
    netLink.on();
    touchScreens.setScreen(&TouchHomeScreen);
  }

  void lcdUpdate()
  {
    // set() only takes while in a group; the host re-announces it to each new member.
    if (mesh.isInGroup() && gLevel != sentLevel)
    {
      Demo::publish(kName, gLevel);
      sentLevel = gLevel;
    }
    if (millis() - lastRefreshMs >= kRefreshMs)
    {
      lastRefreshMs = millis();
      refresh();
    }
    menu.update();
  }

  void touchDraw()
  {
    Display &d = touchDisplay;
    group.draw();
    if (peers.empty())
    {
      drawText(d, mesh.isInGroup() ? "Waiting for oled 3 to join" : "Starting the group", 10, 146, TextRole::Body,
               Datum::TopLeft, d.theme().mutedText);
      return;
    }
    int16_t y = 140;
    for (const Demo::Peer &p : peers)
    {
      drawText(d, p.status.name, 10, y, TextRole::Body);
      drawText(d, String(p.status.level) + "%", TOUCH_WIDTH - 10, y, TextRole::Body, Datum::TopRight);
      d.drawRect(10, y + 20, TOUCH_WIDTH - 20, 8, d.theme().mutedText);
      d.fillRect(11, y + 21, (TOUCH_WIDTH - 22) * p.status.level / 100, 6, d.theme().accent);
      y += 34;
      if (y > TOUCH_HEIGHT - 30)
        break;
    }
  }
}

const Screen2 Demos::MeshLcd = {
    .name = "Mesh",
    .draw = []()
    { menu.draw(); },
    .update = lcdUpdate,
    .onEnter = lcdEnter,
    .onExit = lcdExit,
};

const Screen2 Demos::MeshTouch = makeScreen("Mesh", touchDraw, []()
                                            { group.update(); }, nullptr, nullptr, 250);
