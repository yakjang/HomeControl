# การติดตั้ง PlatformIO (สำหรับ Compile โปรเจ็คนี้)

เนื่องจากโปรเจ็คนี้ถูกเตรียมไว้สำหรับ **PlatformIO** ซึ่งเป็นเครื่องมือพัฒนา Embedded Systems ยอดนิยมที่จัดการ Library และ Toolchain ให้โดยอัตโนมัติ คุณจำเป็นต้องติดตั้งมันก่อนครับ

## วิธีติดตั้ง (ใน VS Code / Antigravity IDE)

1.  **เปิดหน้า Extensions Marketplace**:
    *   กดปุ่มลัด `Ctrl` + `Shift` + `X`
    *   หรือคลิกที่ไอคอนรูป "สี่เหลี่ยมต่อกัน" (Extensions) ที่แถบซ้ายมือสุด

2.  **ค้นหาและติดตั้ง**:
    *   พิมพ์คำว่า `PlatformIO IDE` ในช่องค้นหา
    *   เลือกรายการแรกที่เป็นของ **PlatformIO** (Official)
    *   กดปุ่ม **Install**

3.  **รอการติดตั้ง (สำคัญ!)**:
    *   หลังจากกด Install แล้ว **ต้องรอสักพัก** (ประมาณ 2-5 นาที)
    *   สังเกตที่มุมขวาล่าง จะมีข้อความว่า `PlatformIO: Installing PlatformIO Core...`
    *   **ห้ามปิดโปรแกรม** จนกว่าจะขึ้นว่าติดตั้งเสร็จเรียบร้อยและขอให้ **Reload/Restart**

4.  **ตรวจสอบความถูกต้อง**:
    *   เมื่อติดตั้งเสร็จ จะมีไอคอนรูป **หัวมดต่างดาว (Alien)** ปรากฏขึ้นที่แถบซ้ายมือ
    *   แถบด้านล่าง (Status Bar) จะมีปุ่มเครื่องหมายถูก (**✓**) และลูกศร (**→**) สำหรับ Build และ Upload

## การเริ่มใช้งานครั้งแรก

1.  คลิกที่ไอคอน **Alien** (ซ้ายมือ)
2.  เลือก **Pick a folder** หรือ **Open Project** แล้วเลือกโฟลเดอร์ `HomeControl` นี้
3.  รอให้ PlatformIO ดาวน์โหลด Library และตั้งค่าบอร์ดสักครู่ (จะเห็นขีดหมุนๆ ด้านล่าง)
4.  เมื่อพร้อมแล้ว ลองกดปุ่ม **Build (✓)** ที่แถบด้านล่างได้เลยครับ

# การตั้งค่า Home Assistant (MQTT)

เพื่อให้ปุ่ม "Living Room Light" บนหน้าจอใช้งานได้จริง คุณต้องไปตั้งค่าที่ **Home Assistant** ของคุณด้วย เพื่อให้มันรู้จัก Topic ที่เราส่งไป

## 1. ติดตั้ง MQTT Add-on
1.  ไปที่ **Settings** -> **Add-ons**
2.  ติดตั้ง **Mosquitto broker**
3.  กด **Start** และดู Log ว่าทำงานปกติไหม

## 2. เพิ่ม Config แสงไฟ (YAML)
เปิดไฟล์ `configuration.yaml` ของ Home Assistant แล้วเพิ่มโค้ดนี้ลงไป:

```yaml
mqtt:
  light:
    - name: "Living Room Light"
      unique_id: "living_room_light_display"
      command_topic: "home/livingroom/light/set"
      state_topic: "home/livingroom/light/set"  # ใช้ Topic เดิมเป็น State ไปก่อน (Optimistic Mode)
      payload_on: "ON"
      payload_off: "OFF"
      optimistic: true
      qos: 0
      retain: true
```

*หมายเหตุ: ถ้าคุณมีหลอดไฟจริง (เช่น Zigbee/WiFi Bulb) คุณต้องเขียน Automation ให้ Topic นี้ไป Trigger หลอดไฟดวงนั้นแทน*

## 3. Reload Configuration
1.  ไปที่ **Developer Tools** -> **YAML**
2.  กด **Check Configuration** (ต้องผ่านเขียว)
3.  กด **Restart** Home Assistant

## 4. ทดสอบ
- เมื่อ Restart เสร็จ จะมี Entity ชื่อ `light.living_room_light` โผล่ขึ้นมา
- ลองกดปุ่มบนหน้าจอ ESP32 -> สถานะใน Home Assistant ต้องเปลี่ยนตาม
- ลองเปลี่ยนสถานะใน Home Assistant -> หน้าจอ ESP32 ต้องเปลี่ยนสีตาม (ถ้าระบบ Subscribe ทำงานสมบูรณ์)
## 5. เชื่อมต่อปุ่มกับไฟจริง (Automation)

ตอนนี้ Home Assistant รู้จักปุ่มบนหน้าจอแล้ว (ในชื่อ `light.living_room_light_display`) แต่กดไปก็ยังไม่มีผลกับไฟจริง
วิธีมาตรฐานที่ "Pro" เขาทำกันคือการสร้าง **Automation** เพื่อสั่งงานไขว้กันครับ

### ทำไมต้อง Automation?
เพราะเราต้องการให้สถานะมัน Sync กัน 2 ทาง:
1. กดจอ -> ไฟจริงติด
2. กดสวิตช์ผนัง (หรือแอป) -> สถานะบนจอเปลี่ยนตาม

### วิธีตั้งค่า
ไปที่ **Settings** -> **Automations & Scenes** -> **Create Automation** -> **Edit in YAML** แล้วแปะโค้ดนี้:

```yaml
alias: Sync Living Room Light & Display
description: ""
mode: single
trigger:
  # 1. ถ้าไฟจริงเปลี่ยนสถานะ
  - platform: state
    entity_id: light.real_living_room_light # <--- แก้เป็นชื่อหลอดไฟจริงของคุณ
    to: null
  # 2. ถ้ากดปุ่มบนจอ
  - platform: state
    entity_id: light.living_room_light_display
    to: null
condition: []
action:
  # สั่งให้ทั้งคู่เป็นเหมือนตัวที่เพิ่งเปลี่ยน (Sync State)
  - service: light.turn_{{ trigger.to_state.state }}
    target:
      entity_id:
        - light.real_living_room_light     # <--- แก้เป็นชื่อหลอดไฟจริงของคุณ
        - light.living_room_light_display
```
