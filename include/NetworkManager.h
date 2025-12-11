#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

// Define standard MQTT Callback Signature
typedef void (*MqttCallback)(char *, uint8_t *, unsigned int);

class NetworkManager {
public:
  NetworkManager();
  void begin(const char *ssid, const char *password, const char *mqttServer,
             int mqttPort, const char *mqttUser, const char *mqttPassword);
  void update();
  bool isWifiConnected();
  bool isMqttConnected();

  // Changed to raw function pointer to match PubSubClient
  void setMqttCallback(MqttCallback callback);

  void publish(const char *topic, const char *payload);
  void publishDiscovery();

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
