#include "Wireless.h"

Wireless::Wireless()
{
}

void Wireless::setup()
{
  // WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(AP_SSID, "*#&@^&*#&@", ESP_NOW_CHANNEL, true);

  delay(100);
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // esp_now_register_send_cb([](uint8_t *mac_addr, uint8_t status)
  //                          { wireless.sendCallback(mac_addr, status); });

  setupDone = true;
}

void Wireless::unSetup()
{
  setupDone = false;
  // esp_now_deinit();

  // esp_now_unregister_recv_cb();
  // esp_now_unregister_send_cb();
}

void Wireless::loop()
{
  if (!setupDone)
    return;
}

bool Wireless::isSetupDone()
{
  return setupDone;
}

void Wireless::sendCallback(uint8_t *mac_addr, uint8_t status)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);

  Serial.print("Last Packet Send Status: ");
  Serial.println(status == 0 ? "Delivery Success" : "Delivery Fail");
  lastStatus = status;
}

int Wireless::send(packet *p, uint8_t *peer_addr)
{
  return send((uint8_t *)p, sizeof(packet), peer_addr);
}

int Wireless::send(uint8_t *data, size_t len, uint8_t *peer_addr)
{
  if (!setupDone)
  {
    Serial.println("ESP-NOW not initialized");
    return -1;
  }

  // if (esp_now_add_peer(peer_addr, ESP_NOW_ROLE_SLAVE, ESP_NOW_CHANNEL, NULL, 0) != 0)
  // {
  //   Serial.println("Failed to add peer");
  //   return -1;
  // }

  // if (esp_now_send(peer_addr, data, len) != 0)
  // {
  //   Serial.println("Failed to send data");
  //   return -1;
  // }

  // if (esp_now_del_peer(peer_addr) != 0)
  // {
  //   Serial.println("Failed to delete peer");
  //   return -1;
  // }

  return 0;
}

Wireless wireless;