#include "UsbKeyboard.h"

#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_ENTER   40

#define KEY_SPACE   44

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

#define KEY_ARROW_LEFT 0x50
#define KEY_ARROW_RIGHT 0x4F
#define KEY_ARROW_UP 0x52
#define KEY_ARROW_DOWN 0x51



#define BYPASS_TIMER_ISR 1

int leftInputs[] = {A0, A1}; //INPUT_PULLUP reading pins
int leftOutputs[] = {A2, A3, A4}; //Column HIGH or LOW pins
bool leftHeld[2][3] =  { // Variables for presses
  {false, false, false},
  {false, false, false}
};
int leftButtons[2][3] = { //Used to translate  the reading to a real button
  {KEY_ARROW_LEFT, KEY_ARROW_UP, KEY_SPACE},
  {KEY_ARROW_RIGHT, KEY_ARROW_DOWN, KEY_SPACE}
};

/*
int leftButtons[2][3] = {
  {"LL", "LU", "SELECT"},
  {"LR", "LD", "LShoulder"}
};
*/

int rightInputs[] = {10, A5}; //INPUT_PULLUP reading pins
int rightOutputs[] = {11, 12, 13}; //Column HIGH or LOW pins
bool rightHeld[2][3] =  { // Variables for presses
  {false, false, false},
  {false, false, false}
};
int rightButtons[2][3] = {
  {KEY_X, KEY_Y, KEY_SPACE},
  {KEY_A, KEY_B, KEY_SPACE}
};

/*
int rightButtons[2][3] = {
  {"RL", "RU", "START"},
  {"RR", "RD", "RShoulder"}
};
*/

void setup() {
  //Set inputs
  for (int i = 0; i < 2; i++) {
    pinMode(leftInputs[i], INPUT_PULLUP);
    pinMode(rightInputs[i], INPUT_PULLUP);
  }

  //Set outputs
  for (int i = 0; i < 3; i++) {
    pinMode(leftOutputs[i], OUTPUT);
    digitalWrite(leftOutputs[i], HIGH);
    pinMode(rightOutputs[i], OUTPUT);
    digitalWrite(rightOutputs[i], HIGH);
  }

#if BYPASS_TIMER_ISR
  // disable timer 0 overflow interrupt (used for millis)
  TIMSK0 &= !(1 << TOIE0); // ++
#endif

  Serial.begin(9600); //For debugging
}




#if BYPASS_TIMER_ISR
void delayMs(unsigned int ms) {

  for (int i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
}
#endif

void loop() {

  UsbKeyboard.update();

  // Left side
  for (int y = 0; y < 3; y++) { //Outputs
    digitalWrite(leftOutputs[y], LOW); //Turn column on
    for (int x = 0; x < 2 ; x++) { //Inputs
      if (!digitalRead(leftInputs[x])) { //Read row
        //Button is pressed
        if (!leftHeld[x][y]) {
          UsbKeyboard.sendKeyStroke(leftButtons[x][y]);
          leftHeld[x][y] = true;
        }
      }
      else {
        leftHeld[x][y] = false;
      }
    }//End of x
    digitalWrite(leftOutputs[y], HIGH); //turn column off
  }//End of y

  // Right side
  for (int y = 0; y < 3; y++) { //Outputs
    digitalWrite(rightOutputs[y], LOW); //Turn column on
    for (int x = 0; x < 2 ; x++) { //Inputs
      if (!digitalRead(rightInputs[x])) { //Read row
        if (!rightHeld[x][y]) {
          UsbKeyboard.sendKeyStroke(rightButtons[x][y]);
          rightHeld[x][y] = true;
        }
      }
      else {
        rightHeld[x][y] = false;
      }
    }//End of y
    digitalWrite(rightOutputs[y], HIGH); //turn column off
  }//End of x

#if BYPASS_TIMER_ISR  // check if timer isr fixed.
  delayMs(20);
#else
  delay(20);
#endif
}



