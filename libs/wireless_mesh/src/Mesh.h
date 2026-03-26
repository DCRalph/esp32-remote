#pragma once

#include <Arduino.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "ITransport.h"

// ============================================================
// Mesh sync constants (single place for all tunables and limits)
// ============================================================

namespace MeshConstants
{
  // Protocol / transport
  constexpr uint16_t kMeshPacketType = 0xBEEF;
  constexpr uint8_t kProtocolVersion = 2;
  constexpr uint8_t kFlagNoRelay = 0x01;
  constexpr uint16_t kMaxMeshPayload = 235;
  constexpr uint8_t kMaxEventPayloadSize = 232; // kMaxMeshPayload - sizeof(EventHeader)

  // Property store
  constexpr uint8_t kMaxPropertyValueSize = 230;
  constexpr uint16_t kMaxPropertyEntries = 200;
  constexpr uint32_t kPropertyReannounceIntervalMs = 5000;
  constexpr uint32_t kPropertyCleanupIntervalMs = 10000;
  constexpr uint32_t kPropertyStaleMs = 30000;

  // Discovery / group
  constexpr uint32_t kHeartbeatInterval = 1000;
  constexpr uint32_t kDiscoveryTimeout = 6000;
  constexpr uint32_t kGroupAnnounceInterval = 2000;
  constexpr uint32_t kGroupDiscoveryTimeout = 6000;
  constexpr uint32_t kGroupInfoInterval = 2000;
  constexpr uint32_t kGroupMemberTimeout = 8000;
  /// Base for AUTO-mode group IDs; actual ID is kAutoGroupId | (random 16 bits) when creating.
  constexpr uint32_t kAutoGroupId = 0xA1100000u;
  constexpr uint32_t kAutoGroupIdMask = 0xFFFF0000u;

  // Time sync
  constexpr uint32_t kTimeSyncInterval = 10000;

  // Relay
  constexpr uint8_t kDefaultRelayHopLimit = 4;
  constexpr uint32_t kRelaySeenTtlMs = 15000;
  constexpr size_t kRelaySeenMax = 512;

  // OffsetClock
  constexpr uint32_t kMaxAcceptedRttMs = 2000;
}

// ============================================================
// Mesh protocol envelope and message taxonomy
// ============================================================

namespace MeshProtocol
{
  enum class MessageKind : uint8_t
  {
    Discovery = 1,
    Group = 2,
    Time = 3,
    Property = 5,
    Event = 6
  };

  enum class DiscoveryCommand : uint8_t
  {
    Heartbeat = 1
  };

  enum class GroupCommand : uint8_t
  {
    Announce = 1,
    Join = 2,
    Info = 3,
    Leave = 4
  };

  enum class TimeCommand : uint8_t
  {
    Request = 1,
    Response = 2
  };

  enum class PropertyCommand : uint8_t
  {
    Update = 1,
    Batch = 2
  };

  enum class EventCommand : uint8_t
  {
    Emit = 1
  };

  struct __attribute__((packed)) FrameHeader
  {
    uint8_t version;
    uint8_t kind;
    uint8_t command;
    uint8_t flags;
    uint32_t originDeviceId;
    uint32_t sequence;
    uint8_t hopLimit;
    uint16_t payloadLen;
  };

  struct __attribute__((packed)) PropertyEntryHeader
  {
    uint16_t key;
    uint16_t revision;
    uint8_t valueLen;
  };

  struct __attribute__((packed)) EventHeader
  {
    uint16_t channelId;
    uint8_t payloadLen;
  };

  struct DecodedFrame
  {
    MessageKind kind;
    uint8_t command;
    uint8_t flags;
    uint32_t originDeviceId;
    uint32_t sequence;
    uint8_t hopLimit;
    const uint8_t *payload;
    uint16_t payloadLen;
  };

