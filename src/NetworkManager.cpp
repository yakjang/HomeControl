#include "NetworkManager.h"

NetworkManager::NetworkManager() : _wifiClient(), _mqttClient(_wifiClient) {
  _lastWifiCheck = 0;
  _lastMqttCheck = 0;
}

void NetworkManager::begin(const char *ssid, const char *password,
                           const char *mqttServer, int mqttPort) {
  _ssid = ssid;
  _password = password;
  _mqttServer = mqttServer;
  _mqttPort = mqttPort;

  WiFi.mode(WIFI_STA);
  connectWifi();

  _mqttClient.setServer(_mqttServer, _mqttPort);
}

void NetworkManager::setMqttCallback(
    std::function<void(char *, uint8_t *, unsigned int)> callback) {
  _mqttClient.setCallback(callback);
}

void NetworkManager::update() {
  // Check WiFi
  if (millis() - _lastWifiCheck > 5000) {
    _lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
  }

  // Check MQTT
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
  String clientId = "ESP32S3-Client-";
  clientId += String(random(0xffff), HEX);
  if (_mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT Connected");
    _mqttClient.subscribe("home/livingroom/light/set");
  } else {
    Serial.print("MQTT Failed, rc=");
    Serial.println(_mqttClient.state());
  }
}

bool NetworkManager::isWifiConnected() { return WiFi.status() == WL_CONNECTED; }

bool NetworkManager::isMqttConnected() { return _mqttClient.connected(); }
