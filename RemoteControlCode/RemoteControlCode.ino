
/**
REMOTE CONTROL CODE

Be able to have inputs and send those inputs to the
other ESP-32 to make the robot move. 

Todo: 
-Add LED turn on when the program is on. 
-Add LED when connected.
-Add integration with ESPNOW.
*/

//declaring joystick pins (these might be changed)
int JoyStick_X_Input = A0; 
int JoyStick_Y_Input = A1; 
int Button = 3;

void setup() {
pinMode(JoyStick_X_Input, INPUT);
pinMode(JoyStick_Y_Input, INPUT);
pinMode(Button, INPUT);

digitalWrite(Button, HIGH);
Serial.begin (9600); // serial output with 9600 bps
}

void loop() {
float JoyStick_X_Output, JoyStick_Y_Output;
int JoyStickButton;

JoyStick_X_Output = analogRead(JoyStick_X_Input) * (5.0 / 1023.0);
JoyStick_Y_Output = analogRead(JoyStick_Y_Input) * (5.0 / 1023.0); 
JoyStickButton = digitalRead(Button);

//Joysticks
Serial.print("X-axis:"); 
Serial.print(JoyStick_X_Output, 4); 
Serial.print("V, ");

Serial.print("Y-axis:"); 
Serial.print(JoyStick_Y_Output, 4); 
Serial.print("V, ");

//Buttons
Serial.print("Button:");
if(JoyStickButton == 1) {
Serial.println("not pushed");
}
else {
Serial.println("pushed");
}
delay (200);
}