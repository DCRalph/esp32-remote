## Display abstraction usage

1) Create a driver instance and initialize the shared `display` facade:
```cpp
#include "display/Display.h"
#include "display/TFTDisplayDriver.h"
#include "display/ScreenManager.h"

TFTDisplayDriver displayDriver;

void setup() {
  display.begin(&displayDriver, &screenManager);
  screenManager.init(display);
  // add screens, set the starting screen, etc.
}
```

2) Use the facade in screens and menus instead of raw sprite calls:
```cpp
display.clearScreen();
display.setTextSize(2);
display.setTextDatum(TC_DATUM);
display.setTextColor(TFT_WHITE);
display.drawString("Hello", LCD_WIDTH / 2, LCD_HEIGHT / 2);
display.render(); // clears, draws the current screen, handles popups, and pushes
```

`display` exposes common operations (text, rectangles, lines, colors, push) and wraps whichever driver you provide. The default driver in `TFTDisplayDriver` keeps the existing TFT_eSPI sprite-backed rendering.
