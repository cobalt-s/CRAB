Please keep the .ino files in separate directories.

The transmitter file connects the pins 13, 12, 14, 17 to a button, then to ground.

The receiver file connects pins 25, 26, 32, and 33 to an LED (resistor needed depends on LED), then to ground. 

The pins in order are respective to their button and LED number (i.e. 1, 2, 3, 4).

This is to show that ESPNOW works and can be edited for joystick use rather than buttons. 

In this example, the light will stay on as the button is pressed. As of this commit, I have yet to properly comment my code (so sorry, kind of in a rush right now lol).

It isn't that much code, however, so hopefully things will make sense with further inspection. 