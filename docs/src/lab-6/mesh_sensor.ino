#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2
#define MAX_HOP 3

// ============================================================
// เปลี่ยน MAC ของ Relay ให้ตรงกับ Serial Monitor
// ============================================================
uint8_t relayMAC[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
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

void onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
    Serial.printf("[Sensor] %s | #%lu\n",
                  status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL", pktId - 1);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    esp_now_init();
    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, relayMAC, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    Serial.println("Sensor Ready — MAC: " + WiFi.macAddress());
}

void loop() {
    WiFi.macAddress(pkt.src_mac);
    memcpy(pkt.dst_mac, relayMAC, 6);
    pkt.msg_type  = 0x01;   // DATA
    pkt.hop_count = 1;
    pkt.max_hop   = MAX_HOP;
    pkt.packet_id = pktId++;
    pkt.temperature = 25.0 + random(-50, 50) / 10.0;
    pkt.humidity    = 60.0 + random(-200, 200) / 10.0;
    pkt.light       = random(0, 4095);
    strcpy(pkt.origin_site, "Site-01");

    esp_now_send(relayMAC, (uint8_t *)&pkt, sizeof(pkt));
    Serial.printf("[Sensor] T=%.1f H=%.1f L=%d\n",
                  pkt.temperature, pkt.humidity, pkt.light);

    delay(3000);
}
