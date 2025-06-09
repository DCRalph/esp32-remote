#pragma once

#include <Arduino.h>
#include "esp_spi_flash.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"

// Maximum number of partitions in a table
#define MAX_PARTITIONS 16
#define PARTITION_TABLE_RETRY_LIMIT 10
#define PARTITION_TABLE_RETRY_WAIT 100

// Partition types
enum class PartitionType : uint8_t
{
  APP = 0,
  DATA = 1
};

// Partition subtypes for APP type
enum class AppSubtype : uint8_t
{
  FACTORY = 0x00,
  OTA_0 = 0x10,
  OTA_1 = 0x11,
  OTA_2 = 0x12,
  OTA_3 = 0x13,
  OTA_4 = 0x14,
  OTA_5 = 0x15,
  OTA_6 = 0x16,
  OTA_7 = 0x17,
  OTA_8 = 0x18,
  OTA_9 = 0x19,
  OTA_10 = 0x1a,
  OTA_11 = 0x1b,
  OTA_12 = 0x1c,
  OTA_13 = 0x1d,
  OTA_14 = 0x1e,
  OTA_15 = 0x1f,
  TEST = 0x20
};

// Partition subtypes for DATA type
enum class DataSubtype : uint8_t
{
  OTA = 0x00,
  PHY = 0x01,
  NVS = 0x02,
  COREDUMP = 0x03,
  NVS_KEYS = 0x04,
  ESPHTTPD = 0x80,
  FAT = 0x81,
  SPIFFS = 0x82
};

// Partition entry structure
struct PartitionEntry
{
  uint16_t magic;
  uint8_t type;
  uint8_t subtype;
  uint32_t start;
  uint32_t size;
  char name[20];

  // Helper methods
  String getTypeName() const;
  String getSubtypeName() const;
  void print() const;
  bool operator==(const PartitionEntry &other) const;
  bool operator!=(const PartitionEntry &other) const;
};

class PartitionTable
{
public:
  PartitionTable();

  // Core functionality
  bool readFromFlash();
  bool writeToFlash();
  bool compareWith(const PartitionTable &other) const;
  void print() const;

  // Getters
  const PartitionEntry *getEntries() const { return entries; }
  size_t getCount() const { return count; }

  // Setters
  void setEntries(const PartitionEntry *newEntries, size_t newCount);
  void clear() { count = 0; }

  // Static factory methods
  static PartitionTable createFromFlash();
  static PartitionTable createFromReference(const PartitionEntry *refEntries, size_t refCount);

  // OTA and safety methods
  bool isAlreadyWritten(const PartitionTable &reference) const;
  bool isValidTable() const;
  String getRunningPartitionLabel() const;
  bool isRunningFromOta1() const;
  bool safeToUpdatePartitionTable() const;

  static constexpr uint16_t PARTITION_TABLE_MAGIC = 0x50AA;

private:
  PartitionEntry entries[MAX_PARTITIONS];
  size_t count;

  static const size_t PARTITION_TABLE_ADDRESS = 0x8000;
  static const size_t PARTITION_TABLE_SIZE = 0xC00;
  static const size_t PARTITION_TABLE_ALIGNED_SIZE = 0x1000; // 4KB alignment
  static const size_t PARTITION_ENTRY_SIZE = sizeof(PartitionEntry);

  // Helper methods
  static void sleepMs(int ms);
};