#include "config.h"

Preferences preferences;
HardwareSerial Copro(2);

bool globalRelay1 = false;
bool globalRelay2 = false;
bool globalRelay3 = false;
bool globalRelay4 = false;
bool globalRelay5 = false;
bool globalRelay6 = false;

void initConfig()
{
  Serial.begin(BAUD_RATE);
  Copro.begin(BAUD_RATE, SERIAL_8N1, 1, 2);

  delay(10);
  
  Serial.flush();
  Copro.flush();

  while (Copro.available())
  {
    uint8_t c = Copro.read();
    Serial.println("extra data: " + String(c, HEX));
  }

  Serial.println("[INFO] [CONFIG] Starting...");

  Serial.println("[INFO] [CONFIG] Preferences...");
  preferences.begin("p", false);

  long bootCount = preferences.getLong("bootCount", 0);
  bootCount++;
  preferences.putLong("bootCount", bootCount);
}