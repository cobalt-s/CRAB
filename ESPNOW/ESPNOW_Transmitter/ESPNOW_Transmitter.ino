#include <WiFi.h>
#include <esp_now.h>

// Button pins
#define BTN1 13
#define BTN2 12
#define BTN3 14
#define BTN4 27

// Receiver MAC address (match with receiver address)
uint8_t receiverMAC[] = {0x44, 0x1D, 0x64, 0xF8, 0x22, 0xD8};

typedef struct message {
  uint8_t buttonID;
} message;

message sendData;

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  
  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  Serial.println("Transmitter Ready");
}

void sendButton(uint8_t id) {
  sendData.buttonID = id;
  esp_now_send(receiverMAC, (uint8_t *)&sendData, sizeof(sendData));
  Serial.print("Button sent: ");
  Serial.println(id);
  delay(300); 
}

void loop() {
  if (digitalRead(BTN1) == LOW) sendButton(1);
  if (digitalRead(BTN2) == LOW) sendButton(2);
  if (digitalRead(BTN3) == LOW) sendButton(3);
  if (digitalRead(BTN4) == LOW) sendButton(4);
}
