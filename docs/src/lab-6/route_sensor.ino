#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2
#define MAX_HOP 3

// Broadcast MAC — ส่งให้ทุก Node ในระยะ
uint8_t broadcastMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

uint8_t gatewayMAC[6];       // เก็บ MAC ของ Gateway ที่ค้นพบ
bool    hasRoute = false;    // มีเส้นทางไป Gateway แล้วหรือยัง

void addPeer(uint8_t *mac) {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, mac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
}

void onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
    Serial.printf("[Sensor] %s | #%lu\n",
                  status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL", pktId - 1);
}

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    MeshPacket rx;
    memcpy(&rx, data, sizeof(rx));

    // รับ RREP จาก Gateway — เก็บเส้นทาง
    if (rx.msg_type == 0x03) {  // RREP
        memcpy(gatewayMAC, rx.src_mac, 6);
        hasRoute = true;
        addPeer(gatewayMAC);
        Serial.printf("[Sensor] Route found → Gateway MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                      gatewayMAC[0], gatewayMAC[1], gatewayMAC[2],
                      gatewayMAC[3], gatewayMAC[4], gatewayMAC[5]);
        digitalWrite(LED_PIN, HIGH); delay(100); digitalWrite(LED_PIN, LOW);
    }
}

void sendRouteRequest() {
    WiFi.macAddress(pkt.src_mac);
    memset(pkt.dst_mac, 0xFF, 6);   // broadcast
    pkt.msg_type  = 0x02;           // RREQ
    pkt.hop_count = 1;
    pkt.max_hop   = MAX_HOP;
    pkt.packet_id = pktId++;
    strcpy(pkt.origin_site, "Site-01");

    esp_now_send(broadcastMAC, (uint8_t *)&pkt, sizeof(pkt));
    Serial.println("[Sensor] RREQ sent (broadcast) — searching for Gateway...");
}

void sendData() {
    if (!hasRoute) {
        Serial.println("[Sensor] No route yet — sending RREQ");
        sendRouteRequest();
        return;
    }

    WiFi.macAddress(pkt.src_mac);
    memcpy(pkt.dst_mac, gatewayMAC, 6);
    pkt.msg_type  = 0x01;           // DATA
    pkt.hop_count = 1;
    pkt.max_hop   = MAX_HOP;
    pkt.packet_id = pktId++;
    pkt.temperature = 25.0 + random(-50, 50) / 10.0;
    pkt.humidity    = 60.0 + random(-200, 200) / 10.0;
    pkt.light       = random(0, 4095);
    strcpy(pkt.origin_site, "Site-01");

    esp_now_send(gatewayMAC, (uint8_t *)&pkt, sizeof(pkt));
    Serial.printf("[Sensor] DATA → Gateway | T=%.1f H=%.1f L=%d\n",
                  pkt.temperature, pkt.humidity, pkt.light);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    esp_now_init();
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    addPeer(broadcastMAC);   // ต้องมี peer broadcast เพื่อส่ง RREQ

    Serial.println("Sensor Ready — MAC: " + WiFi.macAddress());
}

void loop() {
    sendData();
    delay(3000);
}
