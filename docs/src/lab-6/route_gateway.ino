#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 13
#define MAX_HOP 3

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

uint32_t totalPackets = 0;

void addPeer(uint8_t *mac) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, mac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
}

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    MeshPacket rx;
    memcpy(&rx, data, sizeof(rx));

    if (rx.msg_type == 0x02) {  // RREQ — มี Node ขอหาเส้นทาง
        // ตอบ RREP กลับไปยังต้นทาง
        MeshPacket rep;
        WiFi.macAddress(rep.src_mac);
        memcpy(rep.dst_mac, rx.src_mac, 6);
        rep.msg_type  = 0x03;   // RREP
        rep.hop_count = 1;
        rep.max_hop   = MAX_HOP;
        rep.packet_id = rx.packet_id;

        addPeer(rx.src_mac);    // เพิ่ม Sensor เป็น peer
        esp_now_send(rx.src_mac, (uint8_t *)&rep, sizeof(rep));
        Serial.printf("[GW] RREQ from %s → RREP sent\n", rx.origin_site);
        digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
    }
    else if (rx.msg_type == 0x01) {  // DATA
        totalPackets++;
        Serial.printf("[GW] #%lu | %s | T=%.1f H=%.1f L=%d hop=%d\n",
                      totalPackets, rx.origin_site,
                      rx.temperature, rx.humidity,
                      rx.light, rx.hop_count);
        digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    esp_now_init();
    esp_now_register_recv_cb(onDataRecv);

    Serial.println("Gateway Ready — MAC: " + WiFi.macAddress());
}

void loop() {
    delay(100);
}