  bool decode(const TransportPacket &pkt, DecodedFrame &out);
  bool encode(TransportPacket &pkt,
              MessageKind kind,
              uint8_t command,
              uint8_t flags,
              uint32_t originDeviceId,
              uint32_t sequence,
              uint8_t hopLimit,
              const uint8_t *payload,
              uint16_t payloadLen);
  bool readPropertyEntryAt(const uint8_t *payload,
                           uint16_t payloadLen,
                           uint16_t offset,
                           uint16_t &nextOffset,
                           PropertyEntryHeader &outHeader,
                           const uint8_t *&outValue);
  bool appendPropertyEntry(std::vector<uint8_t> &out,
                           uint16_t key,
                           uint16_t revision,
                           const uint8_t *value,
                           uint8_t valueLen);
  bool buildEventPayload(std::vector<uint8_t> &out,
                         uint16_t channelId,
                         const uint8_t *payload,
                         uint8_t payloadLen);
  bool readEventPayload(const uint8_t *payload,
                        uint16_t payloadLen,
                        EventHeader &outHeader,
                        const uint8_t *&outEventPayload);
  bool isNewerRevision(uint16_t incoming, uint16_t stored);
} // namespace MeshProtocol

// ============================================================
// Shared time offset clock (single-master authority model)
// ============================================================

class OffsetClock
{
public:
  void reset();
  void setMasterReference();
  bool handleSample(uint32_t requestTimestamp, uint32_t masterTimestamp, uint32_t nowMillis);

  bool isLocked() const;
  int32_t offsetMillis() const;
  uint32_t nowSyncedMillis(uint32_t localNow) const;
  uint32_t lastRttMillis() const;

private:
  bool locked_ = false;
  int32_t offsetMs_ = 0;
  uint32_t lastRttMs_ = 0;
};

// ============================================================
// Owner-partitioned property store and typed handles
// ============================================================

struct PropertySnapshot
{
  uint16_t key = 0;
  uint32_t ownerDeviceId = 0;
  uint16_t revision = 0;
  std::vector<uint8_t> payload;
};

class PropertyStore
{
public:
  struct PropertyEntry
  {
    uint16_t key = 0;
    uint32_t ownerDeviceId = 0;
    uint16_t revision = 0;
    uint8_t valueLen = 0;
    std::array<uint8_t, MeshConstants::kMaxPropertyValueSize> value{};
    uint32_t lastUpdateMs = 0;
  };

  bool setLocal(uint16_t key,
                uint32_t localDeviceId,
                const uint8_t *value,
                uint8_t valueLen,
                uint32_t nowMs,
                PropertySnapshot &outSnapshot);
  bool applyRemote(uint16_t key,
                   uint32_t ownerDeviceId,
                   uint16_t revision,
                   const uint8_t *value,
                   uint8_t valueLen,
                   uint32_t nowMs,
                   PropertySnapshot &outSnapshot,
                   bool &outChanged);

  bool has(uint16_t key, uint32_t ownerDeviceId) const;
  bool get(uint16_t key, uint32_t ownerDeviceId, std::vector<uint8_t> &out) const;
  uint16_t getRevision(uint16_t key, uint32_t ownerDeviceId) const;
  std::vector<PropertySnapshot> getAll() const;
  std::vector<PropertySnapshot> getOwnedBy(uint32_t ownerDeviceId) const;

  void clearRemoteEntries(uint32_t localDeviceId);
  void removeKey(uint16_t key);
  void cleanupStale(uint32_t nowMs, uint32_t staleMs, uint32_t localDeviceId);

private:
  PropertyEntry *findEntry(uint16_t key, uint32_t ownerDeviceId);
  const PropertyEntry *findEntry(uint16_t key, uint32_t ownerDeviceId) const;
  uint16_t nextRevision(uint16_t current) const;

  std::vector<PropertyEntry> entries_;
};

class SyncManager;

template <typename T>
class PropertyHandle
{
public:
  PropertyHandle() = default;

