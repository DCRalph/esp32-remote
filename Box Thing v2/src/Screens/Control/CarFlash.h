// #pragma once

// #include "config.h"
// #include "IO/Display.h"
// #include "IO/GPIO.h"
// // #include "IO/myespnow.h"
// #include "IO/Wireless.h"

// class CarFlashScreen : public Screen
// {
// public:
//   CarFlashScreen(String _name);

//   Menu menu = Menu();

//   MenuItemBack backItem;

//   uint8_t count = 10;
//   uint8_t delay = 50;

//   MenuItemAction relay1FlashItem = MenuItemAction("Flash R 1", 1, [&]()
//                                                   {
//                                                     fullPacket fp;
//                                                     memcpy(fp.mac, remote_addr, 6);
//                                                     fp.direction == PacketDirection::SEND;
//                                                     fp.p.type = CMD_RELAY_1_FLASH;
//                                                     fp.p.len = 2;
//                                                     fp.p.data[0] = count;      // count
//                                                     fp.p.data[1] = delay / 10; // delay in ms * 10

//                                                     wireless.send(&fp);
//                                                     //
//                                                   });

//   MenuItemNumber<uint8_t> relay1FlashCountItem = MenuItemNumber<uint8_t>("Count", &count, 1, 100);
//   MenuItemNumber<uint8_t> relay1FlashDelayItem = MenuItemNumber<uint8_t>("Delay", &delay, 20, 250, 10);

//   void draw() override;
//   void update() override;
//   void onEnter() override;
// };

// CarFlashScreen::CarFlashScreen(String _name) : Screen(_name)
// {
//   menu.addMenuItem(&backItem);
//   menu.addMenuItem(&relay1FlashItem);
//   menu.addMenuItem(&relay1FlashCountItem);
//   menu.addMenuItem(&relay1FlashDelayItem);
// }

// void CarFlashScreen::draw()
// {
//   menu.draw();
// }

// void CarFlashScreen::update()
// {
//   menu.update();
// }

// void CarFlashScreen::onEnter()
// {
// }