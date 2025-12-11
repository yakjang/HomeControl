#pragma once

// ==========================================
//          USER CONFIGURATION
// ==========================================

// --- WiFi Settings ---
#define USER_WIFI_SSID "YAK_IOT"
#define USER_WIFI_PASSWORD "badbadbadb"

// --- MQTT Settings ---
#define USER_MQTT_SERVER "192.168.1.133"
#define USER_MQTT_PORT 1883
#define USER_MQTT_USER "yak"
#define USER_MQTT_PASSWORD "yak"

// ** IMPORTANT: REPLACE THIS WITH YOUR REAL TOPIC **
// Example: "home/livingroom/main_light/set"
#define USER_MQTT_TOPIC_LIGHT_SET "home/livingroom/light/set"
#define USER_MQTT_TOPIC_LIGHT_STATE "home/livingroom/light/status"

// --- Display Settings ---
#define USER_DISPLAY_BRIGHTNESS_DEFAULT 200