  void set(const T &value) const;
  T get() const;
  T get(uint32_t deviceId) const;
  bool has(uint32_t deviceId) const;
  void onChange(std::function<void(uint32_t deviceId, T value)> cb) const;
  uint16_t key() const { return key_; }
  uint16_t revision() const;
  uint16_t revision(uint32_t deviceId) const;

private:
  friend class SyncManager;
  PropertyHandle(SyncManager *manager, uint16_t key)
      : manager_(manager), key_(key) {}

  SyncManager *manager_ = nullptr;
  uint16_t key_ = 0;
};

class RawPropertyHandle
{
public:
  RawPropertyHandle() = default;

  void set(const uint8_t *data, uint8_t len) const;
  uint8_t get(uint8_t *outBuf, uint8_t bufSize) const;
  uint8_t get(uint32_t deviceId, uint8_t *outBuf, uint8_t bufSize) const;
  bool has(uint32_t deviceId) const;
  void onChange(std::function<void(uint32_t deviceId, const uint8_t *data, uint8_t len)> cb) const;
  uint16_t key() const { return key_; }
  uint8_t maxLen() const { return maxLen_; }
  uint16_t revision() const;
  uint16_t revision(uint32_t deviceId) const;

private:
  friend class SyncManager;
  RawPropertyHandle(SyncManager *manager, uint16_t key, uint8_t maxLen)
      : manager_(manager), key_(key), maxLen_(maxLen) {}

  SyncManager *manager_ = nullptr;
  uint16_t key_ = 0;
  uint8_t maxLen_ = 0;
};

template <typename T = void>
class EventHandle
{
public:
  EventHandle() = default;

  void emit(const T &payload) const;
  void onEvent(std::function<void(uint32_t fromDevice, T payload)> cb) const;
  uint16_t channelId() const { return channelId_; }

private:
  friend class SyncManager;
  EventHandle(SyncManager *manager, uint16_t channelId)
      : manager_(manager), channelId_(channelId) {}

  SyncManager *manager_ = nullptr;
  uint16_t channelId_ = 0;
};

template <>
class EventHandle<void>
{
public:
  EventHandle() = default;

  void emit() const;
  void onEvent(std::function<void(uint32_t fromDevice)> cb) const;
  uint16_t channelId() const { return channelId_; }

private:
  friend class SyncManager;
  EventHandle(SyncManager *manager, uint16_t channelId)
      : manager_(manager), channelId_(channelId) {}

  SyncManager *manager_ = nullptr;
  uint16_t channelId_ = 0;
};

// ============================================================
// Public mesh manager API
// ============================================================

enum class SyncMode : uint8_t
{
  SOLO,
  JOIN,
  HOST,
  AUTO
};

struct DiscoveredDevice
{
  uint32_t deviceId;
  uint8_t mac[6];
  uint32_t lastSeen;
  SyncMode syncMode;
};

struct GroupAdvert
{
  uint32_t groupId;
  uint32_t masterDeviceId;
  uint8_t masterMac[6];
  uint32_t lastSeen;
};

struct GroupMember
{
  uint32_t deviceId;
  uint8_t mac[6];
};

struct GroupLeaveCmd
{
  uint32_t groupId;
  uint32_t deviceId;
};

struct GroupInfo
{
  uint32_t groupId;
  uint32_t masterDeviceId;
  bool isMaster;
  std::map<std::string, GroupMember> members;
  bool timeSynced;
  int32_t timeOffset;
};

class SyncManager
{
public:
  static SyncManager *getInstance();

  void begin();
  void loop();
  void setTransport(ITransport *transport);
  ITransport *getTransport() const;
  void setModePersistence(std::function<uint8_t()> loadCb,
                          std::function<void(uint8_t)> saveCb);
  void setDeviceIdProvider(std::function<uint32_t()> provider);

  const std::map<std::string, DiscoveredDevice> &
  getDiscoveredDevices() const;

  std::vector<GroupAdvert> getDiscoveredGroups() const;

