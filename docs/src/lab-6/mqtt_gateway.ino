#include <esp_now.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_PIN 13

// ============================================================
// กำหนดค่า WiFi และ MQTT — ปรับตามการทดลองของนักศึกษา
// ============================================================
const char* ssid        = "YOUR_SSID";
const char* password    = "YOUR_PASSWORD";
const char* mqtt_server = "broker.hivemq.com";
const int   mqtt_port   = 1883;
const char* client_id   = "mesh_gateway_66010001";  // เปลี่ยนเป็นรหัสนักศึกษา
const char* topic_tlm   = "kmutnb/66010001/mesh/telemetry";  // Telemetry topic
// ============================================================

typedef struct {
    uint8_t  src_mac[6];
    uint8_t  dst_mac[6];
    uint8_t  msg_type;
    uint8_t  hop_count;
    uint8_t  max_hop;
    uint32_t packet_id;
    float    temperature;
    float    humidity;
    int      light;
    char     origin_site[16];
} MeshPacket;

WiFiClient espClient;
PubSubClient mqtt(espClient);

uint32_t totalPackets = 0;

void reconnect() {
    while (!mqtt.connected()) {
        Serial.print("Connecting MQTT...");
        if (mqtt.connect(client_id)) {
            Serial.println(" connected");
        } else {
            Serial.print(" failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" retry in 5s");
            delay(5000);
        }
    }
}

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    MeshPacket pkt;
    memcpy(&pkt, data, sizeof(pkt));

    totalPackets++;

    // แปลง ESP-NOW packet → MQTT JSON payload
    StaticJsonDocument<200> doc;
    doc["site_id"]    = pkt.origin_site;
    doc["temperature"] = pkt.temperature;
    doc["humidity"]    = pkt.humidity;
    doc["light"]       = pkt.light;
    doc["hop_count"]   = pkt.hop_count;
    doc["packet_id"]   = pkt.packet_id;

    String payload;
    serializeJson(doc, payload);

    // publish ไป MQTT Broker
    mqtt.publish(topic_tlm, payload.c_str());

    Serial.printf("[GW] #%lu | %s | T=%.1f H=%.1f L=%d hop=%d → MQTT\n",
                  totalPackets, pkt.origin_site,
                  pkt.temperature, pkt.humidity,
                  pkt.light, pkt.hop_count);

    digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    // เชื่อมต่อ WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

    // ตั้งค่า MQTT
    mqtt.setServer(mqtt_server, mqtt_port);

    // ตั้งค่า ESP-NOW
    esp_now_init();
    esp_now_register_recv_cb(onDataRecv);

    Serial.println("MQTT Gateway Ready — MAC: " + WiFi.macAddress());
}

void loop() {
    if (!mqtt.connected()) reconnect();
    mqtt.loop();
    delay(100);
}
