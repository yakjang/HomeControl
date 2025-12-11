#include "GuiController.h"

static LGFX tft;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[800 * 10]; // Buffer for 10 lines

void GuiController::disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                               lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  // tft.pushColors((uint16_t *)&color_p->full, w * h, true); // Depending on
  // color depth
  tft.writePixels((uint16_t *)&color_p->full, w * h);
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
  tft.setRotation(0); // Landscape
  tft.setBrightness(255);

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

  applyCyberpunkTheme();
}

void GuiController::applyCyberpunkTheme() {
  // Dark Background
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x050505), LV_PART_MAIN);

  // Basic Label
  _statusLabel = lv_label_create(lv_scr_act());
  lv_label_set_text(_statusLabel, "SYSTEM INIT...");
  lv_obj_align(_statusLabel, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0x00FFFF), 0);
}

void GuiController::createUI() {
  // Header
  _statusBar = lv_obj_create(lv_scr_act());
  lv_obj_set_size(_statusBar, 800, 40);
  lv_obj_set_style_bg_color(_statusBar, lv_color_hex(0x101010), 0);
  lv_obj_set_style_border_color(_statusBar, lv_color_hex(0x00FFFF), 0);
  lv_obj_set_style_border_width(_statusBar, 2, 0);
  lv_obj_align(_statusBar, LV_ALIGN_TOP_MID, 0, 0);

  lv_label_set_text(_statusLabel, "SYSTEM READY");
  lv_obj_align(_statusLabel, LV_ALIGN_CENTER, 0, 0);
}

void GuiController::update() { lv_timer_handler(); }

void GuiController::updateStatus(bool wifi, bool mqtt, String timeStr) {
  String status = "WiFi: " + String(wifi ? "ON" : "OFF") +
                  " | MQTT: " + String(mqtt ? "ON" : "OFF") + " | " + timeStr;
  lv_label_set_text(_statusLabel, status.c_str());
}
