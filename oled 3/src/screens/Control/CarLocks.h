// #pragma once

// #include "config.h"
// #include "IO/Display.h"
// #include "IO/Buttons.h"
// #include "IO/Wireless.h"
// #include "IO/Popup.h"

// class CarLocksScreen : public Screen
// {
// public:
//   CarLocksScreen(String _name);

//   void draw() override;
//   void update() override;
// };

// CarLocksScreen::CarLocksScreen(String _name) : Screen(_name)
// {
// }

// void CarLocksScreen::draw()
// {

//   display.sprite.setTextDatum(TL_DATUM);
//   display.sprite.setTextColor(TFT_WHITE);
//   display.sprite.setTextSize(4);
  
//   display.sprite.drawString("Lock", 90, LCD_HEIGHT - 80);

//   display.sprite.drawString("Unlock", 280, LCD_HEIGHT - 80);
// }

// void CarLocksScreen::update()
// {

//   if (ClickButtonUP.clicks == -1)
//   {
//     data_packet p;
//     p.type = 0x10; // car locks
//     p.len = 1;
//     p.data[0] = 0; // lock

//     wireless.send(&p, car_addr);
//   }

//   if (ClickButtonDOWN.clicks == -1)
//   {
//     data_packet p;
//     p.type = 0x10; // car locks
//     p.len = 1;
//     p.data[0] = 1; // unlock

//     wireless.send(&p, car_addr);
//   }

//   if (ClickButtonUP.clicks == 1 || ClickButtonDOWN.clicks == 1)
//     screenManager.back();
// }