//Connections for different controller parts
const int JoyStick_X_Input = 0; //pin number to be changed when we wire the eps32
const int JoyStick_Y_Input = 1; //pin number to be changed when we wire the eps32
const int JoyStick_Button = 3;  //pin number to be changed when we wire the eps32
const int buttonPinBlue = 5;    //pin number to be changed when we wire the eps32
const int buttonPinYellow = 6;  //pin number to be changed when we wire the eps32

/*
// Receiver MAC address (match with receiver address)
uint8_t receiverMAC[] = {0x44, 0x1D, 0x64, 0xF8, 0x22, 0xD8};
*/

//Setting up variables as floats and ints
float JoyStick_X_Output, JoyStick_Y_Output;
int JoyStickButton;

/*
typedef struct message {
  uint8_t buttonID;
} message;

message sendData;
*/


void setup() {
  //Set's up pins to read Joystick values
  pinMode(JoyStick_X_Input, INPUT);
  pinMode(JoyStick_Y_Input, INPUT);
  //Set's up pins to read button values
  pinMode(JoyStick_Button, INPUT_PULLUP); //when the button is not pressed it is HIGH
  pinMode(buttonPinBlue, INPUT_PULLUP);  //when the button is not pressed it is HIGH
  pinMode(buttonPinYellow, INPUT_PULLUP);//when the button is not pressed it is HIGH

   // serial output with 9600 bps
  Serial.begin(9600); 
  Serial.println("Remote control is ready.");
}

//turns the input from a joystick into a -1.0 to 1.0 range
float readJoySitckRange(int pin){
  return(analogRead(pin) - 512) / 512.0;
}

//Assumes the button is a Pull Up
bool buttonPressCheck(int pin){
  return digitalRead(pin) == LOW; //recall if LOW, that means it is pressed
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
  delay(1000);
}