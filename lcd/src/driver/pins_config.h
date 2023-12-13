#pragma once

/***********************config*************************/

#define TFT_ROT              0

#if TFT_ROT == 0 || TFT_ROT == 2
#define LCD_WIDTH             170
#define LCD_HEIGHT            320
#elif TFT_ROT == 1 || TFT_ROT == 3
#define LCD_WIDTH             320
#define LCD_HEIGHT            170
#endif

// #define TFT_WIDTH             240
// #define TFT_HEIGHT            536
// #define TFT_WIDTH             536
// #define TFT_HEIGHT            240

// #define SEND_BUF_SIZE         (0x4000) //(LCD_WIDTH * LCD_HEIGHT + 8) / 10

