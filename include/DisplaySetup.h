#pragma once

#include <Arduino.h>
// #define LGFX_USE_V1 // Already defined globally or in platformio.ini
// typically
#include <LovyanGFX.hpp>

// Explicitly include ESP32-S3 specific RGB headers
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::v1::Panel_RGB _panel_instance;
  lgfx::v1::Bus_RGB _bus_instance;
  lgfx::v1::Light_PWM _light_instance;
  lgfx::v1::Touch_GT911 _touch_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      // RGB Panel Pinout for Waveshare ESP32-S3-Touch-LCD-7 Rev 1.1/1.2
      cfg.pin_d0 = 1;  // R0
      cfg.pin_d1 = 2;  // R1
      cfg.pin_d2 = 42; // R2
      cfg.pin_d3 = 41; // R3
      cfg.pin_d4 = 40; // R4

      cfg.pin_d5 = 39;  // G0
      cfg.pin_d6 = 0;   // G1
      cfg.pin_d7 = 45;  // G2
      cfg.pin_d8 = 48;  // G3
      cfg.pin_d9 = 47;  // G4
      cfg.pin_d10 = 21; // G5

      cfg.pin_d11 = 14; // B0
      cfg.pin_d12 = 38; // B1
      cfg.pin_d13 = 18; // B2
      cfg.pin_d14 = 17; // B3
      cfg.pin_d15 = 10; // B4

      cfg.pin_pclk = 7;
      cfg.pin_vsync = 3;
      cfg.pin_hsync = 46;
      cfg.pin_henable = 5; // DE

      // Timing parameters for Waveshare 7-inch 800x480
      // Timing parameters for Waveshare 7-inch 800x480 (User Config)
      // These must be in the Bus Config for RGB Panel
      cfg.freq_write = 14000000; // 14MHz
      cfg.hsync_pulse_width = 10;
      cfg.hsync_back_porch = 10;
      cfg.hsync_front_porch = 20;
      cfg.vsync_pulse_width = 10;
      cfg.vsync_back_porch = 10;
      cfg.vsync_front_porch = 10;
      cfg.hsync_polarity = 0;
      cfg.vsync_polarity = 0;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = 800;
      cfg.memory_height = 480;
      cfg.panel_width = 800;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 6;      // User says GPIO 6!
      cfg.invert = false;  // Standard Polarity usually
      cfg.freq = 5000;     // 5kHz is safer/standard
      cfg.pwm_channel = 0; // standard channel
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0;
      cfg.x_max = 799;
      cfg.y_min = 0;
      cfg.y_max = 479;
      cfg.pin_int = -1;      // User Config says -1
      cfg.pin_rst = -1;      // User Config says -1
      cfg.bus_shared = true; // Shared I2C
      cfg.i2c_port = 0;      // Port 0 for IO8/9
      cfg.i2c_addr = 0x5D;   // GT911 Address
      cfg.pin_sda = 8;
      cfg.pin_scl = 9;
      cfg.freq = 400000;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};
