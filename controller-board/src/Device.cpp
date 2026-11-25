#include "Device.h"

#include <LittleFS.h>
#include <Preferences.h>
#include <cstddef>

#include "config.h"

#undef LOG_TAG
#define LOG_TAG "DEVICE"
#include <dcr_Logger.h>

namespace
{
  // Leading fields of bioreactor-controller's DeviceInfo (src/config.h), stored as raw
  // bytes under prefs/deviceInfo. Only these are read; the layout must match.
  struct StoredDeviceInfoPrefix
  {
    bool provisioned;
    bool debugEnabled;
    bool firstBoot;
    bool calibrated;
    uint8_t macAddress[6];
    char serialNumber[9];
    char modelNumber[9];
    int hardwareVersion;
    int deviceType;
  };
  static_assert(offsetof(StoredDeviceInfoPrefix, hardwareVersion) == 28, "DeviceInfo layout");
  static_assert(offsetof(StoredDeviceInfoPrefix, deviceType) == 32, "DeviceInfo layout");

  constexpr int kDeviceTypeBioreactor8 = 1; // DeviceType::BIOREACTOR_8
  constexpr char kTouchCalFile[] = "/TouchCalData";

  Device::Info deviceInfo;
  uint32_t powerPressedSinceMs = 0;
  bool powerArmed = false; // the press that switched the board on must be released first

  void readDeviceInfo()
  {
    Preferences prefs;
    if (!prefs.begin("prefs", true)) // read-only; fails if the namespace does not exist
    {
      debugW("No bioreactor NVS data; using defaults");
      return;
    }
    StoredDeviceInfoPrefix stored{};
    const size_t len = prefs.getBytesLength("deviceInfo");
    if (len >= sizeof(stored) && prefs.getBytes("deviceInfo", &stored, sizeof(stored)) == sizeof(stored))
    {
      deviceInfo.found = true;
      deviceInfo.hardwareVersion = stored.hardwareVersion;
      deviceInfo.deviceType = stored.deviceType;
      memcpy(deviceInfo.serial, stored.serialNumber, sizeof(deviceInfo.serial));
      deviceInfo.serial[sizeof(deviceInfo.serial) - 1] = '\0';
    }
    prefs.end();
  }
}

void Device::begin()
{
  pinMode(LATCH_PWR_PIN, OUTPUT);
  digitalWrite(LATCH_PWR_PIN, HIGH);
  pinMode(POWER_BUTTON_PIN, INPUT_PULLDOWN);

  readDeviceInfo();
  // Same choices as bioreactor-controller's setDeviceSettings().
  deviceInfo.rotation = deviceInfo.deviceType == kDeviceTypeBioreactor8 ? 3 : 1;
  deviceInfo.backlightActiveHigh = deviceInfo.hardwareVersion != 2 && deviceInfo.hardwareVersion != 3;

  pinMode(DISPLAY_BACKLIGHT_PIN, OUTPUT);
  setBacklight(false);

  debugI("Device %s: hw v%d, type %d, rotation %u, backlight active %s",
         deviceInfo.found ? deviceInfo.serial : "(unprovisioned)", deviceInfo.hardwareVersion,
         deviceInfo.deviceType, deviceInfo.rotation, deviceInfo.backlightActiveHigh ? "high" : "low");
}

const Device::Info &Device::info()
{
  return deviceInfo;
}

void Device::setBacklight(bool on)
{
  digitalWrite(DISPLAY_BACKLIGHT_PIN, on == deviceInfo.backlightActiveHigh ? HIGH : LOW);
}

bool Device::loadTouchCalibration(uint16_t (&calData)[5])
{
  // formatOnFail = false: a failed mount must never erase the bioreactor's data.
  if (!LittleFS.begin(false))
  {
    debugW("LittleFS not mounted; no saved touch calibration");
    return false;
  }
  bool ok = false;
  File f = LittleFS.open(kTouchCalFile, "r");
  if (f)
  {
    // The bioreactor writes "a,b,c,d,e".
    const String text = f.readString();
    f.close();
    int start = 0;
    int parsed = 0;
    for (; parsed < 5; ++parsed)
    {
      const int comma = text.indexOf(',', start);
      const String field = comma < 0 ? text.substring(start) : text.substring(start, comma);
      if (field.length() == 0)
        break;
      calData[parsed] = static_cast<uint16_t>(field.toInt());
      if (comma < 0)
      {
        ++parsed;
        break;
      }
      start = comma + 1;
    }
    ok = parsed == 5;
  }
  LittleFS.end();
  debugI("Touch calibration %s", ok ? "loaded from the bioreactor" : "not found");
  return ok;
}

void Device::loop()
{
  const bool pressed = digitalRead(POWER_BUTTON_PIN) == HIGH;
  if (!pressed)
  {
    powerArmed = true;
    powerPressedSinceMs = 0;
    return;
  }
  if (!powerArmed)
    return;
  if (powerPressedSinceMs == 0)
    powerPressedSinceMs = millis() | 1;
  if (millis() - powerPressedSinceMs >= POWER_OFF_HOLD_MS)
    powerOff();
}

void Device::powerOff()
{
  debugI("Power off");
  setBacklight(false);
  delay(50);
  digitalWrite(LATCH_PWR_PIN, LOW);
  // On USB power the board stays up; idle until reset.
  for (;;)
    delay(1000);
}
