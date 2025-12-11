#include "GuiController.h"
#include "UserConfig.h"

static LGFX tft;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[800 * 10];

void GuiController::disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                               lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void GuiController::touchpad_read(lv_indev_drv_t *indev_driver,
                                  lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);

  if (touched) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void GuiController::begin() {
  tft.begin();
  tft.setRotation(0);
  tft.setBrightness(255);
  tft.setSwapBytes(true);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, 800 * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 800;
  disp_drv.ver_res = 480;
  disp_drv.flush_cb = disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_drv);
}

void GuiController::createUI() {
  // THEME: FLAT & FAST (Performance Optimized)
  // No Gradients, No Shadows, No Opacity, No Borders

  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0); // Pure Black

  // Status Bar (Simple Grey Bar)
  _statusBar = lv_obj_create(scr);
  lv_obj_set_size(_statusBar, 800, 40);
  lv_obj_align(_statusBar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(_statusBar, lv_color_hex(0x202020), 0);
  lv_obj_set_style_border_width(_statusBar, 0, 0);
  lv_obj_set_style_radius(_statusBar, 0, 0);
  lv_obj_clear_flag(_statusBar, LV_OBJ_FLAG_SCROLLABLE);

  _statusLabel = lv_label_create(_statusBar);
  lv_label_set_text(_statusLabel, "System Ready");
  lv_obj_align(_statusLabel, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0xAAAAAA), 0);

  // Light Button (Simple Square)
  _btnLight = lv_btn_create(scr);
  lv_obj_set_size(_btnLight, 200, 200);
  lv_obj_align(_btnLight, LV_ALIGN_CENTER, -50, 0);
  lv_obj_add_event_cb(_btnLight, btn_event_cb, LV_EVENT_ALL, this);
  lv_obj_add_flag(_btnLight, LV_OBJ_FLAG_CHECKABLE);

  // Default Style (OFF) - Dark Grey
  lv_obj_set_style_bg_color(_btnLight, lv_color_hex(0x333333), 0);
  lv_obj_set_style_radius(_btnLight, 8, 0);
  lv_obj_set_style_shadow_width(_btnLight, 0, 0); // No Shadow

  _labelLight = lv_label_create(_btnLight);
  lv_label_set_text(_labelLight, "LIGHT OFF");
  lv_obj_center(_labelLight);
  lv_obj_set_style_text_font(_labelLight, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(_labelLight, lv_color_hex(0xFFFFFF), 0);

  // Brightness Slider (Simple Bar)
  _sliderBrightness = lv_slider_create(scr);
  lv_obj_set_size(_sliderBrightness, 40, 200);
  lv_obj_align(_sliderBrightness, LV_ALIGN_CENTER, 150, 0);
  lv_slider_set_range(_sliderBrightness, 10, 255);
  lv_slider_set_value(_sliderBrightness, USER_DISPLAY_BRIGHTNESS_DEFAULT,
                      LV_ANIM_OFF);
  lv_obj_add_event_cb(_sliderBrightness, slider_event_cb,
                      LV_EVENT_VALUE_CHANGED, this);

  lv_obj_set_style_bg_color(_sliderBrightness, lv_color_hex(0x333333),
                            LV_PART_MAIN);
  lv_obj_set_style_bg_color(_sliderBrightness, lv_color_hex(0xFFFFFF),
                            LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(_sliderBrightness, lv_color_hex(0xAAAAAA),
                            LV_PART_KNOB);
}

void GuiController::btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  GuiController *gui = (GuiController *)lv_event_get_user_data(e);

  if (code == LV_EVENT_VALUE_CHANGED) {
    bool state = lv_obj_has_state(btn, LV_STATE_CHECKED);

    if (state) {
      // ON: Green
      lv_label_set_text(gui->_labelLight, "LIGHT ON");
      lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA00), 0);
      if (gui->_network)
        gui->_network->publish(USER_MQTT_TOPIC_LIGHT_SET, "ON");
    } else {
      // OFF: Grey
      lv_label_set_text(gui->_labelLight, "LIGHT OFF");
      lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), 0);
      if (gui->_network)
        gui->_network->publish(USER_MQTT_TOPIC_LIGHT_SET, "OFF");
    }
  }
}

void GuiController::slider_event_cb(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  GuiController *gui = (GuiController *)lv_event_get_user_data(e);
  int val = lv_slider_get_value(slider);
  gui->updateBacklight((uint8_t)val);
}

void GuiController::update() { lv_timer_handler(); }

void GuiController::updateStatus(bool wifi, bool mqtt, String timeStr) {
  String status = "WiFi: " + String(wifi ? "OK" : "--") +
                  " | MQTT: " + String(mqtt ? "OK" : "--");
  lv_label_set_text(_statusLabel, status.c_str());
}

void GuiController::updateBacklight(uint8_t brightness) {
  tft.setBrightness(brightness);
}

void GuiController::setNetworkManager(NetworkManager *network) {
  _network = network;
}

void GuiController::processMqttMessage(const char *topic, const char *payload) {
  String msg = String(payload);
  if (msg == "ON") {
    lv_obj_add_state(_btnLight, LV_STATE_CHECKED);
    lv_label_set_text(_labelLight, "LIGHT ON");
    lv_obj_set_style_bg_color(_btnLight, lv_color_hex(0x00AA00), 0);
  } else {
    lv_obj_clear_state(_btnLight, LV_STATE_CHECKED);
    lv_label_set_text(_labelLight, "LIGHT OFF");
    lv_obj_set_style_bg_color(_btnLight, lv_color_hex(0x333333), 0);
  }
}
