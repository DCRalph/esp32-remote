#include "config.h"
#include "secrets.h"
#include "IO/Wireless.h"
#include "IO/GpIO.h"
#include "IO/Battery.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <FastLED.h>
#include <ArduinoJson.h>

uint64_t ledBlinkMs = 0;
uint8_t blinks = 0;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
unsigned long lastBattUpdate = 0;

const long interval = 10000; // every 10 seconds

const char *dhtTopic = "solar/sensor/dht11";
const char *batteryTopic = "solar/sensor/battery";
const char *ledCommandTopic = "solar/led/command";
const char *ledStatusTopic = "solar/led/status";

#define LED_PIN 6 // GPIO 6 for the LED data
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

#define DHTPIN 5 // GPIO 5 for DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void espNowRecvCb(fullPacket *fp)
{

  blinks = 4;
  ledBlinkMs = millis();
}

void connectWiFI();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);

void setup()
{
  Serial.begin(115200);

  delay(2000);

  WiFi.mode(WIFI_AP_STA);

  GpIO::initIO();

  // Initialize DHT sensor
  dht.begin();

  // Initialize FastLED for WS2812 (WS2812, GRB order)
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  connectWiFI();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // wireless.setup();
  wireless.setOnReceiveOther(espNowRecvCb);
  led.On();
  delay(500);
  led.Off();
}

void loop()
{
  if (blinks > 0)
  {
    if (millis() - ledBlinkMs > 100)
    {
      ledBlinkMs = millis();
      led.Toggle();
      blinks--;
    }

    if (blinks == 0)
      led.Off();
  }

  if (Serial.available() > 0)
  {
    String input = Serial.readString();
    while (Serial.available() > 0)
      input += Serial.readString();
    if (input == "reboot")
      ESP.restart();
    else if (input == "getip")
      Serial.println(WiFi.localIP());
    else if (input == "getmac")
      Serial.println(WiFi.macAddress());
    else
      Serial.println("[INFO] [SERIAL] " + input);
  }

  // Ensure we're connected to MQTT
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  // Update battery voltage every 500ms
  if (now - lastBattUpdate > 500)
  {
    lastBattUpdate = now;
    batteryUpdate();
    led.Toggle();
  }

  // Publish sensor data every 'interval'
  if (now - lastMsg > interval)
  {
    lastMsg = now;

    // Read and publish DHT11 sensor data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    if (isnan(temperature))
    {
      temperature = -127;
    }

    if (isnan(humidity))
    {
      humidity = -127;
    }

    char payload[100];
    snprintf(payload, sizeof(payload),
             "{\"temperature\": %.2f, \"humidity\": %.2f}",
             temperature, humidity);
    client.publish(dhtTopic, payload);
    Serial.print("DHT: ");
    Serial.println(payload);

    // Read and publish battery voltage

    char batPayload[50];
    snprintf(batPayload, sizeof(batPayload), "{\"voltage\": %.2f}", batteryGetVoltage());
    client.publish(batteryTopic, batPayload);
    Serial.print("Battery: ");
    Serial.println(batPayload);
  }
}

void connectWiFI()
{
  WiFi.begin(ssid, password);
  Serial.print("[INFO] [WIFI] Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("[INFO] [WIFI] Connected to WiFi");
  Serial.println("[INFO] [WIFI] IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected to the MQTT broker
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32S3Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println(" connected");
      // Subscribe to LED command topic
      client.subscribe(ledCommandTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

// MQTT callback to process incoming LED commands (JSON format)
// Examples:
//   {"state":"ON","color":{"r":97,"g":255,"b":79}}
//   {"state":"ON","brightness":115}
//   {"state":"OFF"}
void callback(char *topic, byte *payload, unsigned int length)
{
  String topicStr = topic;
  String payloadStr;
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }
  Serial.printf("Message arrived [%s]: %s\n",
                topic, payloadStr.c_str());

  if (topicStr == ledCommandTopic)
  {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payloadStr);
    if (error)
    {
      Serial.print("JSON deserialization failed: ");
      Serial.println(error.f_str());
      return;
    }
    const char *state = doc["state"];

    // Turn LED off if state is "OFF"
    if (state && strcmp(state, "OFF") == 0)
    {
      leds[0] = CRGB::Black;
      FastLED.show();
      client.publish(ledStatusTopic,
                     "{\"state\":\"OFF\",\"r\":0,\"g\":0,\"b\":0}");
      Serial.println("LED turned OFF");
      return;
    }

    int r = 0, g = 0, b = 0;
    bool updateColor = false;

    // If a "color" object is provided, extract its RGB values
    if (doc.containsKey("color"))
    {
      JsonObject color = doc["color"];
      r = color["r"] | 0;
      g = color["g"] | 0;
      b = color["b"] | 0;
      updateColor = true;
    }

    // If a brightness value is provided, use it
    if (doc.containsKey("brightness"))
    {
      int brightness = doc["brightness"] | 0;
      brightness = constrain(brightness, 0, 255);
      FastLED.setBrightness(brightness);
      Serial.printf("Brightness set to: %d\n", brightness);
    }

    // If no color object is provided, assume full white or keep
    // current color. In this example, we use white.
    if (!updateColor)
    {
      r = 255;
      g = 255;
      b = 255;
    }

    if (updateColor)
    {
      Serial.printf("Color set to: r=%d, g=%d, b=%d\n", r, g, b);
    }

    // Set the LED color and update
    leds[0] = CRGB(r, g, b);
    FastLED.show();

    // Publish status update with the current LED values
    char statusMsg[100];
    snprintf(statusMsg, sizeof(statusMsg),
             "{\"state\":\"ON\",\"r\":%d,\"g\":%d,\"b\":%d}",
             r, g, b);
    client.publish(ledStatusTopic, statusMsg);
    Serial.print("Published LED state: ");
    Serial.println(statusMsg);
  }
}