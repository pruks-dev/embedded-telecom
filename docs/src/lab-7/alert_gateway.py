"""
Alert Gateway — Lab 7.2
รับ Telemetry → ตรวจจับ Anomaly → publish Alarm กลับไป MQTT

การใช้งาน:
    python3 alert_gateway.py

ต้องติดตั้ง:
    pip install paho-mqtt
"""
import json
import statistics
import time

import paho.mqtt.client as mqtt

# TODO: เปลี่ยนเป็นค่าของนักศึกษา
STUDENT_ID = "66010001"
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
TOPIC_TELEMETRY = f"kmutnb/{STUDENT_ID}/telemetry"
TOPIC_ALARM = f"kmutnb/{STUDENT_ID}/alarm"

BASELINE_SIZE = 30
WARNING_Z = 2.0
CRITICAL_Z = 3.0

temp_history = []
hum_history = []
light_history = []

baseline_ready = False
temp_mean = temp_std = 0.0
hum_mean = hum_std = 0.0
light_mean = light_std = 0.0


def z_score(value, mean, std):
    if std == 0:
        return 0.0
    return (value - mean) / std


def classify(z):
    if z >= CRITICAL_Z:
        return "CRITICAL"
    elif z >= WARNING_Z:
        return "WARNING"
    else:
        return "NORMAL"


def on_message(client, userdata, msg):
    global baseline_ready
    global temp_mean, temp_std, hum_mean, hum_std, light_mean, light_std

    try:
        data = json.loads(msg.payload.decode())
    except json.JSONDecodeError:
        print("[ERROR] JSON ไม่ถูกต้อง:", msg.payload)
        return

    temp = data.get("temperature", 0.0)
    hum = data.get("humidity", 0.0)
    light = data.get("light", 0)

    if not baseline_ready:
        temp_history.append(temp)
        hum_history.append(hum)
        light_history.append(light)

        if len(temp_history) >= BASELINE_SIZE:
            temp_mean = statistics.mean(temp_history)
            temp_std = statistics.stdev(temp_history)
            hum_mean = statistics.mean(hum_history)
            hum_std = statistics.stdev(hum_history)
            light_mean = statistics.mean(light_history)
            light_std = statistics.stdev(light_history)
            baseline_ready = True
            print(f"[BASELINE] temp: mean={temp_mean:.1f} std={temp_std:.1f} | "
                  f"hum: mean={hum_mean:.1f} std={hum_std:.1f} | "
                  f"light: mean={light_mean:.0f} std={light_std:.0f}")
        return

    z_temp = z_score(temp, temp_mean, temp_std)
    z_hum = z_score(hum, hum_mean, hum_std)
    z_light = z_score(light, light_mean, light_std)

    # หา sensor ที่ผิดปกติมากที่สุด
    sensors = {"temperature": z_temp, "humidity": z_hum, "light": z_light}
    worst = max(sensors, key=lambda k: abs(sensors[k]))
    z_max = abs(sensors[worst])
    level = classify(z_max)

    ts = time.strftime("%H:%M:%S")
    print(f"[{ts}] temp={temp:.1f} hum={hum:.1f} light={light} | "
          f"Z={z_max:.1f} | {level}")

    # publish alarm เฉพาะเมื่อผิดปกติ (WARNING/CRITICAL)
    if level != "NORMAL":
        alarm = {
            "site_id": "A-01",
            "alarm_level": level,
            "z_score": round(z_max, 2),
            "sensor": worst,
            "value": data.get(worst, 0),
            "timestamp": int(time.time()),
        }
        client.publish(TOPIC_ALARM, json.dumps(alarm))
        print(f"  → publish alarm: {json.dumps(alarm)}")


def main():
    client = mqtt.Client()
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.subscribe(TOPIC_TELEMETRY)
    print(f"Subscribed: {TOPIC_TELEMETRY}")
    print(f"Publish alarm to: {TOPIC_ALARM}")
    print(f"กำลังเก็บ baseline {BASELINE_SIZE} ค่าแรก...")
    client.loop_forever()


if __name__ == "__main__":
    main()