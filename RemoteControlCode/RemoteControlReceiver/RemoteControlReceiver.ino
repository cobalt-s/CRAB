/*
 * ===========================================================================
 * Crab Robot - ESP32 Receiver
 * ===========================================================================
 * Authors:   Renzo. Cobalt, maybe some claude lol 
 * Modified:  4/7/2026
 *
 * Description:
 *   Receives ESP-NOW packets from the remote transmitter and routes
 *   commands to the leg controller and onboard display.
 *
 * ===========================================================================
 */

#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>

// ---------------------------------------------------------------------------
// Debug LED Pin Definitions
// ---------------------------------------------------------------------------
#define LED1 25
#define LED2 26
#define LED3 32
#define LED4 33

// ---------------------------------------------------------------------------
// PCA9685 Pin / Address Definitions
// ---------------------------------------------------------------------------
#define PCA9685_I2C_ADDR  0x40   // Default I2C address (A0-A5 all LOW)
#define PCA9685_SDA_PIN   21     // I2C SDA
#define PCA9685_SCL_PIN   22     // I2C SCL

// PCA9685 channel assignments — 4 legs × 3 DOF = 12 channels (4 channels spare)
// Naming: LEG_<number>_<joint>  (COXA = hip, FEMUR = shoulder, TIBIA = knee)
#define LEG1_COXA_CH   0
#define LEG1_FEMUR_CH  1
#define LEG1_TIBIA_CH  2

#define LEG2_COXA_CH   3
#define LEG2_FEMUR_CH  4
#define LEG2_TIBIA_CH  5

#define LEG3_COXA_CH   6
#define LEG3_FEMUR_CH  7
#define LEG3_TIBIA_CH  8

#define LEG4_COXA_CH   9
#define LEG4_FEMUR_CH  10
#define LEG4_TIBIA_CH  11

// Channels 12–15 are available for future use (claw, camera pan/tilt, etc.)

// ---------------------------------------------------------------------------
// ESP-NOW Message Format
// ---------------------------------------------------------------------------

/**
 * @brief Packet structure sent by the remote transmitter.
 *
 * Keep this struct identical on both transmitter and receiver.
 */
typedef struct {
  uint8_t buttonID;   ///< ID of the button pressed (1–4 currently)
} RemotePacket;

RemotePacket receivedData;

// ---------------------------------------------------------------------------
// Forward Declarations
// ---------------------------------------------------------------------------
void initLEDs();
void initESPNow();
void initLegs();
void initDisplay();
void setAllLEDs(bool state);
void indicateButton(uint8_t buttonID);
void setLegAngle(uint8_t legIndex, uint8_t joint, float angleDeg);
void moveLegTo(uint8_t legIndex, float coxa, float femur, float tibia);
void displayStatus(const char* line1, const char* line2);
void onReceive(const esp_now_recv_info* info, const uint8_t* data, int len);

// ---------------------------------------------------------------------------
// LED Helpers
// ---------------------------------------------------------------------------

/**
 * @brief Configures all four debug LED pins as outputs and turns them off.
 */
void initLEDs() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  setAllLEDs(false);
}

/**
 * @brief Sets all four debug LEDs to the same state.
 *
 * @param state  true = HIGH (on), false = LOW (off)
 */
void setAllLEDs(bool state) {
  digitalWrite(LED1, state);
  digitalWrite(LED2, state);
  digitalWrite(LED3, state);
  digitalWrite(LED4, state);
}

/**
 * @brief Lights the LED that corresponds to a received button ID.
 *        Turns off all LEDs first to ensure only one is active.
 *
 * @param buttonID  Button index from the remote (1–4)
 */
void indicateButton(uint8_t buttonID) {
  setAllLEDs(false);
  switch (buttonID) {
    case 1: digitalWrite(LED1, HIGH); break;
    case 2: digitalWrite(LED2, HIGH); break;
    case 3: digitalWrite(LED3, HIGH); break;
    case 4: digitalWrite(LED4, HIGH); break;
    default:
      Serial.printf("[WARN] Unknown buttonID: %d\n", buttonID);
      break;
  }
}

// ---------------------------------------------------------------------------
// Leg / PCA9685 Abstraction
// ---------------------------------------------------------------------------

/**
 * @brief Initializes I2C and the PCA9685 PWM driver.
 *        Sets the PWM frequency appropriate for hobby servos (50 Hz).
 *
 * TODO: Initialize Wire with PCA9685_SDA_PIN and PCA9685_SCL_PIN.
 * TODO: Set PCA9685 mode registers (MODE1, MODE2).
 * TODO: Set PWM frequency to 50 Hz via the prescale register.
 */
void initLegs() {
  // TODO: Wire.begin(PCA9685_SDA_PIN, PCA9685_SCL_PIN);
  // TODO: PCA9685 reset + configure
  Serial.println("[Legs] PCA9685 init — TODO");
}

/**
 * @brief Commands a single servo on the PCA9685 to a target angle.
 *
 * @param legIndex   Leg number (1–4)
 * @param joint      Joint index: 0 = coxa, 1 = femur, 2 = tibia
 * @param angleDeg   Target angle in degrees (servo-range dependent)
 *
 * TODO: Map legIndex + joint to the correct PCA9685 channel (see defines above).
 * TODO: Convert angleDeg to a 12-bit PWM tick count using the servo's
 *       min/max pulse widths and the 50 Hz period.
 * TODO: Write the tick value to the PCA9685 via I2C (LEDn_ON / LEDn_OFF regs).
 */
