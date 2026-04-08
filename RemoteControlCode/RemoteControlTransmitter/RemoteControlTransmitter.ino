/*
* Remote Control Code for Crab Project
* Authors: Renzo, Jimmy
* Date last modified: 4/6/2026
* TODO: 
* add buzzer implementation
* add screen implementation
*/


// Import statements for ESP-NOW
#include <WiFi.h>
#include <esp_now.h>

// Pin assignments (to be updated when physically wired)
const int PIN_JOYSTICK_X      = 0;
const int PIN_JOYSTICK_Y      = 1;
const int PIN_JOYSTICK_BTN    = 3;
const int PIN_BTN_BLUE        = 5;
const int PIN_BTN_YELLOW      = 6;
const int LED_1               = 7; 
const int LED_2               = 8;

// Receiver MAC address (must match receiver device)
const uint8_t RECEIVER_MAC[] = {0x44, 0x1D, 0x64, 0xF8, 0x22, 0xD8};

/*
 * Struct defining the data package sent to the receiver.
 * Includes button states and joystick axes.
 */
typedef struct ControllerMessage {
  bool bluePressed;   // True if the blue button is pressed
  bool yellowPressed; // True if the yellow button is pressed
  bool joyBtnPressed; // True if the joystick button is pressed
  int  joyX;          // Joystick X axis value (-2048 to 2047)
  int  joyY;          // Joystick Y axis value (-2048 to 2047)
} ControllerMessage;

ControllerMessage outgoingData;


void setup() {
  Serial.begin(9600);
  Serial.println("DEBUG: Transmitter code setup starting...");
  delay(1000);
  setupPins();
  Serial.println("DEBUG: Pins Setup");
  setupEspNow();
  Serial.println("Remote control is ready.");
}


void setupPins() {
  pinMode(PIN_JOYSTICK_X,   INPUT);
  pinMode(PIN_JOYSTICK_Y,   INPUT);
  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP); // HIGH when not pressed
  pinMode(PIN_BTN_BLUE,     INPUT_PULLUP); // HIGH when not pressed
  pinMode(PIN_BTN_YELLOW,   INPUT_PULLUP); // HIGH when not pressed
}

/*
 * Setup ESPNOw sets up the esp to connect to the other. 
 */
void setupEspNow() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("DEBUG: ESP-NOW init failed");
    pinMode(LED_1, OUTPUT);
    while (true){
      digitalWrite(LED_1, HIGH);
      delay(250);
      digitalWrite(LED_1, LOW);
      delay(250);
    }
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, RECEIVER_MAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if(esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("DEBUG: Failed to add peer");
  }

  Serial.println("DEBUG: TRANSMITTER READY");
}


/*
 * This reads the analog joystick pin and centers the output around zero. 
 * Range: -2048 to 2047 (ESP32's 12-bit ADC).
 * Note: would be -512 to 511 on Arduino's 10-bit ADC.
 */
int readJoystickCentered(int pin) {
  return (analogRead(pin) - 2048);
}


// Returns true if the button is pressed (assumes INPUT_PULLUP wiring)
bool isButtonPressed(int pin) {
  return digitalRead(pin) == LOW;
}

/*
 * sendControllerState will send the state of the button to the
 * other ESP32.
 */
void sendControllerState() {
  outgoingData.bluePressed   = isButtonPressed(PIN_BTN_BLUE);
  outgoingData.yellowPressed = isButtonPressed(PIN_BTN_YELLOW);
  outgoingData.joyBtnPressed = isButtonPressed(PIN_JOYSTICK_BTN);
  outgoingData.joyX          = readJoystickCentered(PIN_JOYSTICK_X);
  outgoingData.joyY          = readJoystickCentered(PIN_JOYSTICK_Y);

  esp_now_send(RECEIVER_MAC, (uint8_t *)&outgoingData, sizeof(outgoingData));
}


// Prints all current input states to Serial for debugging
void debugPrintInputs() {
  Serial.print(".  Blue: ");
  Serial.print(isButtonPressed(PIN_BTN_BLUE));

  Serial.print("   Yellow: ");
  Serial.print(isButtonPressed(PIN_BTN_YELLOW));

  Serial.print("   Joy Btn: ");
  Serial.print(isButtonPressed(PIN_JOYSTICK_BTN));

  Serial.print("   Joy X: ");
  Serial.print(readJoystickCentered(PIN_JOYSTICK_X));

  Serial.print("   Joy Y: ");
  Serial.println(readJoystickCentered(PIN_JOYSTICK_Y));
}


void loop() {
  debugPrintInputs();
  sendControllerState();
  delay(500); //this is transmitting twice per second 500ms * 2 is 1 sec lol basic math. 
}
