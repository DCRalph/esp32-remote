#include "Wireless.h"

Wireless::Wireless()
{
}

void Wireless::setup()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  delay(100);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status)
                           { wireless.sendCallback(mac_addr, status); });

  setupDone = true;
}

void Wireless::unsetup()
{
  setupDone = false;
  esp_now_deinit();

  esp_now_unregister_recv_cb();
  esp_now_unregister_send_cb();
}

void Wireless::loop()
{
  if (!setupDone)
    return;
}

void Wireless::sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);

  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  if (status == ESP_NOW_SEND_SUCCESS)
  {
    screenManager.showPopup(new Popup("Success", "Packet sent successfully"));
  }
  else
  {
    screenManager.showPopup(new Popup("Error", "Failed to send packet"));
  }
}

int Wireless::send(packet *p, u8_t *peer_addr)
{
  return send((u8_t *)p, sizeof(packet), peer_addr);
}

int Wireless::send(u8_t *data, size_t len, u8_t *peer_addr)
{
  if (!setupDone)
  {
    Serial.println("ESP-NOW not initialized");
    return -1;
  }

  esp_now_peer_info_t peerInfo = esp_now_peer_info_t();

  memcpy(peerInfo.peer_addr, peer_addr, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Peer added");
#endif

  if (esp_now_send(peerInfo.peer_addr, data, len) != ESP_OK)
  {
    Serial.println("Failed to send data");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Data sent");
#endif

  if (esp_now_del_peer(peer_addr) != ESP_OK)
  {
    Serial.println("Failed to delete peer");
    return -1;
  }
#ifdef DEBUG_ESP_NOW
  Serial.println("Peer deleted");
#endif

  return 0;
}

Wireless wireless;