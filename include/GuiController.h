#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include "DisplaySetup.h"

class GuiController {
public:
    void begin();
    void createUI();
    void update();
    void updateStatus(bool wifi, bool mqtt, String timeStr);

private:
    static void disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p);
    static void touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);
    
    // UI Helpers
    lv_obj_t* createButton(lv_obj_t* parent, const char* label, int x, int y, int w, int h, lv_color_t color);
    void applyCyberpunkTheme();

    lv_obj_t* _statusBar;
    lv_obj_t* _statusLabel;
    lv_obj_t* _wifiIcon;
    lv_obj_t* _mqttIcon;
};
