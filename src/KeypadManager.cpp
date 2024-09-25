#include "KeypadManager.h"

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[4][4] = {
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte colPins[4] = {13, 12, 14, 27};
byte rowPins[4] = {26, 25, 33, 32};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