  const GroupInfo &getGroupInfo() const;
  bool isInGroup() const;
  bool isGroupMaster() const;
  uint32_t getGroupId() const;

  uint32_t getDeviceId() const;

  void createGroup(uint32_t groupId = 0);
  void joinGroup(uint32_t groupId);
  void leaveGroup();

  void setSyncMode(SyncMode mode);
  SyncMode getSyncMode() const;
  String getSyncModeString();
  String getSyncModeString(SyncMode mode);

  void requestTimeSync();
  bool isTimeSynced() const;
  uint32_t getSyncedTime() const;
  int32_t getTimeOffset() const;
  void setRelayEnabled(bool enabled);
  bool isRelayEnabled() const;

  static uint32_t syncMillis();

  void setDeviceDiscoveredCallback(
      std::function<void(const DiscoveredDevice &)> cb);
  void setGroupFoundCallback(
      std::function<void(const GroupAdvert &)> cb);
  void setGroupCreatedCallback(
      std::function<void(const GroupInfo &)> cb);
  void setGroupJoinedCallback(
      std::function<void(const GroupInfo &)> cb);
  void setGroupLeftCallback(std::function<void()> cb);

  void setTimeSyncCallback(
      std::function<void(uint32_t syncedTime)> cb);

  void printDeviceInfo();
  void printGroupInfo();
  void printSyncModeInfo();

  template <typename T>
  PropertyHandle<T> property(uint16_t key)
  {
    static_assert(std::is_trivially_copyable<T>::value, "Property type must be trivially copyable");
    if (!registerProperty(key, sizeof(T)))
    {
      return PropertyHandle<T>();
    }
    return PropertyHandle<T>(this, key);
  }

  RawPropertyHandle propertyRaw(uint16_t key, uint8_t maxLen);
  bool unregisterProperty(uint16_t key);

  template <typename T>
  std::enable_if_t<!std::is_void_v<T>, EventHandle<T>> event(uint16_t channelId)
  {
    if constexpr (std::is_void_v<T>)
    {
      return event(channelId); // delegate to non-templated void overload
    }
    else
    {
      static_assert(std::is_trivially_copyable<T>::value, "Event payload type must be trivially copyable");
      if (!registerEvent(channelId, sizeof(T)))
      {
        return EventHandle<T>();
      }
      return EventHandle<T>(this, channelId);
    }
  }

  template <typename T>
  std::enable_if_t<std::is_void_v<T>, EventHandle<void>> event(uint16_t channelId)
  {
    if (!registerEvent(channelId, 0))
    {
      return EventHandle<void>();
    }
    return EventHandle<void>(this, channelId);
  }

  EventHandle<void> event(uint16_t channelId);
  bool unregisterEvent(uint16_t channelId);
  std::vector<PropertySnapshot> getDiscoveredProperties() const;

  void sendHeartbeat();
  void sendGroupAnnounce();
  void sendGroupInfo();

private:
  friend class RawPropertyHandle;
  template <typename T>
  friend class PropertyHandle;
  template <typename T>
  friend class EventHandle;

  SyncManager();
  ~SyncManager();

