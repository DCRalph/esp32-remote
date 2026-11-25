// Every screen in the firmware, declared in one place.
//
// dcr_display navigates by `const Screen2 *` rather than by name, so anything
// that wants to jump to a screen needs its address. Screen headers are single
// translation units included once by main.cpp, and they reference each other
// freely, so the declarations live here to break the ordering cycle.
#pragma once

#include <ScreenTypes.h>

// Boot / power
extern const Screen2 StartUpScreen2;
extern const Screen2 UpdateProgressScreen2;
extern const Screen2 ShutdownScreen2;

// Debug
extern const Screen2 DebugScreen2;
extern const Screen2 IOTestScreen2;
extern const Screen2 BatteryScreen2;

// Top level
extern const Screen2 HomeScreen2;
extern const Screen2 SwitchMenuScreen2;
extern const Screen2 SettingsScreen2;

// Control
extern const Screen2 EspnowSwitchScreen2;
extern const Screen2 CarControlScreen2;
extern const Screen2 RemoteRelayScreen2;
extern const Screen2 EncoderTransmiterScreen2;
extern const Screen2 ServoControlScreen2;

// Settings
extern const Screen2 GeneralSettingsScreen2;
extern const Screen2 WiFiSettingsScreen2;
extern const Screen2 WiFiInfoScreen2;
extern const Screen2 WiFiScanScreen2;
extern const Screen2 WiFiPasswordScreen2;
