#include "GuiController.h"
#include "NetworkManager.h"
#include "UserConfig.h"
#include <Arduino.h>
#include <Wire.h>

NetworkManager network;
GuiController gui;

// Global Callback Function for standard PubSubClient compatibility
void globalMqttCallback(char *topic, uint8_t *payload, unsigned int length) {
  // Convert payload to string
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  Serial.printf("MQTT RX: [%s] %s\n", topic, msg);

  // Dispatch to GUI
  gui.processMqttMessage(topic, msg);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println("   HOME CONTROL - FLAT UI EDITION        ");
  Serial.println("=========================================");

  // Also try I2C Reset just in case (Address 0x20)
  Wire.begin(8, 9);

  // Config 0x20 Output (XL9535)
  Wire.beginTransmission(0x20);
  Wire.write(0x06);
  Wire.write(0x00);
  Wire.endTransmission();

  // Reset Sequence
  Wire.beginTransmission(0x20);
  Wire.write(0x02);
  Wire.write(0xF7);
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x20);
  Wire.write(0x02);
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(200);

  // Initialize GUI
  Serial.println("Initializing Display Driver...");
  gui.begin();
  gui.updateBacklight(USER_DISPLAY_BRIGHTNESS_DEFAULT);
  gui.createUI();

  // Initialize Network
  Serial.println("Initializing Network...");
  network.begin(USER_WIFI_SSID, USER_WIFI_PASSWORD, USER_MQTT_SERVER,
                USER_MQTT_PORT, USER_MQTT_USER, USER_MQTT_PASSWORD);

  // Register Global Callback
  network.setMqttCallback(globalMqttCallback);

  // Link Network to GUI (for publishing)
  gui.setNetworkManager(&network);

  Serial.println("Setup Complete.");
}

void loop() {
  network.update();
  gui.update();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    gui.updateStatus(network.isWifiConnected(), network.isMqttConnected(),
                     "12:00");
  }
  delay(5);
}
