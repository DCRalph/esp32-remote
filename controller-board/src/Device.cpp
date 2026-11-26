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

  // 20 kHz is above hearing and flicker; 8 bits is plenty of steps for a slider.
  constexpr uint8_t kBacklightChannel = 0;
  constexpr uint32_t kBacklightHz = 20000;
  constexpr uint8_t kBacklightBits = 8;
  bool backlightOn = false;
  uint8_t backlightPercent = 100;

  void readDeviceInfo()
  {
    Preferences prefs;
    if (!prefs.begin("prefs", true)) // read-only; fails if the namespace does not exist
    {
      debugW("No bioreactor NVS data; using defaults");
      return;
    }
    // getBytes() refuses a buffer smaller than the stored blob (100 bytes today), so
    // read it whole and take the leading fields.
    StoredDeviceInfoPrefix stored{};
    uint8_t raw[256];
    const size_t len = prefs.getBytesLength("deviceInfo");
    if (len >= sizeof(stored) && len <= sizeof(raw) && prefs.getBytes("deviceInfo", raw, len) == len)
    {
      memcpy(&stored, raw, sizeof(stored));
      deviceInfo.found = true;
      deviceInfo.hardwareVersion = stored.hardwareVersion;
      deviceInfo.deviceType = stored.deviceType;
      memcpy(deviceInfo.serial, stored.serialNumber, sizeof(deviceInfo.serial));
      deviceInfo.serial[sizeof(deviceInfo.serial) - 1] = '\0';
    }
    prefs.end();
  }
}

Controls::Button Device::powerButton([] {
  Controls::Button::Config c;
  c.pin = POWER_BUTTON_PIN;
  c.activeHigh = true;
  c.pull = Controls::Button::Pull::Down;
  c.longMs = POWER_OFF_HOLD_MS;
  return c;
}());

// The repeat is only there for the Buttons screen to show.
Controls::Button Device::bootButton([] {
  Controls::Button::Config c;
  c.pin = BOOT_BUTTON_PIN;
  c.repeatMs = 200;
  return c;
}());

void Device::begin()
{
  pinMode(LATCH_PWR_PIN, OUTPUT);
  digitalWrite(LATCH_PWR_PIN, HIGH);
  // The press that switched the board on is still held here, so it never counts.
  Controls::add(powerButton);
  Controls::add(bootButton);

  readDeviceInfo();
  // Same choices as bioreactor-controller's setDeviceSettings(), then turned if asked.
  const uint8_t bioreactorRotation = deviceInfo.deviceType == kDeviceTypeBioreactor8 ? 3 : 1;
  deviceInfo.rotation = ROTATE_180 ? (bioreactorRotation + 2) % 4 : bioreactorRotation;
  deviceInfo.backlightActiveHigh = deviceInfo.hardwareVersion != 2 && deviceInfo.hardwareVersion != 3;

  ledcSetup(kBacklightChannel, kBacklightHz, kBacklightBits);
  ledcAttachPin(DISPLAY_BACKLIGHT_PIN, kBacklightChannel);
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
  backlightOn = on;
  // 255 reads as fully on: the core turns the maximum duty into 100 %.
  const uint32_t duty = on ? 255u * backlightPercent / 100 : 0;
  ledcWrite(kBacklightChannel, deviceInfo.backlightActiveHigh ? duty : 255 - duty);
}

void Device::setBrightness(uint8_t percent)
{
  backlightPercent = constrain(percent, 5, 100);
  if (backlightOn)
    setBacklight(true);
}

uint8_t Device::brightness()
{
  return backlightPercent;
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
  Controls::update();
  if (powerButton.longPressed())
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
