#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2
#define MAX_HOP 3
#define ACK_TIMEOUT 2000   // รอ ACK 2 วินาที

// ============================================================
// เปลี่ยน MAC ของ Relay A และ Relay B ให้ตรงกับ Serial Monitor
// ============================================================
uint8_t relayAMAC[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
uint8_t relayBMAC[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
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

MeshPacket pkt;
uint32_t pktId = 0;

uint8_t *activeRelay = relayAMAC;   // เส้นทางหลักเริ่มต้น
bool    ackReceived = false;
unsigned long lastSendTime = 0;

void addPeer(uint8_t *mac) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, mac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
}

void onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        // ส่งสำเร็จ — รอ ACK
        lastSendTime = millis();
        ackReceived = false;
    } else {
        // ส่งไม่สำเร็จ — Relay น่าจะล่ม → สลับเส้นทางทันที
        Serial.println("[Sensor] Send FAIL — switching route!");
        switchRoute();
    }
}

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    MeshPacket rx;
    memcpy(&rx, data, sizeof(rx));

    if (rx.msg_type == 0x06) {  // ACK
        ackReceived = true;
        Serial.printf("[Sensor] ACK received from %02X:%02X:%02X:%02X:%02X:%02X\n",
                      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
    }
}

void switchRoute() {
    if (activeRelay == relayAMAC) {
        activeRelay = relayBMAC;
        Serial.println("[Sensor] → Switched to Relay B (backup)");
    } else {
        activeRelay = relayAMAC;
        Serial.println("[Sensor] → Switched to Relay A (primary)");
    }
    addPeer(activeRelay);
}

void sendData() {
    WiFi.macAddress(pkt.src_mac);
    memcpy(pkt.dst_mac, activeRelay, 6);
    pkt.msg_type  = 0x01;   // DATA
    pkt.hop_count = 1;
    pkt.max_hop   = MAX_HOP;
    pkt.packet_id = pktId++;
    pkt.temperature = 25.0 + random(-50, 50) / 10.0;
    pkt.humidity    = 60.0 + random(-200, 200) / 10.0;
    pkt.light       = random(0, 4095);
    strcpy(pkt.origin_site, "Site-01");

    esp_now_send(activeRelay, (uint8_t *)&pkt, sizeof(pkt));
    Serial.printf("[Sensor] DATA → %s | T=%.1f H=%.1f L=%d\n",
                  activeRelay == relayAMAC ? "Relay A" : "Relay B",
                  pkt.temperature, pkt.humidity, pkt.light);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    esp_now_init();
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    addPeer(relayAMAC);
    addPeer(relayBMAC);

    Serial.println("Sensor Ready — MAC: " + WiFi.macAddress());
}

void loop() {
    sendData();

    // รอ ACK — ถ้าไม่มาในเวลาที่กำหนด → สลับเส้นทาง
    unsigned long start = millis();
    while (millis() - start < ACK_TIMEOUT) {
        if (ackReceived) break;
        delay(10);
    }

    if (!ackReceived) {
        Serial.println("[Sensor] ACK timeout — switching route!");
        switchRoute();
    }

    delay(1000);
}
