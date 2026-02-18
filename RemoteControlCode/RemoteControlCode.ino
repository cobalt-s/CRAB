//import statements for ESP now
#include <Wifi.h>
#include <esp_now.h>

//Connections for different controller parts
const int JoyStick_X_Input = 0; //pin number to be changed when we wire the eps32
const int JoyStick_Y_Input = 1; //pin number to be changed when we wire the eps32
const int JoyStick_Button = 3;  //pin number to be changed when we wire the eps32
const int buttonPinBlue = 5;    //pin number to be changed when we wire the eps32
const int buttonPinYellow = 6;  //pin number to be changed when we wire the eps32

//

// Receiver MAC address (match with receiver address)
uint8_t receiverMAC[] = {0x44, 0x1D, 0x64, 0xF8, 0x22, 0xD8};


//Setting up variables as floats and ints
float JoyStick_X_Output, JoyStick_Y_Output;
int JoyStickButton;

/*As far as I am aware this sets up what we are sending to the reciever
 *Earlier you were just sending a value for which battery is pressed but
 *After some vibe coding ngl and some thinking I think this is what we 
 *Would need for the controller. 
 */
typedef struct message {
  bool bluePressed;   //Indicates if the blue button is pressed.
  bool yellPressed;   //Indicates if the yellow button is pressed.
  bool joySPressed;   //Indicates if the button on the joystick is pressed.

  int joyX;  //X value of the joystick.
  int joyY;  //Y value of the joystick. 

} message;

message sendData;




void setup() {
  Serial.begin(9600); // serial output with 9600 bps
  delay(1000);
  //I turned this into a function so that setup doesn't get cluttered when I 
  // add wireless stuff.
  pinSetup();
  EspNowSetup();

  Serial.println("Remote control is ready.");
}


void pinSetup(){
  //Set's up pins to read Joystick values
  pinMode(JoyStick_X_Input, INPUT);
  pinMode(JoyStick_Y_Input, INPUT);
  //Set's up pins to read button values
  pinMode(JoyStick_Button, INPUT_PULLUP); //when the button is not pressed it is HIGH
  pinMode(buttonPinBlue, INPUT_PULLUP);  //when the button is not pressed it is HIGH
  pinMode(buttonPinYellow, INPUT_PULLUP);//when the button is not pressed it is HIGH
}


void EspNowSetup(){
  /*I trust Renzo I have no idea what this does?*/

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;   //Dr. Schuessler would be disappointed
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6); 
  peerInfo.channel = 0;                       
  peerInfo.encrypt = false;                   

  esp_now_add_peer(&peerInfo);

  Serial.println("Transmitter Ready");
}


//turns the input from a joystick into a -2048 to 2047 range (more precise)
//Note, this 4095 range is for ESP32 only, it would be 1028 for Arduino.
int readJoySitckRange(int pin){
  /*
   * If this is too jittery we could add code to center it when it's a low
   * enough value.
   */

  return (analogRead(pin)-2048);
}


//Assumes the button is a Pull Up
bool buttonPressCheck(int pin){
  return digitalRead(pin) == LOW; //recall if LOW, that means it is pressed
}

void sendStatus(){
  //I was thinking this might be better if we actually store the state of
  //The variables instead of just immediately sending them. Not that hard
  //To tweak if we wanted to.
  sendData.bluePressed = buttonPressCheck(buttonPinBlue);
  sendData.yellPressed = buttonPressCheck(buttonPinYellow);
  sendData.joySPressed = buttonPressCheck(JoyStick_Button);

  sendData.joyX = readJoySitckRange(JoyStick_X_Input);
  sendData.joyY = readJoySitckRange(JoyStick_Y_Input);

  //Might need to change this idk?
  esp_now_send(receiverMAC, (uint8_t *)&sendData, sizeof(sendData));

  Serial.print("Package Sent: ");
}


//This method compiles a string that showcases the state of all buttons and
//Joystick
void inputConsoleDebug(){
  Serial.print("Blue Pressed: ");
  Serial.print(buttonPressCheck(buttonPinBlue));

  Serial.print("   Yellow Pressed: ");
  Serial.print(buttonPressCheck(buttonPinYellow));

  Serial.print("   Joystick Pressed: ");
  Serial.print(buttonPressCheck(JoyStick_Button));

  Serial.print("   Joystick X: ");
  Serial.print(readJoySitckRange(JoyStick_X_Input));

  Serial.print("   Joystick Y: ");
  Serial.println(readJoySitckRange(JoyStick_Y_Input));
}

void loop(){
  inputConsoleDebug();
  sendStatus();
  delay(500);  //I think twice every second might be a good speed we can change later.
}