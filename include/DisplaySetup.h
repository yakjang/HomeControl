#pragma once
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_RGB     _panel_instance;
    lgfx::Bus_RGB       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_GT911   _touch_instance;

public:
    LGFX(void) {
        {
            auto cfg = _bus_instance.config();
            cfg.panel = &_panel_instance;
            
            // RGB Panel Config for Waveshare 7-inch
            cfg.pin_d0  = 45; // R0
            cfg.pin_d1  = 48; // R1
            cfg.pin_d2  = 47; // R2
            cfg.pin_d3  = 21; // R3
            cfg.pin_d4  = 14; // R4
            
            cfg.pin_d5  = 5;  // G0
            cfg.pin_d6  = 6;  // G1
            cfg.pin_d7  = 7;  // G2
            cfg.pin_d8  = 15; // G3
            cfg.pin_d9  = 16; // G4
            cfg.pin_d10 = 4;  // G5
            
            cfg.pin_d11 = 8;  // B0
            cfg.pin_d12 = 3;  // B1
            cfg.pin_d13 = 46; // B2
            cfg.pin_d14 = 9;  // B3
            cfg.pin_d15 = 1;  // B4

            cfg.pin_pclk = 42;
            cfg.pin_vsync = 41;
            cfg.pin_hsync = 39;
            cfg.pin_de    = 40;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();
            cfg.memory_width  = 800; // Resolution Width
            cfg.memory_height = 480; // Resolution Height
            cfg.panel_width   = 800;
            cfg.panel_height  = 480;
            cfg.offset_x      = 0;
            cfg.offset_y      = 0;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = 2; // Backlight Pin
            cfg.invert = false;
            cfg.freq   = 12000;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        {
            auto cfg = _touch_instance.config();
            cfg.x_min      = 0;
            cfg.x_max      = 799;
            cfg.y_min      = 0;
            cfg.y_max      = 479;
            cfg.pin_int    = -1;
            cfg.pin_rst    = 38; // Touch Reset
            cfg.bus_shared = true; // Shared I2C? Usually no for this board, separate.
            // But LovyanGFX often handles I2C internally if we set pins
            cfg.i2c_port   = 1;
            cfg.i2c_addr   = 0x5D; // Or 0x14
            cfg.pin_sda    = 19;
            cfg.pin_scl    = 20;
            cfg.freq       = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};
