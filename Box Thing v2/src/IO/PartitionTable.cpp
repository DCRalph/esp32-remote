#include "IO/PartitionTable.h"

// PartitionEntry methods
String PartitionEntry::getTypeName() const
{
  switch (static_cast<PartitionType>(type))
  {
  case PartitionType::APP:
    switch (static_cast<AppSubtype>(subtype))
    {
    case AppSubtype::FACTORY:
      return "factory";
    case AppSubtype::OTA_0:
      return "ota_0";
    case AppSubtype::OTA_1:
      return "ota_1";
    case AppSubtype::OTA_2:
      return "ota_2";
    case AppSubtype::OTA_3:
      return "ota_3";
    case AppSubtype::OTA_4:
      return "ota_4";
    case AppSubtype::OTA_5:
      return "ota_5";
    case AppSubtype::OTA_6:
      return "ota_6";
    case AppSubtype::OTA_7:
      return "ota_7";
    case AppSubtype::OTA_8:
      return "ota_8";
    case AppSubtype::OTA_9:
      return "ota_9";
    case AppSubtype::OTA_10:
      return "ota_10";
    case AppSubtype::OTA_11:
      return "ota_11";
    case AppSubtype::OTA_12:
      return "ota_12";
    case AppSubtype::OTA_13:
      return "ota_13";
    case AppSubtype::OTA_14:
      return "ota_14";
    case AppSubtype::OTA_15:
      return "ota_15";
    case AppSubtype::TEST:
      return "test";
    default:
      return "unknown(" + String(subtype, HEX) + ")";
    }
  case PartitionType::DATA:
    switch (static_cast<DataSubtype>(subtype))
    {
    case DataSubtype::OTA:
      return "ota";
    case DataSubtype::PHY:
      return "phy";
    case DataSubtype::NVS:
      return "nvs";
    case DataSubtype::COREDUMP:
      return "coredump";
    case DataSubtype::NVS_KEYS:
      return "nvs_keys";
    case DataSubtype::ESPHTTPD:
      return "esphttpd";
    case DataSubtype::FAT:
      return "fat";
    case DataSubtype::SPIFFS:
      return "spiffs";
    default:
      return "unknown(" + String(subtype, HEX) + ")";
    }
  default:
    return "unknown(" + String(type, HEX) + ")";
  }
}

String PartitionEntry::getSubtypeName() const
{
  return getTypeName(); // Reuse the same logic for now
}

void PartitionEntry::print() const
{
  String name = String(this->name);
  for (int i = name.length(); i < 20; i++)
    name += " ";

  Serial.println(name + ", " +
                 getTypeName() + ", " +
                 String(start) + ", " +
                 String(size) + ", " +
                 String(magic, HEX));
}

bool PartitionEntry::operator==(const PartitionEntry &other) const
{
  return magic == other.magic &&
         type == other.type &&
         subtype == other.subtype &&
         start == other.start &&
         size == other.size &&
         strcmp(name, other.name) == 0;
}

bool PartitionEntry::operator!=(const PartitionEntry &other) const
{
  return !(*this == other);
}

// PartitionTable methods
PartitionTable::PartitionTable() : count(0)
{
  memset(entries, 0, sizeof(entries));
}

bool PartitionTable::readFromFlash()
{
  uint8_t buffer[PARTITION_TABLE_SIZE];
  memset(buffer, 0, PARTITION_TABLE_SIZE);

  esp_err_t err = spi_flash_read(PARTITION_TABLE_ADDRESS, buffer, PARTITION_TABLE_SIZE);
  if (err != ESP_OK)
  {
    Serial.println("[ERROR] Failed to read partition table: " + String(esp_err_to_name(err)));
    return false;
  }

  count = 0;
  for (size_t i = 0; i < MAX_PARTITIONS; i++)
  {
    PartitionEntry &entry = entries[i];
    memcpy(&entry, buffer + (i * PARTITION_ENTRY_SIZE), PARTITION_ENTRY_SIZE);

    if (entry.magic != 0x50AA)
    {
      break;
    }
    count++;
  }

  return true;
}

