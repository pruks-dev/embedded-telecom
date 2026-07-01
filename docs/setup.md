# การติดตั้งซอฟต์แวร์

---

## 1. ติดตั้ง Arduino IDE 2.3.x

- ดาวน์โหลดได้ที่ https://www.arduino.cc/en/software
- เลือกเวอร์ชัน Arduino IDE **2.3.x** ตามระบบปฏิบัติการ (Windows / macOS / Linux)

---

## 2. เพิ่ม ESP32 Board ใน Boards Manager

- เปิด Arduino IDE → ไปที่ File → Preferences
- ในช่อง **Additional Boards Manager URLs** เพิ่ม:
  ```
  https://espressif.github.io/arduino-esp32/package_esp32_index.json
  ```
- ไปที่ Boards Manager (ไอคอนด้านซ้าย หรือ Tools → Board → Boards Manager)
- ค้นหา `ESP32` → เลือก **ESP32 by Espressif Systems** → คลิก Install

---

## 3. เลือก Board และ Port

- ไปที่ Tools → Board → ESP32 Arduino → เลือก **ESP32 Dev Module**
- เสียบ ESP32 เข้ากับคอมพิวเตอร์ผ่าน USB
- ไปที่ Tools → Port → เลือกพอร์ตที่ตรงกับ ESP32
  - Windows: `COM3`, `COM4` ฯลฯ
  - macOS: `/dev/cu.SLAB_USBtoUART` หรือ `/dev/cu.usbserial-*`
  - Linux: `/dev/ttyUSB0` หรือ `/dev/ttyACM0`
- ถ้าไม่เจอพอร์ต → ติดตั้ง Driver CP210x ( Silicon Labs CP210x USB to UART Bridge )

![ESP32 Pinout](images/esp32-pin.png)

> ศึกษา Pinout ของ ESP32 ให้เข้าใจก่อนเริ่มทดลอง — โดยเฉพาะพินที่ใช้บ่อย: **GPIO** (digital), **ADC** (analog), **SDA/SCL** (I2C), **3.3V / GND**

---

## 4. ติดตั้ง Library ที่จำเป็น

- ไปที่ Tools → Manage Libraries (หรือไอคอน Library Manager ด้านซ้าย)
- ค้นหาและติดตั้ง Library ต่อไปนี้:

  | Library | ผู้พัฒนา | ใช้ใน |
  | ------- | ------- | ----- |
  | `DHT sensor library` | Adafruit | Lab 1 |
  | `Adafruit SSD1306` | Adafruit | Lab 1 |
  | `Adafruit GFX Library` | Adafruit | Lab 1 |
  | `ArduinoJson` | Benoit Blanchon | Lab 2 เป็นต้นไป |

---

## 5. ทดสอบการทำงาน

- เปิดตัวอย่าง File → Examples → WiFi → WiFiScan
- เลือก Board เป็น **ESP32 Dev Module** และเลือก Port ที่ถูกต้อง
- อัปโหลดโปรแกรมลง ESP32
- เปิด Serial Monitor (ตั้งค่า Baud Rate เป็น **115200**)
- ถ้าเห็นรายชื่อ WiFi Access Points แสดงว่าติดตั้งและพร้อมใช้งาน
