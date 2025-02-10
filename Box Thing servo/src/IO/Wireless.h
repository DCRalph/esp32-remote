/*
 * Wireless.h
 *
 * Description:
 * ------------
 * Header file for the Wireless class, which uses ESP-NOW to send and receive
 * data packets on the ESP32. It also implements a "synced struct" mechanism
 * that allows two ESP32s to keep a shared struct in sync automatically.
 */

#pragma once

// Include necessary Arduino/ESP32 headers.
// Make sure these are installed/available in your environment.
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <vector>
#include <functional>
#include <cstring>

// A global config file could define constants such as ESP_NOW_CHANNEL, etc.
// For demonstration, we define them here (adjust as needed).
#ifndef ESP_NOW_CHANNEL
#define ESP_NOW_CHANNEL 1
#endif

//---------------------------------------------------------------------------------
// Data structures
//---------------------------------------------------------------------------------

/*
 * data_packet
 *
 * Description:
 * ------------
 * A simple packed struct that carries a type, a length, and up to 200 bytes of data.
 *
 * Attributes:
 *   - type : a user-defined type (e.g., to indicate message type).
 *   - len  : the number of bytes in 'data' currently in use.
 *   - data : raw data buffer (up to 200 bytes).
 */
struct __attribute__((packed)) data_packet {
  uint8_t type;
  uint8_t len;
  uint8_t data[200];
};

/*
 * PacketDirection
 *
 * Description:
 * ------------
 * An enum class to indicate whether a packet is being sent or received.
 */
enum class PacketDirection {
  SEND,
  RECV
};

/*
 * fullPacket
 *
 * Description:
 * ------------
 * A composite that includes the sender's/receiver's MAC address,
 * the direction (SEND or RECV), and the underlying data_packet.
 *
 * Attributes:
 *   - mac       : the MAC address associated with this packet.
 *   - direction : indicates if this packet is being sent or received.
 *   - p         : the data packet itself.
 */
struct fullPacket {
  uint8_t mac[6];
  PacketDirection direction;
  data_packet p;
};

//---------------------------------------------------------------------------------
// Synced struct feature
//---------------------------------------------------------------------------------

/*
 * SyncedStruct
 *
 * Description:
 * ------------
 * Represents a struct that will be kept in sync between two ESP32s.
 * The user calls registerSync(...) to create one of these, specifying:
 *   - a pointer to the local data structure
 *   - the size of that structure
 *   - the remote MAC address
 *
 * Then calls startSync(...) to begin auto-syncing, and so on.
 *
 * Attributes:
 *   - mac         : MAC address of the remote ESP32 with which we sync.
 *   - dataStruct  : pointer to the local data structure to keep in sync.
 *   - dataSize    : size in bytes of that data structure.
 *   - lastSyncTime: tracks last time this struct was synced, for scheduling.
 *   - synced      : indicates if the local struct is believed to be fully synced.
 *   - activeSync  : if true, we automatically sync on an interval (if desired).
 *   - _justSynced : internal flag set true after a successful sync; user can query.
 */
struct SyncedStruct {
  uint8_t mac[6];
  void *dataStruct;   
  size_t dataSize;    
  uint64_t lastSyncTime;
  bool synced;
  bool activeSync;  
  bool _justSynced; 

  // Simple helper method to let user check if the struct was just synced.
  // Once checked, we reset it to false so it doesn't stay "justSynced" forever.
  bool justSynced() {
    if (_justSynced) {
      _justSynced = false;
      return true;
    }
    return false;
  }
};

//---------------------------------------------------------------------------------
// Wireless Class
//---------------------------------------------------------------------------------

class Wireless {
private:
  // Whether setup() has been successfully called.
  bool setupDone = false;

  // Callback function invoked when a packet is received (if set).
  std::function<void(fullPacket *fp)> recvCb;

  // Keep track of any SyncedStruct objects we have registered.
  // We can store them in a vector so multiple synced structs can be managed.
  std::vector<SyncedStruct> syncedStructs;

public:
  // The last send status set by the send callback.
  esp_now_send_status_t lastStatus = ESP_NOW_SEND_FAIL;

  // Constructor / Destructor
  Wireless();
  ~Wireless() = default;

  //---------------------------------------------------------------------------------
  // Basic ESP-NOW lifecycle
  //---------------------------------------------------------------------------------
  void setup();
  void unSetup();
  void loop();  

  // Check if we have finished setup
  bool isSetupDone();

  //---------------------------------------------------------------------------------
  // Callbacks
  //---------------------------------------------------------------------------------
  void sendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
  void recvCallback(const uint8_t *mac_addr, const uint8_t *data, int len);

  //---------------------------------------------------------------------------------
  // User-facing set/gets
  //---------------------------------------------------------------------------------
  void setRecvCb(std::function<void(fullPacket *fp)> cb);

  //---------------------------------------------------------------------------------
  // Sending data
  //---------------------------------------------------------------------------------
  int send(data_packet *p, uint8_t *peer_addr);
  int send(uint8_t *data, size_t len, uint8_t *peer_addr);
  int send(fullPacket *fp);

  //---------------------------------------------------------------------------------
  // Synced Struct Feature
  //---------------------------------------------------------------------------------

  /*
   * registerSync
   *
   * Description:
   * ------------
   * Register a struct to be synced with a remote device. Returns a SyncedStruct
   * that the user can store.
   *
   * Parameters:
   *   - dataPtr : pointer to local data
   *   - size    : size of the data struct in bytes
   *   - mac     : MAC address of the remote device we want to sync with
   *
   * Returns:
   *   - a SyncedStruct object that can be used with startSync, stopSync, etc.
   */
  SyncedStruct registerSync(void *dataPtr, size_t size, const uint8_t mac[6]);

  /*
   * startSync
   *
   * Description:
   * ------------
   * Enables auto-sync for the given SyncedStruct. 
   * If the struct was previously registered, we mark it as active.
   */
  void startSync(SyncedStruct *syncedStruct);

  /*
   * stopSync
   *
   * Description:
   * ------------
   * Disables auto-sync for the given SyncedStruct.
   */
  void stopSync(SyncedStruct *syncedStruct);

  /*
   * syncNow
   *
   * Description:
   * ------------
   * Immediately sends the local data to the remote device associated with
   * the given SyncedStruct.
   */
  void syncNow(SyncedStruct *syncedStruct);

private:
  /*
   * handleSyncedStructs
   *
   * Description:
   * ------------
   * Internally called from loop() to handle any auto-sync logic (timers, etc.).
   * This is where you'd expand if you want scheduled sync intervals.
   */
  void handleSyncedStructs();

  /*
   * findSyncedStructByMac
   *
   * Description:
   * ------------
   * Helper to locate a SyncedStruct in our vector by MAC address.
   */
  SyncedStruct* findSyncedStructByMac(const uint8_t mac[6]);
};

// Create a single global instance of the Wireless class.
extern Wireless wireless;

