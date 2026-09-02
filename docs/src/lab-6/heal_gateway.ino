#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 13

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

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    MeshPacket pkt;
    memcpy(&pkt, data, sizeof(pkt));

    totalPackets++;
    Serial.printf("[GW] #%lu | %s | T=%.1f H=%.1f L=%d hop=%d\n",
                  totalPackets, pkt.origin_site,
                  pkt.temperature, pkt.humidity,
                  pkt.light, pkt.hop_count);

    digitalWrite(LED_PIN, HIGH); delay(50); digitalWrite(LED_PIN, LOW);
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
