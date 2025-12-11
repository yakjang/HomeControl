#include "GuiController.h"
#include "NetworkManager.h"
#include <Arduino.h>
#include <Wire.h>

NetworkManager network;
GuiController gui;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println("   USER CONFIG MODE: GPIO 6 BACKLIGHT    ");
  Serial.println("=========================================");

  // FORCE BACKLIGHT PIN 6 HIGH IMMEDIATELY
  // Removed manual control to allow LGFX PWM to take over
  // pinMode(6, OUTPUT);
  // digitalWrite(6, HIGH);

  // Also try I2C Reset just in case (Address 0x20)
  Wire.begin(8, 9);
  Serial.println("Pulse Reset on 0x20...");

  // Config 0x20 Output
  // XL9535 Config Reg 6
  Wire.beginTransmission(0x20);
  Wire.write(0x06);
  Wire.write(0x00);
  Wire.endTransmission();

  // Assert Reset (IO3 Low)
  Wire.beginTransmission(0x20);
  Wire.write(0x02);
  Wire.write(0xF7);
  Wire.endTransmission();
  delay(100);
  // Release Reset (IO3 High)
  Wire.beginTransmission(0x20);
  Wire.write(0x02);
  Wire.write(0xFF);
  Wire.endTransmission();
  delay(200);

  // Initialize GUI
  // Note: DisplaySetup.h now manages Pin 6 too, but our manual digitalWrite
  // helps during boot.
  Serial.println("Initializing Display Driver...");
  gui.begin();
  gui.updateBacklight(255); // Ensure Max Brightness
  gui.createUI();

  // Initialize Network
  Serial.println("Initializing Network...");
  // Credentials provided by User
  network.begin("YAK_IOT", "badbadbadb", "192.168.1.133", 1883, "yak", "yak");

  // Link Network to GUI
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
