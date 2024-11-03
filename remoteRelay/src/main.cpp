#include <Arduino.h>

#include <WiFi.h>
#include <esp_wifi.h>

#include <esp_now.h>

#define CHANNEL 1
#define RELAY1 6
#define RELAY2 8

struct __attribute__((packed)) packet
{
  uint8_t type;
  uint8_t len;
  uint8_t data[200];
};

void RELAY1FN()
{
  digitalWrite(RELAY1, 0);
  delay(1000);
  digitalWrite(RELAY1, 1);

  Serial.println("RELAY1FN");
}

void RELAY2FN()
{
  digitalWrite(RELAY2, 0);
  delay(1000);
  digitalWrite(RELAY2, 1);

  Serial.println("RELAY2FN");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: ");
  Serial.println(macStr);
  Serial.print("Last Packet Recv Data Length: ");
  Serial.println(data_len);
  // Serial.print("Last Packet Recv Data: ");
  // Serial.println(*data);
  Serial.println("");

  packet p;

  memcpy(&p, data, sizeof(packet));

  String dataS = "";

  for (int i = 0; i < p.len; i++)
  {
    dataS += (char)p.data[i];
  }

  Serial.print("Data: ");
  Serial.println("type: " + String(p.type) + " len: " + String(p.len));
  Serial.println("Data Hex: ");
  for (int i = 0; i < p.len; i++)
  {
    Serial.print(p.data[i], HEX);
    Serial.print(" ");
  }

  Serial.println("");
  Serial.println("Data ASCII: ");
  Serial.println(dataS);

  digitalWrite(15, 1);

  if (p.type == 11)
  {
    RELAY1FN();
  }

  if (p.type == 12)
  {
    RELAY2FN();
  }

  digitalWrite(15, 0);
}

void setup()
{
  Serial.begin(115200);

  pinMode(15, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, 1);
  digitalWrite(RELAY2, 1);

  digitalWrite(15, 1);
  delay(3000);
  digitalWrite(15, 0);

  WiFi.mode(WIFI_AP);
  // WiFi.disconnect();

  // esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  // esp_wifi_set_promiscuous(false);

  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  // put your main code here, to run repeatedly:
}
