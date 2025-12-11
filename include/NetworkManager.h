#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class NetworkManager {
public:
  NetworkManager();
  void begin(const char *ssid, const char *password, const char *mqttServer,
             int mqttPort, const char *mqttUser, const char *mqttPassword);
  void update();
  bool isWifiConnected();
  bool isMqttConnected();
  void setMqttCallback(
      std::function<void(char *, uint8_t *, unsigned int)> callback);
  void publish(const char *topic, const char *payload);

private:
  const char *_ssid;
  const char *_password;
  const char *_mqttServer;
  int _mqttPort;
  const char *_mqttUser;
  const char *_mqttPassword;

  WiFiClient _wifiClient;
  PubSubClient _mqttClient;

  unsigned long _lastWifiCheck;
  unsigned long _lastMqttCheck;

  void connectWifi();
  void connectMqtt();
};
