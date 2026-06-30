# ปฏิบัติการระบบฝังตัวสำหรับงานโทรคมนาคม

**Embedded System Laboratory for Telecommunications — 010113330**

---

รายวิชานี้เป็นส่วนหนึ่งของหลักสูตรวิศวกรรมไฟฟ้า
ภาควิชาวิศวกรรมไฟฟ้าและคอมพิวเตอร์ คณะวิศวกรรมศาสตร์
มหาวิทยาลัยเทคโนโลยีพระจอมเกล้าพระนครเหนือ

จัดทำโดย อาจารย์ ดร. พฤกษ์ สระศรีทอง (PRUKS)
pruk.s@eng.kmutnb.ac.th

---

รายวิชานี้เป็นวิชาต่อเนื่องจาก Microprocessor and Embedded Systems
นักศึกษามีพื้นฐานการใช้งาน ESP32, GPIO และการเขียนโปรแกรม Embedded แล้ว

ตลอดทั้งภาคการศึกษา นักศึกษาจะพัฒนาระบบ **Telecommunication Base Station Remote Monitoring System**
ซึ่งเป็นระบบตรวจสอบสถานีฐานโทรคมนาคมระยะไกลแบบสมบูรณ์ ครอบคลุมตั้งแต่
การอ่านค่าเซ็นเซอร์ การสื่อสารข้อมูล การแสดงผลแบบ Real-time ไปจนถึงการประมวลผล Edge AI

## ใบงานการทดลอง

| Lab | หัวข้อ | เทคโนโลยีหลัก |
| --- | --- | --- |
| 1 | Sensor Node | ESP32, DHT11, LDR, WiFi Web Server |
| 2 | Data Communication | WiFi, MQTT, HTTP, JSON |
| 3 | Network Operation Center (NOC) | MQTT Subscribe, Dashboard, Site Status |
| 4 | Edge Gateway | Raspberry Pi, MQTT Broker, Protocol Translation |
| 5 | Alarm System | PIR, Relay, Buzzer, OLED, Alarm Levels |
| 6 | Edge AI | Edge AI, LLM, Fault Prediction |
| 7 | Workflow Automation | n8n, LINE, Email, Incident Management |
| 8 | AIoT Station | Full System Integration |

👉 **ดูวิธีการติดตั้ง Arduino IDE และตั้งค่า ESP32 ได้ที่ [การติดตั้งซอฟต์แวร์](setup.md)**

## อุปกรณ์หลัก (Hardware)

- ESP32 Development Board
- Raspberry Pi (รุ่น 3B+/4B)
- OLED Display (I2C, 128x64)
- DHT11/22 — อุณหภูมิและความชื้น
- LDR (Light Dependent Resistor)
- Potentiometer
- PIR Motion Sensor
- Obstacle Sensor (Infrared)
- Relay Module
- Buzzer
- LED, Resistor, Breadboard, จัมเปอร์

## เครื่องมือที่ใช้ในการพัฒนา

- **[Arduino IDE](https://www.arduino.cc/en/software)** หรือ **PlatformIO** — สำหรับพัฒนาโปรแกรม ESP32
- **Python 3** — สำหรับพัฒนาโปรแกรมบน Raspberry Pi
- **Mosquitto MQTT Broker** — สำหรับทดสอบการสื่อสาร MQTT
- **n8n** — สำหรับ Workflow Automation (รันบน Raspberry Pi)
- **Node-RED** — สำหรับสร้าง Dashboard (ทางเลือก)
- **ngrok** — สำหรับทดสอบ Webhook (ไม่จำเป็น)
