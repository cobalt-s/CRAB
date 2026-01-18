#include <WiFi.h>
#include <esp_now.h>

// LED pins
#define LED1 25
#define LED2 26
#define LED3 32
#define LED4 33

typedef struct message {
  uint8_t buttonID;
} message;

message receivedData;

// Correct callback signature for ESP32 IDF v5.x
void onReceive(const esp_now_recv_info *info,
               const uint8_t *incomingData,
               int len) {

  memcpy(&receivedData, incomingData, sizeof(receivedData));

  // Turn off all LEDs
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);

  // Turn on the LED corresponding to buttonID
  switch (receivedData.buttonID) {
    case 1: digitalWrite(LED1, HIGH); break;
    case 2: digitalWrite(LED2, HIGH); break;
    case 3: digitalWrite(LED3, HIGH); break;
    case 4: digitalWrite(LED4, HIGH); break;
  }

  Serial.print("Received button: ");
  Serial.println(receivedData.buttonID);
}

void setup() {
  Serial.begin(9600);
  delay(1000); // Wait for Serial Monitor

  // Setup LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  // Wi-Fi station mode
  WiFi.mode(WIFI_STA);
  delay(500); // allow MAC to initialize

  // Print MAC address (for transmitter)
  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  esp_err_t result = esp_now_init();
  if (result != ESP_OK) {
    Serial.print("ESP-NOW init failed, error code: ");
    Serial.println(result);
    return;
  }

  // Register receive callback with correct signature
  esp_now_register_recv_cb(onReceive);

  Serial.println("Receiver Ready");
}

void loop() {
  // Nothing needed; ESP-NOW callback handles LED updates
}
