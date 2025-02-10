/*
 * Wireless.cpp
 *
 * Description:
 * ------------
 * Implementation file for the Wireless class, which uses ESP-NOW to send
 * and receive data packets. Also implements the new synced struct feature.
 */

#include "Wireless.h"

/*
 * Global instance of the Wireless class. 
 * This is declared extern in Wireless.h.
 */
Wireless wireless;

//---------------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------------
Wireless::Wireless() {
  // Nothing special in constructor. Real setup is in setup().
}

//---------------------------------------------------------------------------------
// setup
//---------------------------------------------------------------------------------
void Wireless::setup() {
  // Start by putting the WiFi in STA mode so that ESP-NOW can function properly.
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  // Set the channel to our preferred ESP_NOW_CHANNEL. 
  // We briefly enable promiscuous mode to force the channel setting.
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  delay(1);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status) {
    wireless.sendCallback(mac_addr, status);
  });

  // Register the receive callback
  esp_now_register_recv_cb([](const uint8_t *mac_addr, const uint8_t *data, int len) {
    wireless.recvCallback(mac_addr, data, len);
  });

  // If we reach here, we've successfully started ESP-NOW.
  setupDone = true;
  Serial.println("ESP-NOW initialized successfully.");
}

//---------------------------------------------------------------------------------
// unSetup
//---------------------------------------------------------------------------------
void Wireless::unSetup() {
  // Mark setup as not done
  setupDone = false;

  // Unregister callbacks
  esp_now_unregister_recv_cb();
  esp_now_unregister_send_cb();

  // Deinitialize ESP-NOW
  esp_now_deinit();

  Serial.println("ESP-NOW deinitialized.");
}

//---------------------------------------------------------------------------------
// loop
//---------------------------------------------------------------------------------
void Wireless::loop() {
  if (!setupDone) {
    return;
  }
  // Handle any "auto-sync" logic for synced structs here.
  handleSyncedStructs();
}

//---------------------------------------------------------------------------------
// isSetupDone
//---------------------------------------------------------------------------------
bool Wireless::isSetupDone() {
  return setupDone;
}

//---------------------------------------------------------------------------------
// sendCallback
//---------------------------------------------------------------------------------
void Wireless::sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Save the status for reference
  lastStatus = status;

#ifdef DEBUG_ESP_NOW
  // Convert MAC to string for debugging.
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println("########### Sent Packet ###########");
  Serial.print("Sent to: ");
  Serial.println(macStr);
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("###################################");
#endif
}