bool PartitionTable::writeToFlash()
{
  uint8_t *buffer = (uint8_t *)malloc(PARTITION_TABLE_ALIGNED_SIZE);
  if (buffer == NULL)
  {
    Serial.println("[ERROR] Failed to allocate memory for partition table");
    return false;
  }

  memset(buffer, 0, PARTITION_TABLE_ALIGNED_SIZE);

  for (size_t i = 0; i < count; i++)
  {
    if (entries[i].magic != 0x50AA)
    {
      Serial.println("[ERROR] Invalid magic number in partition entry " + String(i));
      free(buffer);
      return false;
    }
    memcpy(buffer + (i * PARTITION_ENTRY_SIZE), &entries[i], PARTITION_ENTRY_SIZE);
  }

  if (isAlreadyWritten(*this))
  {
    Serial.println("[INFO] Partition table is already up to date");
    free(buffer);
    return true;
  }

  if (!isRunningFromOta1())
  {
    Serial.println("[ERROR] Not running from OTA1 partition - unsafe to update partition table");
    free(buffer);
    return false;
  }

  esp_err_t err = ESP_FAIL;
  for (int attempts = 1; attempts <= PARTITION_TABLE_RETRY_LIMIT; attempts++)
  {
    Serial.println("[INFO] Attempt " + String(attempts) + " of " + String(PARTITION_TABLE_RETRY_LIMIT));

    Serial.println("[INFO] Erasing partition table...");
    err = spi_flash_erase_range(PARTITION_TABLE_ADDRESS, PARTITION_TABLE_ALIGNED_SIZE);
    if (err != ESP_OK)
    {
      Serial.println("[ERROR] Failed to erase partition table: " + String(esp_err_to_name(err)));
      sleepMs(PARTITION_TABLE_RETRY_WAIT);
      continue;
    }

    err = spi_flash_write(PARTITION_TABLE_ADDRESS, buffer, PARTITION_TABLE_ALIGNED_SIZE);
    if (err != ESP_OK)
    {
      Serial.println("[ERROR] Failed to write partition table: " + String(esp_err_to_name(err)));
      sleepMs(PARTITION_TABLE_RETRY_WAIT);
      continue;
    }

    break;
  }

  free(buffer);

  if (err != ESP_OK)
  {
    Serial.println("[ERROR] Failed to update partition table after " + String(PARTITION_TABLE_RETRY_LIMIT) + " attempts");
    return false;
  }

  Serial.println("[INFO] Partition table updated successfully");
  return true;
}

bool PartitionTable::compareWith(const PartitionTable &other) const
{
  if (count != other.count)
  {
    Serial.println("[WARN] Partition count mismatch: " + String(count) + " vs " + String(other.count));
    return false;
  }

  bool match = true;
  for (size_t i = 0; i < count; i++)
  {
    if (entries[i] != other.entries[i])
    {
      Serial.println("[WARN] Partition mismatch at index " + String(i));
      Serial.println("Current:");
      entries[i].print();
      Serial.println("Reference:");
      other.entries[i].print();
      match = false;
    }
  }

  return match;
}

void PartitionTable::print() const
{
  Serial.println("[INFO] Partition Table:");
  Serial.println("# Name,                  Type, Offset, Size, Magic");
  for (size_t i = 0; i < count; i++)
  {
    entries[i].print();
  }
}

void PartitionTable::setEntries(const PartitionEntry *newEntries, size_t newCount)
{
  if (newCount > MAX_PARTITIONS)
  {
    Serial.println("[ERROR] Too many partitions");
    return;
  }

  count = newCount;
  memcpy(entries, newEntries, newCount * sizeof(PartitionEntry));
}

PartitionTable PartitionTable::createFromFlash()
{
  PartitionTable table;
  table.readFromFlash();
  return table;
}

PartitionTable PartitionTable::createFromReference(const PartitionEntry *refEntries, size_t refCount)
{
  PartitionTable table;
  table.setEntries(refEntries, refCount);
  return table;
}

bool PartitionTable::isAlreadyWritten(const PartitionTable &reference) const
{
  uint8_t buffer[256];
  uint8_t refBuffer[PARTITION_TABLE_SIZE];

  memset(refBuffer, 0, PARTITION_TABLE_SIZE);
  for (size_t i = 0; i < reference.count; i++)
  {
    memcpy(refBuffer + (i * PARTITION_ENTRY_SIZE), &reference.entries[i], PARTITION_ENTRY_SIZE);
  }

  for (int offset = 0; offset < PARTITION_TABLE_SIZE; offset += 256)
  {
    esp_err_t err = spi_flash_read(PARTITION_TABLE_ADDRESS + offset, buffer, 256);
    if (err != ESP_OK)
    {
      Serial.println("[ERROR] Failed to read partition table: " + String(esp_err_to_name(err)));
      return false;
    }

    if (offset == 0 && (buffer[0] != 0xAA || buffer[1] != 0x50))
    {
      Serial.println("[ERROR] Invalid existing partition table. Maybe bad offset.");
      return false;
    }

    for (int i = 0; i < 256 && (offset + i) < PARTITION_TABLE_SIZE; i++)
    {
      if (buffer[i] != refBuffer[offset + i])
      {
        Serial.println("[INFO] Partition table differs at offset " + String(offset + i));
        return false;
      }
    }
  }

  return true;
}

bool PartitionTable::isValidTable() const
{
  if (count == 0)
  {
    return false;
  }

  if (entries[0].magic != 0x50AA)
  {
    return false;
  }

  return true;
}

String PartitionTable::getRunningPartitionLabel() const
{
  const esp_partition_t *running = esp_ota_get_running_partition();
  if (running == nullptr)
  {
    return "unknown";
  }
  return String(running->label);
}

bool PartitionTable::isRunningFromOta1() const
{
  const esp_partition_t *running = esp_ota_get_running_partition();
  if (running == nullptr)
  {
    return false;
  }
  return (running->type == ESP_PARTITION_TYPE_APP &&
          running->subtype == ESP_PARTITION_SUBTYPE_APP_OTA_1);
}

bool PartitionTable::safeToUpdatePartitionTable() const
{
  return isRunningFromOta1();
}

void PartitionTable::sleepMs(int ms)
{
  delay(ms);
}