#include "config.h"

bool relay1Busy = false;
bool relay2Busy = false;
bool relay3Busy = false;
bool relay4Busy = false;
bool relay5Busy = false;
bool relay6Busy = false;

TaskHandle_t blinkLedHandle;
TaskHandle_t relay1FlashHandle;
TaskHandle_t lockDoorHandle;
TaskHandle_t unlockDoorHandle;