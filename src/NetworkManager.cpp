#include "NetworkManager.h"
#include "UserConfig.h"
#include <ArduinoJson.h>

NetworkManager::NetworkManager() : _wifiClient(), _mqttClient(_wifiClient) {
  _lastWifiCheck = 0;
  _lastMqttCheck = 0;
}

void NetworkManager::begin(const char *ssid, const char *password,
                           const char *mqttServer, int mqttPort,
                           const char *mqttUser, const char *mqttPassword) {
  _ssid = ssid;
  _password = password;
  _mqttServer = mqttServer;
  _mqttPort = mqttPort;
  _mqttUser = mqttUser;
  _mqttPassword = mqttPassword;

  WiFi.mode(WIFI_STA);
  connectWifi();

  _mqttClient.setServer(_mqttServer, _mqttPort);
  _mqttClient.setBufferSize(2048);
}

// Fixed: Now accepts raw function pointer
void NetworkManager::setMqttCallback(MqttCallback callback) {
  _mqttClient.setCallback(callback);
}

void NetworkManager::update() {
  if (millis() - _lastWifiCheck > 5000) {
    _lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!_mqttClient.connected()) {
      if (millis() - _lastMqttCheck > 5000) {
        _lastMqttCheck = millis();
        connectMqtt();
      }
    } else {
      _mqttClient.loop();
    }
  }
}

void NetworkManager::connectWifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(_ssid);
  WiFi.begin(_ssid, _password);
}

void NetworkManager::connectMqtt() {
  Serial.print("Connecting to MQTT: ");
  Serial.println(_mqttServer);
  String clientId = "ESP32S3-Display-";
  clientId += String(random(0xffff), HEX);

  if (_mqttClient.connect(clientId.c_str(), _mqttUser, _mqttPassword)) {
    Serial.println("MQTT Connected");
    _mqttClient.subscribe(USER_MQTT_TOPIC_LIGHT_SET);
    publishDiscovery();
  } else {
    Serial.print("MQTT Failed, rc=");
    Serial.println(_mqttClient.state());
  }
}

bool NetworkManager::isWifiConnected() { return WiFi.status() == WL_CONNECTED; }

bool NetworkManager::isMqttConnected() { return _mqttClient.connected(); }

void NetworkManager::publish(const char *topic, const char *payload) {
  if (isMqttConnected()) {
    _mqttClient.publish(topic, payload);
    Serial.printf("MQTT PUB: %s -> %s\n", topic, payload);
  }
}

void NetworkManager::publishDiscovery() {
  if (!_mqttClient.connected())
    return;

  Serial.println("Sending HA Discovery...");
  DynamicJsonDocument doc(1024);

  doc["name"] = "Living Room Light (Display)";
  doc["unique_id"] = "living_room_light_display_s3";
  doc["cmd_t"] = USER_MQTT_TOPIC_LIGHT_SET;
  doc["stat_t"] = USER_MQTT_TOPIC_LIGHT_SET;
  doc["pl_on"] = "ON";
  doc["pl_off"] = "OFF";
  doc["opt"] = true;
  doc["ret"] = true;

  JsonObject device = doc.createNestedObject("device");
  device["ids"] = "esp32_s3_display_01";
  device["name"] = "Smart Display Controller";
  device["mf"] = "Yak IoT";
  device["mdl"] = "ESP32-S3-Touch-LCD-7";
  device["sw"] = "1.0.0";

  String payload;
  serializeJson(doc, payload);

  String topic = "homeassistant/light/esp32_s3_display/light_config/config";
  _mqttClient.publish(topic.c_str(), payload.c_str(), true);
}