void setLegAngle(uint8_t legIndex, uint8_t joint, float angleDeg) {
  // TODO: channel lookup table indexed by [legIndex][joint]
  // TODO: angleDeg -> pwm ticks conversion
  // TODO: Wire write to PCA9685
  Serial.printf("[Legs] setLegAngle(leg=%d, joint=%d, angle=%.1f) — TODO\n",
                legIndex, joint, angleDeg);
}

/**
 * @brief Moves a single leg to the specified joint angles in one call.
 *        Convenience wrapper around three setLegAngle() calls.
 *
 * @param legIndex  Leg number (1–4)
 * @param coxa      Coxa (hip) angle in degrees
 * @param femur     Femur (shoulder) angle in degrees
 * @param tibia     Tibia (knee) angle in degrees
 *
 * TODO: Decide whether to move joints simultaneously or sequentially.
 * TODO: Add interpolation / speed limiting if needed.
 */
void moveLegTo(uint8_t legIndex, float coxa, float femur, float tibia) {
  setLegAngle(legIndex, 0, coxa);
  setLegAngle(legIndex, 1, femur);
  setLegAngle(legIndex, 2, tibia);
}

// ---------------------------------------------------------------------------
// Display Abstraction
// ---------------------------------------------------------------------------

/**
 * @brief Initializes the onboard display (OLED / TFT — TBD).
 *
 * TODO: Include the appropriate display library header (e.g. Adafruit_SSD1306,
 *       TFT_eSPI, U8g2, etc.).
 * TODO: Call the library's begin() / init() method.
 * TODO: Clear the screen and show a splash / boot message.
 */
void initDisplay() {
  // TODO: display.begin(...)
  // TODO: display.clearDisplay()
  // TODO: display.print("Crab Bot Ready")
  Serial.println("[Display] init — TODO");
}

/**
 * @brief Writes up to two lines of status text to the display.
 *
 * @param line1  First line of text (e.g. "BTN: 3")
 * @param line2  Second line of text (e.g. "Walking Fwd")
 *
 * TODO: Clear the display (or just the text region).
 * TODO: Set cursor, font size, and print line1 / line2.
 * TODO: Call display.display() if using a buffered driver like SSD1306.
 */
void displayStatus(const char* line1, const char* line2) {
  // TODO: display.clearDisplay()
  // TODO: display.setCursor(0, 0); display.println(line1)
  // TODO: display.setCursor(0, 16); display.println(line2)
  // TODO: display.display()
  Serial.printf("[Display] '%s' | '%s' — TODO\n", line1, line2);
}

// ---------------------------------------------------------------------------
// ESP-NOW
// ---------------------------------------------------------------------------

/**
 * @brief Initializes ESP-NOW in Wi-Fi station mode and registers the
 *        receive callback. Halts with repeated error prints on failure.
 *
 * Failsafes:
 *   - If esp_now_init() fails, logs the error code and blinks all LEDs
 *     indefinitely so the fault is visible even without a serial monitor.
 *   - If registering the callback fails, the same blink loop is entered.
 */
void initESPNow() {
  WiFi.mode(WIFI_STA);
  delay(500);  // Allow MAC address to stabilize after mode switch

  Serial.print("[ESP-NOW] Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  esp_err_t initResult = esp_now_init();
  if (initResult != ESP_OK) {
    Serial.printf("[ESP-NOW] Init FAILED (err 0x%X) — check antenna / power\n",
                  initResult);
    // Blink all LEDs as a visible fault indicator
    while (true) {
      setAllLEDs(true);
      delay(200);
      setAllLEDs(false);
      delay(200);
    }
  }

  esp_err_t cbResult = esp_now_register_recv_cb(onReceive);
  if (cbResult != ESP_OK) {
    Serial.printf("[ESP-NOW] Callback registration FAILED (err 0x%X)\n",
                  cbResult);
    while (true) {
      setAllLEDs(true);
      delay(500);
      setAllLEDs(false);
      delay(500);
    }
  }

  Serial.println("[ESP-NOW] Ready");
}

/**
 * @brief ESP-NOW receive callback — fires on every incoming packet.
 *        Copies the payload into receivedData, updates the debug LEDs,
 *        and logs the button ID over serial.
 *
 * @param info  Metadata about the sender (MAC address, RSSI, etc.)
 * @param data  Raw payload bytes
 * @param len   Payload length in bytes
 */
void onReceive(const esp_now_recv_info* info, const uint8_t* data, int len) {
  if (len != sizeof(RemotePacket)) {
    Serial.printf("[ESP-NOW] Unexpected packet size: %d (expected %d)\n",
                  len, sizeof(RemotePacket));
    return;
  }

  memcpy(&receivedData, data, sizeof(receivedData));

  Serial.printf("[ESP-NOW] Button received: %d\n", receivedData.buttonID);
  indicateButton(receivedData.buttonID);

  // TODO: Route buttonID to leg movement commands via moveLegTo() / gait FSM.
  // TODO: Update display with displayStatus().
}

// ---------------------------------------------------------------------------
// Arduino Entry Points
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);  
  

  Serial.println("=== Crab Robot Receiver ===");

  initLEDs();
  initESPNow();
  initLegs();
  initDisplay();

  Serial.println("[Setup] All subsystems initialized.");
}

void loop() {
  // ESP-NOW is interrupt-driven; no polling needed here.
  // TODO: Add gait state-machine tick if walking patterns are implemented.
  // TODO: Add watchdog / heartbeat timeout to safe-stop legs if signal is lost.
}