  void handleSyncPacket(const TransportAddress &source, const TransportPacket &packet);
  void handleTransportFrame(const TransportAddress &source, const TransportPacket &packet);
  void processHeartbeat(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processGroupAnnounce(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processGroupJoin(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processGroupInfo(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processGroupLeave(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processTimeRequest(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processTimeResponse(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processPropertyUpdate(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processPropertyBatch(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);
  void processEventEmit(const MeshProtocol::DecodedFrame &frame, const TransportAddress &source);

  void checkDiscoveryCleanup(uint32_t now);
  void checkGroupCleanup(uint32_t now);
  void checkMemberTimeout(uint32_t now);
  void checkJoinMode(uint32_t now);
  void checkAutoMode(uint32_t now);
  void cleanupRelayCache(uint32_t now);
  void cleanupProperties(uint32_t now);
  void reannounceProperties(uint32_t now, bool force = false);

  void loadPreferences();
  void saveSyncModePreferences();

  bool registerProperty(uint16_t key, uint8_t maxLen);
  bool registerEvent(uint16_t channelId, uint8_t maxLen);
  bool setPropertyBytes(uint16_t key, const uint8_t *data, uint8_t len);
  bool getPropertyBytesForOwner(uint16_t key, uint32_t ownerDeviceId, std::vector<uint8_t> &out) const;
  bool hasPropertyValue(uint16_t key, uint32_t ownerDeviceId) const;
  uint16_t getPropertyRevision(uint16_t key, uint32_t ownerDeviceId) const;
  void addPropertyChangeCallback(uint16_t key, std::function<void(uint32_t, const uint8_t *, uint8_t)> cb);
  void clearPropertyChangeCallbacks(uint16_t key);
  bool emitEventBytes(uint16_t channelId, const uint8_t *payload, uint8_t payloadLen);
  void addEventCallback(uint16_t channelId, std::function<void(uint32_t, const uint8_t *, uint8_t)> cb);
  void clearEventCallbacks(uint16_t channelId);
  void notifyPropertyChange(uint16_t key, uint32_t ownerDeviceId, const uint8_t *data, uint8_t len);
  void notifyEvent(uint16_t channelId, uint32_t fromDeviceId, const uint8_t *payload, uint8_t payloadLen);
  bool applyIncomingPropertyEntry(uint16_t key, uint16_t revision, uint32_t ownerDeviceId, const uint8_t *value, uint8_t valueLen);

  bool buildLocalMeshPacket(TransportPacket &pkt,
                            MeshProtocol::MessageKind kind,
                            uint8_t command,
                            const uint8_t *payload,
                            uint16_t payloadLen,
                            uint8_t flags = 0,
                            uint8_t hopLimit = MeshConstants::kDefaultRelayHopLimit);
  bool rememberRelayFrame(uint32_t originDeviceId, uint32_t sequence, uint32_t now);
  bool shouldForwardRelayFrame(const MeshProtocol::DecodedFrame &frame) const;
  void forwardRelayFrame(const MeshProtocol::DecodedFrame &frame);
  uint64_t relayFrameKey(uint32_t originDeviceId, uint32_t sequence) const;
  bool isRelayedFrame(const MeshProtocol::DecodedFrame &frame) const;
  bool isGroupMember(uint32_t deviceId) const;
  bool sourceMatchesDevice(const TransportAddress &source, uint32_t deviceId, bool isRelayed) const;
  bool sendPacketChecked(const TransportPacket &pkt, const TransportAddress &peer, const char *context) const;
  bool getMasterMac(uint8_t outMac[6]) const;
  uint32_t generateDeviceId();
  uint32_t generateGroupId();
  std::string macToString(const uint8_t *mac) const;
  const uint8_t *getOurMac();

  ITransport *transport_ = nullptr;
  std::function<uint8_t()> loadModeCb_;
  std::function<void(uint8_t)> saveModeCb_;
  std::function<uint32_t()> deviceIdProvider_;

  uint8_t localMac_[6]{};
  bool localMacReady_ = false;

  std::map<std::string, DiscoveredDevice> discoveredDevices;
  std::map<uint32_t, GroupAdvert> discoveredGroups;
  GroupInfo currentGroup;
  uint32_t ourDeviceId;

  SyncMode syncMode = SyncMode::SOLO;

  OffsetClock offsetClock_;
  uint32_t lastTimeSync = 0;
  uint32_t lastTimeReq = 0;
  bool started_ = false;

  struct PropertyRegistration
  {
    uint8_t maxLen = 0;
    std::vector<std::function<void(uint32_t, const uint8_t *, uint8_t)>> callbacks;
  };
  struct EventRegistration
  {
    uint8_t maxLen = 0;
    std::vector<std::function<void(uint32_t, const uint8_t *, uint8_t)>> callbacks;
  };
  std::map<uint16_t, PropertyRegistration> propertyRegistry_;
  std::map<uint16_t, EventRegistration> eventRegistry_;
  PropertyStore propertyStore_;

  uint32_t lastHeartbeat = 0;
  uint32_t lastGrpAnnounce = 0;
  uint32_t lastGrpInfo = 0;
  uint32_t lastPropertyReannounce = 0;
  uint32_t lastPropertyCleanup = 0;
  bool relayEnabled_ = true;
  uint32_t nextSequence_ = 1;
  std::map<uint64_t, uint32_t> relaySeenFrames_;

  std::function<void(const DiscoveredDevice &)> onDeviceDiscovered;
  std::function<void(const GroupAdvert &)> onGroupFound;
  std::function<void(const GroupInfo &)> onGroupCreated;
  std::function<void(const GroupInfo &)> onGroupJoined;
  std::function<void()> onGroupLeft;
  std::function<void(uint32_t)> onTimeSynced;
};

template <typename T>
void PropertyHandle<T>::set(const T &value) const
{
  if (manager_ == nullptr)
  {
    return;
  }
  manager_->setPropertyBytes(key_, reinterpret_cast<const uint8_t *>(&value), sizeof(T));
}

template <typename T>
T PropertyHandle<T>::get() const
{
  if (manager_ == nullptr)
  {
    return T{};
  }
  return get(manager_->getDeviceId());
}

template <typename T>
T PropertyHandle<T>::get(uint32_t deviceId) const
{
  T out{};
  if (manager_ == nullptr || deviceId == 0)
  {
    return out;
  }
  std::vector<uint8_t> bytes;
  if (!manager_->getPropertyBytesForOwner(key_, deviceId, bytes) || bytes.size() != sizeof(T))
  {
    return out;
  }
  memcpy(&out, bytes.data(), sizeof(T));
  return out;
}

template <typename T>
bool PropertyHandle<T>::has(uint32_t deviceId) const
{
  return manager_ != nullptr && manager_->hasPropertyValue(key_, deviceId);
}

template <typename T>
void PropertyHandle<T>::onChange(std::function<void(uint32_t deviceId, T value)> cb) const
{
  if (manager_ == nullptr || !cb)
  {
    return;
  }
  manager_->addPropertyChangeCallback(
      key_,
      [cb = std::move(cb)](uint32_t deviceId, const uint8_t *data, uint8_t len)
      {
        if (data == nullptr || len != sizeof(T))
        {
          return;
        }
        T value{};
        memcpy(&value, data, sizeof(T));
        cb(deviceId, value);
      });
}

template <typename T>
uint16_t PropertyHandle<T>::revision() const
{
  if (manager_ == nullptr)
  {
    return 0;
  }
  return manager_->getPropertyRevision(key_, manager_->getDeviceId());
}

template <typename T>
uint16_t PropertyHandle<T>::revision(uint32_t deviceId) const
{
  if (manager_ == nullptr || deviceId == 0)
  {
    return 0;
  }
  return manager_->getPropertyRevision(key_, deviceId);
}

template <typename T>
void EventHandle<T>::emit(const T &payload) const
{
  if (manager_ == nullptr)
  {
    return;
  }
  manager_->emitEventBytes(channelId_, reinterpret_cast<const uint8_t *>(&payload), sizeof(T));
}

template <typename T>
void EventHandle<T>::onEvent(std::function<void(uint32_t fromDevice, T payload)> cb) const
{
  if (manager_ == nullptr || !cb)
  {
    return;
  }
  manager_->addEventCallback(
      channelId_,
      [cb = std::move(cb)](uint32_t fromDevice, const uint8_t *payload, uint8_t payloadLen)
      {
        if (payload == nullptr || payloadLen != sizeof(T))
        {
          return;
        }
        T value{};
        memcpy(&value, payload, sizeof(T));
        cb(fromDevice, value);
      });
}