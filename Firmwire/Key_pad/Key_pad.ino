#include <Keypad.h>

// Set up the Keypad
const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns
char keys[ROWS][COLS] = {
  {'D', '#', '0', '*'},
  {'C', '9', '8', '7'},
  {'B', '6', '5', '4'},
  {'A', '3', '2', '1'}
};
byte rowPins[ROWS] = {33, 35, 37, 23};   // Connect to the row pinouts of the keypad
byte colPins[COLS] = {25, 27, 31, 29};     // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  Serial.println("Matrix Keypad Test: Press keys to see them on Serial Monitor.");
}

void loop() {
  // Check for key presses
  char key = keypad.getKey();
  
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
  }
}
