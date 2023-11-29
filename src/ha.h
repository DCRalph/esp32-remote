#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "secrets.h"

class HA
{
public:
  DynamicJsonDocument getState(String entity_id)
  {
    HTTPClient http;
    String url = String(HA_SERVER) + "/api/states/" + entity_id;
    http.begin(url);
    http.addHeader("Authorization", HA_TOKEN);
    int httpCode = http.GET();
    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    // String state = doc["state"];
    return doc;
  }

  DynamicJsonDocument callService(String domain, String service, String entity_id)
  {
    HTTPClient http;
    String url = String(HA_SERVER) + "/api/services/" + domain + "/" + service;
    http.begin(url);
    http.addHeader("Authorization", HA_TOKEN);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"entity_id\": \"" + entity_id + "\"}";

    int httpCode = http.POST(payload);
    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    // String state = doc["state"];
    return doc;
  }

  DynamicJsonDocument callService(String domain, String service, String entity_id, String payload)
  {
    HTTPClient http;
    String url = String(HA_SERVER) + "/api/services/" + domain + "/" + service;
    http.begin(url);
    http.addHeader("Authorization", HA_TOKEN);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);
    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);

    // String state = doc["state"];
    return doc;
  }
};

HA ha;