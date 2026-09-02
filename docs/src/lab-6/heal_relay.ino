#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 15
#define MAX_HOP 3

// ============================================================
// เปลี่ยน MAC ของ Gateway ให้ตรงกับ Serial Monitor
// ============================================================
uint8_t gatewayMAC[] = {0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22};
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

MeshPacket incoming;

uint32_t seen_ids[100];
int seen_count = 0;

bool isDuplicate(uint32_t id) {
    for (int i = 0; i < seen_count; i++) {
        if (seen_ids[i] == id) return true;
    }
    if (seen_count < 100) seen_ids[seen_count++] = id;
    return false;
}

void addPeer(uint8_t *mac) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, mac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
}

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    memcpy(&incoming, data, sizeof(incoming));

    // ตรวจสอบ packet ซ้ำ
    if (isDuplicate(incoming.packet_id)) {
        Serial.printf("[Relay] Duplicate #%lu — dropped\n", incoming.packet_id);
        return;
    }

    // ตรวจสอบ hop count
    if (incoming.hop_count >= incoming.max_hop) {
        Serial.println("[Relay] Max hop reached — dropped");
        return;
    }

    // ส่ง ACK กลับไปยังต้นทาง (Sensor)
    MeshPacket ack;
    WiFi.macAddress(ack.src_mac);
    memcpy(ack.dst_mac, incoming.src_mac, 6);
    ack.msg_type  = 0x06;   // ACK
    ack.hop_count = 1;
    ack.max_hop   = MAX_HOP;
    ack.packet_id = incoming.packet_id;

    addPeer(incoming.src_mac);
    esp_now_send(incoming.src_mac, (uint8_t *)&ack, sizeof(ack));

    // forward DATA ไป Gateway
    incoming.hop_count++;
    esp_err_t result = esp_now_send(gatewayMAC, (uint8_t *)&incoming, sizeof(incoming));
    Serial.printf("[Relay] RECV #%lu | %s | T=%.1f H=%.1f L=%d | hop=%d/%d | →GW: %s\n",
                  incoming.packet_id, incoming.origin_site,
                  incoming.temperature, incoming.humidity, incoming.light,
                  incoming.hop_count, incoming.max_hop,
                  result == ESP_OK ? "OK" : "FAIL");

    digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    esp_now_init();
    esp_now_register_recv_cb(onDataRecv);

    addPeer(gatewayMAC);

    Serial.println("Relay Ready — MAC: " + WiFi.macAddress());
}

void loop() {
    delay(100);
}
