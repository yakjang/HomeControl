#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class NetworkManager {
public:
    NetworkManager();
    void begin(const char* ssid, const char* password, const char* mqttServer, int mqttPort);
    void update();
    bool isWifiConnected();
    bool isMqttConnected();
    void setMqttCallback(std::function<void(char*, uint8_t*, unsigned int)> callback);

private:
    const char* _ssid;
    const char* _password;
    const char* _mqttServer;
    int _mqttPort;

    WiFiClient _wifiClient;
    PubSubClient _mqttClient;

    unsigned long _lastWifiCheck;
    unsigned long _lastMqttCheck;
    
    void connectWifi();
    void connectMqtt();
};
