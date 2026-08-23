#pragma once

#include <Menu.h>
#include <dcr_TaskManager.h>

#include "config.h"
#include "IO/GPIO.h"
#include "Screens/Screens.h"
#include "Setup/InitOTA.h"

static String formatBytes(size_t bytes, bool _short = false)
{
  if (bytes < 1024)
  {
    if (_short)
    {
      // Since bytes is an integer, leave it as is.
      return String(bytes);
    }
    else
    {
      return String(bytes) + " B";
    }
  }
  else if (bytes < (1024UL * 1024UL))
  {
    // Force floating point math by casting bytes to double.
    double kb = ((double)bytes) / 1024.0;
    if (_short)
    {
      return String(kb, 1) + "K";
    }
    else
    {
      return String(kb, 2) + " KB";
    }
  }
  else
  {
    double mb = ((double)bytes) / (1024.0 * 1024.0);
    if (_short)
    {
      return String(mb, 1) + "M";
    }
    else
    {
      return String(mb, 2) + " MB";
    }
  }
}

namespace
{
  long gDebugBootCount = 0;
  bool gDebugLedState = false;
  bool gDebugOtaState = false;

  String gDebugTotalMem = "0";
  String gDebugFreeMem = "0";
  String gDebugUsedMem = "0";

  // dcr_taskManager replaces the old per-phase render timers, which dcr_display's
  // render() does not expose.
  int gDebugCpu0 = 0;
  int gDebugCpu1 = 0;
  int gDebugTaskCount = 0;

  Menu debugMenu(MenuSize::Medium);

  MenuItemBack debugBackItem;
  MenuItemNavigate debugIoTestItem("IO Test", &IOTestScreen2);
  MenuItemNavigate debugBatteryItem("Battery", &BatteryScreen2);
  MenuItemToggle debugLedItem("LED", &gDebugLedState);
  MenuItemNumber<long> debugBootCountItem("Boot", &gDebugBootCount);
  MenuItemToggle debugOtaItem("OTA", &gDebugOtaState);

  MenuItemNumber<uint32_t> debugFpsItem("FPS", &lastFps);
  MenuItemNumber<uint32_t> debugFrameTimeItem("F Time", &lastFrameTime);

  MenuItemNumber<int> debugCpu0Item("CPU0 %", &gDebugCpu0);
  MenuItemNumber<int> debugCpu1Item("CPU1 %", &gDebugCpu1);
  MenuItemNumber<int> debugTaskCountItem("Tasks", &gDebugTaskCount);

  MenuItemString debugTotalMemItem("tmem", &gDebugTotalMem);
  MenuItemString debugFreeMemItem("fmem", &gDebugFreeMem);
  MenuItemString debugUsedMemItem("umem", &gDebugUsedMem);

  [[maybe_unused]] const bool debugMenuBuilt = []()
  {
    debugMenu.addMenuItem(&debugBackItem);
    debugMenu.addMenuItem(&debugIoTestItem);
    debugMenu.addMenuItem(&debugBatteryItem);
    debugMenu.addMenuItem(&debugLedItem);
    debugMenu.addMenuItem(&debugBootCountItem);
    debugMenu.addMenuItem(&debugOtaItem);

    debugMenu.addMenuItem(&debugFpsItem);
    debugMenu.addMenuItem(&debugFrameTimeItem);

    debugMenu.addMenuItem(&debugCpu0Item);
    debugMenu.addMenuItem(&debugCpu1Item);
    debugMenu.addMenuItem(&debugTaskCountItem);

    debugMenu.addMenuItem(&debugTotalMemItem);
    debugMenu.addMenuItem(&debugFreeMemItem);
    debugMenu.addMenuItem(&debugUsedMemItem);

    debugLedItem.setOnChange([]()
                             { led.Write(gDebugLedState); });

    debugOtaItem.setOnChange([]()
                             {
                               if (!otaSetup)
                                 InitOta();
                               gDebugOtaState = true;
                               //
                             });
    return true;
  }();

  void debugDraw()
  {
    debugMenu.draw();
  }

  void debugUpdate()
  {
    debugMenu.update();

    const uint64_t totalMem = ESP.getHeapSize();
    const uint64_t freeMem = ESP.getFreeHeap();

    gDebugTotalMem = formatBytes(totalMem, true);
    gDebugFreeMem = formatBytes(freeMem, true);
    gDebugUsedMem = formatBytes(totalMem - freeMem, true);

    gDebugCpu0 = (int)taskManager.getCpuUsagePercent(0);
    gDebugCpu1 = (int)taskManager.getCpuUsagePercent(1);
    gDebugTaskCount = (int)taskManager.snapshotTasks().size();
  }

  void debugOnEnter()
  {
    gDebugBootCount = preferences.getLong("bootCount", 0);
    gDebugLedState = led.read();
    gDebugOtaState = otaSetup;
  }
}

const Screen2 DebugScreen2 = {
    .name = "Debug",
    .draw = debugDraw,
    .update = debugUpdate,
    .onEnter = debugOnEnter,
    .onExit = nullptr,
};