//---------------------------------------------------------------------------------
// recvCallback
//---------------------------------------------------------------------------------
void Wireless::recvCallback(const uint8_t *mac_addr, const uint8_t *data, int len) {
  // Safety check
  if (data == nullptr || len < (int)sizeof(data_packet)) {
    Serial.println("Received invalid data or length.");
    return;
  }

  // Copy the incoming data into a data_packet structure
  data_packet *p = (data_packet *) data;

#ifdef DEBUG_ESP_NOW
  // Convert MAC to string for debugging
  char macStr[18];
  snprintf(macStr, sizeof(macStr),
           "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.println("########### Received Packet ###########");
  Serial.print("Recv from: ");
  Serial.println(macStr);
  Serial.print("Type: ");
  Serial.println(p->type);
  Serial.print("Len: ");
  Serial.println(p->len);
  Serial.print("Data: ");
  for (int i = 0; i < p->len; i++) {
    Serial.print(p->data[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("#######################################");
#endif

  // If this might be a synced struct update, handle that first
  // (For example, you could define a certain 'type' or custom logic 
  //  that indicates "this is a synced struct" data. 
  //  For demonstration, let's assume we use 'type==0x99' or something.)
  if (p->type == 0x99) {
    // We interpret p->data as a raw struct update
    SyncedStruct* s = findSyncedStructByMac(mac_addr);
    if (s) {
      // Copy the payload into the local dataStruct if lengths match
      if (p->len == s->dataSize) {
        memcpy(s->dataStruct, p->data, s->dataSize);
        s->synced = true;
        s->_justSynced = true;
        s->lastSyncTime = millis();
      }
    }
    return; 
  }

  // Otherwise, it's a normal data packet -> pass it to user callback if set
  fullPacket fp;
  memcpy(fp.mac, mac_addr, 6);
  fp.direction = PacketDirection::RECV;
  memcpy(&fp.p, p, sizeof(data_packet));

  if (recvCb) {
    recvCb(&fp);
  }
}

//---------------------------------------------------------------------------------
// setRecvCb
//---------------------------------------------------------------------------------
void Wireless::setRecvCb(std::function<void(fullPacket *fp)> cb) {
  recvCb = cb;
}

//---------------------------------------------------------------------------------
// send(data_packet *p, uint8_t *peer_addr)
//---------------------------------------------------------------------------------
int Wireless::send(data_packet *p, uint8_t *peer_addr) {
  // Just call our other send(...) using the raw pointer + size.
  return send((uint8_t *)p, sizeof(data_packet), peer_addr);
}

//---------------------------------------------------------------------------------
// send(uint8_t *data, size_t len, uint8_t *peer_addr)
//---------------------------------------------------------------------------------
int Wireless::send(uint8_t *data, size_t len, uint8_t *peer_addr) {
  // If not initialized, fail
  if (!setupDone) {
    Serial.println("ESP-NOW not initialized");
    return -1;
  }

  // Prepare a peer structure
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peer_addr, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;

  // Add the peer if it isn't already added
  // (We can attempt to add every time. If it's already there, 
  //  esp_now_add_peer() might fail with ESP_ERR_ESPNOW_FULL or ESP_ERR_ESPNOW_EXIST.
  //  We can remove that error or handle it. For simplicity, let's ignore duplicates.)
  esp_err_t addResult = esp_now_add_peer(&peerInfo);
  if (addResult != ESP_OK && addResult != ESP_ERR_ESPNOW_EXIST) {
    Serial.print("Failed to add peer, error code: ");
    Serial.println(addResult);
    return -1;
  }

  // Send data
  esp_err_t sendResult = esp_now_send(peerInfo.peer_addr, data, len);
  if (sendResult != ESP_OK) {
    Serial.print("Failed to send data, error code: ");
    Serial.println(sendResult);
    return -1;
  }

#ifdef DEBUG_ESP_NOW
  Serial.println("Data sent successfully.");
#endif

  // Optionally, remove the peer if we only do ephemeral sends
  // But removing peer can break repeated sends. 
  // If you want ephemeral connections, uncomment:
  /*
  if (esp_now_del_peer(peer_addr) != ESP_OK) {
    Serial.println("Failed to delete peer");
    return -1;
  }
  #ifdef DEBUG_ESP_NOW
  Serial.println("Peer deleted");
  #endif
  */

  return 0;
}

//---------------------------------------------------------------------------------
// send(fullPacket *fp)
//---------------------------------------------------------------------------------
int Wireless::send(fullPacket *fp) {
  // We only send if direction is SEND
  if (fp->direction == PacketDirection::SEND) {
    return send(&fp->p, fp->mac);
  } else {
    Serial.println("Cannot send a receive packet");
    return -1;
  }
}

//---------------------------------------------------------------------------------
// Synced Struct Feature
//---------------------------------------------------------------------------------

/*
 * registerSync
 *
 * This function returns a SyncedStruct object that the user can store.
 * We also keep a copy in our internal vector for management.
 */
SyncedStruct Wireless::registerSync(void *dataPtr, size_t size, const uint8_t mac[6]) {
  SyncedStruct s;
  memcpy(s.mac, mac, 6);
  s.dataStruct = dataPtr;
  s.dataSize = size;
  s.lastSyncTime = 0;
  s.synced = false;
  s.activeSync = false;
  s._justSynced = false;

  // Save in our vector for reference
  syncedStructs.push_back(s);

  return s;
}

/*
 * startSync
 *
 * Mark the given struct as active for auto-syncing.
 */
void Wireless::startSync(SyncedStruct *syncedStruct) {
  // Find the struct in the vector and set activeSync = true
  for (auto &it : syncedStructs) {
    if (&it == syncedStruct) {
      it.activeSync = true;
      Serial.println("startSync: SyncedStruct is now active");
      return;
    }
  }
  Serial.println("startSync: SyncedStruct not found in our list.");
}

/*
 * stopSync
 *
 * Mark the given struct as no longer active for auto-syncing.
 */
void Wireless::stopSync(SyncedStruct *syncedStruct) {
  // Find the struct in the vector and set activeSync = false
  for (auto &it : syncedStructs) {
    if (&it == syncedStruct) {
      it.activeSync = false;
      Serial.println("stopSync: SyncedStruct is now inactive");
      return;
    }
  }
  Serial.println("stopSync: SyncedStruct not found in our list.");
}

/*
 * syncNow
 *
 * Immediately sends the local data to the remote side.
 * We wrap it in a data_packet with p->type = 0x99 to indicate "synced struct".
 */
void Wireless::syncNow(SyncedStruct *syncedStruct) {
  if (!setupDone) {
    Serial.println("syncNow: Wireless not set up yet.");
    return;
  }

  // Quick check: do we have a matching struct in our internal vector?
  for (auto &it : syncedStructs) {
    if (&it == syncedStruct) {
      // Build a data_packet with the synced data
      data_packet p;
      p.type = 0x99; // Arbitrary type to indicate "synced struct"
      p.len = (uint8_t)it.dataSize;
      if (p.len > sizeof(p.data)) {
        Serial.println("syncNow: dataSize is too large for data_packet!");
        return;
      }
      memcpy(p.data, it.dataStruct, it.dataSize);

      // Attempt to send it
      int result = send(&p, it.mac);
      if (result == 0) {
        Serial.println("syncNow: Synced data sent successfully.");
        // We can consider ourselves 'synced' from this side's perspective,
        // though not guaranteed unless we get confirmation from remote.
        // For now we set it to true for demonstration.
        it.synced = true;
      }
      return;
    }
  }

  Serial.println("syncNow: SyncedStruct not found in our list.");
}

/*
 * handleSyncedStructs
 *
 * Called from loop() to handle auto-sync. If you want to regularly sync,
 * you can check how long it's been since lastSyncTime, etc.
 */
void Wireless::handleSyncedStructs() {
  // Example logic: if activeSync is true, and it's been more than 5 seconds
  // since the last sync, we call syncNow. 
  // Adjust the interval as needed. 
  static const uint64_t SYNC_INTERVAL_MS = 5000;

  uint64_t now = millis();
  for (auto &it : syncedStructs) {
    if (it.activeSync) {
      // Check if enough time has passed to auto-sync again
      if ((now - it.lastSyncTime) > SYNC_INTERVAL_MS) {
        syncNow(&it);
        it.lastSyncTime = now;  // update after we request sync
      }
    }
  }
}

/*
 * findSyncedStructByMac
 *
 * Locates a SyncedStruct in our vector by matching MAC address.
 */
SyncedStruct* Wireless::findSyncedStructByMac(const uint8_t mac[6]) {
  for (auto &it : syncedStructs) {
    if (memcmp(it.mac, mac, 6) == 0) {
      return &it;
    }
  }
  return nullptr;
}


// //##############
// // synced struct feature
// //##############

// // the idea for this is to have a struct defined on 2 esp32s that will automatically sync with each other when data is changed
// // this will be useful for syncing data between 2 esp32s that are connected to each other



// // info about a synced struct defiened in the wireless class
// struct SyncedStruct
// {
//   uint8_t mac[6];
//   void *dataStruct;
//   size_t dataSize;
//   uint64_t lastSyncTime;
//   bool synced;
// };

// // esp1

// // define the struct
// struct ExampleData
// {
//   int exampleInt;
//   float exampleFloat;
//   char exampleString[20];
// };

// void exampleForEsp1()
// {

//   ExampleData exampleData;

//   // code to setup the synced struct

//   // this should seup the struct to auto sync with the other esp

//   uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//   SyncedStruct syncInstance = wireless.registerSync(&exampleData, sizeof(ExampleData), mac);

//   // functions to control the sync
//   wireless.startSync(&syncInstance);
//   wireless.stopSync(&syncInstance);
//   wireless.syncNow(&syncInstance);

//   // i should be able to access the struct like this at any time

//   if (syncInstance.synced)
//   {
//     Serial.println(exampleData.exampleInt);
//     Serial.println(exampleData.exampleFloat);
//     Serial.println(exampleData.exampleString);
//   }

//   // there should me a syncInstance.justSynced flag that is set to true when the struct is synced
//   // this flag should be set to false after the struct is read

//   if (syncInstance.justSynced())
//   {
//     Serial.println("Just synced");
//   }
// }

// // esp2

// // define the struct
// struct ExampleData
// {
//   int exampleInt;
//   float exampleFloat;
//   char exampleString[20];
// };

// void exampleForEsp2()
// {

//   ExampleData exampleData;

//   // code to setup the synced struct

//   // this should seup the struct to auto sync with the other esp

//   uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//   SyncedStruct syncInstance = wireless.registerSync(&exampleData, sizeof(ExampleData), mac);

//   // functions to control the sync
//   wireless.startSync(&syncInstance);
//   wireless.stopSync(&syncInstance);
//   wireless.syncNow(&syncInstance);

//   // i should be able to access the struct like this at any time

//   if (syncInstance.synced)
//   {
//     Serial.println(exampleData.exampleInt);
//     Serial.println(exampleData.exampleFloat);
//     Serial.println(exampleData.exampleString);
//   }

//   // there should me a syncInstance.justSynced flag that is set to true when the struct is synced
//   // this flag should be set to false after the struct is read

//   if (syncInstance.justSynced())
//   {
//     Serial.println("Just synced");
//   }
// }

