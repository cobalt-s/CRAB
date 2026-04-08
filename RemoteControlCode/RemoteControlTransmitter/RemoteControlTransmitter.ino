/*
* Remote Control Code for Crab Project
* Authors: Renzo, Jimmy
* Date last modified: 4/6/2026
* TODO: 
* add buzzer implementation
* add screen implementation
*/


//Import for controllers pin assignments
#include <SPI.h>
#include <Wire.h>
// Import statements for ESP-NOW
#include <WiFi.h>
#include <esp_now.h>
//Import statements for LED display graphics
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin assignments (to be updated when physically wired)
const int PIN_JOYSTICK_X      = 0;
const int PIN_JOYSTICK_Y      = 1;
const int PIN_JOYSTICK_BTN    = 3;
const int PIN_BTN_BLUE        = 5;
const int PIN_BTN_YELLOW      = 6;
const int LED_1               = 7; 
const int LED_2               = 8;

//Global Variables and Constants
const int SCREEN_WIDTH        = 128;
const int SCREEN_HEIGHT       = 64;
bool switchFace               = 0;

//Constructing an object of the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
  Serial.begin(9600); //Jimmy comment: Might need 115200 Baud rate or config changes idk yet
  delay(1000);
  setupPins();
  setupDisplay();
  setupEspNow();
  Serial.println("Remote control is ready.");
}

setupDisplay(){

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Jimmy comment: This loops forever if it fails do we want that?
  }

  //Jimmy comment: I believe we can remove the next 2 lines. Will test with Renzo when wired
  display.display(); //Jimmy comment: Displays the adafruit logo I believe
  delay(1000);
  display.clearDisplay();
  display.display();

}

void setupPins() {
  pinMode(PIN_JOYSTICK_X,   INPUT);
  pinMode(PIN_JOYSTICK_Y,   INPUT);
  pinMode(PIN_JOYSTICK_BTN, INPUT_PULLUP); // HIGH when not pressed
  pinMode(PIN_BTN_BLUE,     INPUT_PULLUP); // HIGH when not pressed
  pinMode(PIN_BTN_YELLOW,   INPUT_PULLUP); // HIGH when not pressed
  //Pins for Display are set up with include/import statements
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

void debugDisplay() {
  drawHappyFace();
  delay(5000);
  drawHappyFace(15, 10, 20, 1, 50, 40, 1);
  delay(5000);
  drawHappyFace(15, 10, 20, 1, 50, 100, 1);
  delay(5000);
}

void drawEyes(int16_t theXFromCenter, int16_t theY, int16_t theSize, uint16_t theColor){

  display.drawLine(SCREEN_WIDTH/2 - theXFromCenter-1, theY+2, SCREEN_WIDTH/2 - theXFromCenter-1, theY + theSize-2, theColor);
  display.drawLine(SCREEN_WIDTH/2 - theXFromCenter, theY, SCREEN_WIDTH/2 - theXFromCenter, theY + theSize, theColor);
  display.drawLine(SCREEN_WIDTH/2 - theXFromCenter+1, theY+2, SCREEN_WIDTH/2 - theXFromCenter+1, theY + theSize-2, theColor);

  display.drawLine(SCREEN_WIDTH/2 + theXFromCenter-1, theY+2, SCREEN_WIDTH/2 + theXFromCenter-1, theY + theSize-2, theColor);
  display.drawLine(SCREEN_WIDTH/2 + theXFromCenter, theY, SCREEN_WIDTH/2 + theXFromCenter, theY + theSize, theColor);
  display.drawLine(SCREEN_WIDTH/2 + theXFromCenter+1, theY+2, SCREEN_WIDTH/2 + theXFromCenter+1, theY + theSize-2, theColor);

}

void drawSmile(int16_t theY, int16_t theSize, uint16_t theColor){
  int16_t currentLength = theSize/2;
  int16_t currentY = theY;
  int16_t theChange = 2;
  while(currentLength > 0){
    display.drawLine(SCREEN_WIDTH/2 - currentLength, currentY, SCREEN_WIDTH/2 + currentLength, currentY, theColor);
    currentY++;
    currentLength -= theChange;
    theChange += 1;
  }

}

void drawHappyFace(int16_t theEyeX, int16_t theEyeY, int16_t theEyeSize, uint16_t theEyeColor, 
              int16_t theMouthY, int16_t theMouthWidth, uint16_t theMouthColor){

  display.clearDisplay();

  drawEyes(theEyeX, theEyeY, theEyeSize, theEyeColor);

  drawSmile(theMouthY, theMouthWidth, theMouthColor);

  display.display();

}

//Default Happy face drawing
void drawHappyFace(){

  drawHappyFace(20, 10, 15, 1, 40, 80, 1);

}

void loop() {
  debugPrintInputs();
  debugDisplay();
  sendControllerState();
  delay(500); //this is transmitting twice per second 500ms * 2 is 1 sec lol basic math. 
}
