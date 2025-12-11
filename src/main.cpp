#include "GuiController.h"
#include "NetworkManager.h"
#include <Arduino.h>


NetworkManager network;
GuiController gui;

void setup() {
  Serial.begin(115200);
  // Wait for serial if needed, but usually skip for standalone
  // while(!Serial) delay(10);

  Serial.println("System Booting...");

  // Initialize GUI & Display
  Serial.println("Initializing Display & GUI...");
  gui.begin();
  gui.createUI();

  // Initialize Network
  Serial.println("Initializing Network...");
  network.begin("YourSSID", "YourPass", "192.168.1.100", 1883);

  Serial.println("Setup Complete.");
}

void loop() {
  network.update();
  gui.update();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    // Update Status Bar
    gui.updateStatus(network.isWifiConnected(), network.isMqttConnected(),
                     "12:00:00");
  }

  delay(5); // Yield
}
