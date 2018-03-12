  /**
* breakout for thimble rgb matrix arcade
* based on the pong.ino included with the kit!
*
* By Chuan-Li Ojales Chang and Cody Antonio Gagnon
* MIT License
*/

//Kit specific libraries
#include <RGB_sprite.h>
#include <masterScreen.h>

//File and library for USB emulation
#include <usbconfig.h>
#include <UsbKeyboard.h> //Library needed for USB input
bool usbEnabled = false;


//Defines for readability later on

#define num_sprites 34
#define paddle      0
#define ball        1
// sprintes 2 - 33 are bricks

//Settings for the button scan array, left side
int leftInputs[] = {A0, A1}; //INPUT_PULLUP reading pins
int leftOutputs[] = {A2, A3, A4}; //Column HIGH or LOW pins
//Used to translate  the reading to a real button
String leftButtons[2][3] = {
  {"LL", "LU", "SELECT"},
  {"LR", "LD", "LShoulder"}
};

//Settings for the button scan array, left side
int rightInputs[] = {10, A5}; //INPUT_PULLUP reading pins
int rightOutputs[] = {11, 12, 13}; //Column HIGH or LOW pins
//Used to translate  the reading to a real button
String rightButtons[2][3] = {
  {"RL", "RU", "START"},
  {"RR", "RD", "RShoulder"}
};

//***********Game States************
enum possibleStates {
  UP,
  UPRIGHT,
  UPLEFT,
  DOWN,
  DWNRIGHT,
  DWNLEFT,
  PAUSE,
  SERVE,
  WIN,
  STARTGAME,
  POINT
};

possibleStates gameState;

//Starting direction; these are the possibilities for a ball served from the paddle
int startDir[] = {UPLEFT, UP, UPRIGHT}

//***********Paddle Global Variables************

// Paddle
int score = 0;
int paddle_pos            =   0;
byte leftLeftButton_currentState   = 0;
byte rightRightButton_currentState  = 0;

bool isCollisionResult = 0;

//***********Game Screen and Sprite Global Variables************
//Declare the screen
masterScreen gameScreen;
//Declare the paddles and ball
RGB_Sprite sprite_lst[num_sprites];
//Declare the score counters
RGB_Sprite score;

void setup() {

  //Set the rows of the button array as inputs
  for (int i = 0; i < 2; i++) {
    pinMode(leftInputs[i], INPUT_PULLUP);
    pinMode(rightInputs[i], INPUT_PULLUP);
  }

  //Set the columns as outputs
  for (int i = 0; i < 3; i++) {
    pinMode(leftOutputs[i], OUTPUT);
    digitalWrite(leftOutputs[i], HIGH);
    pinMode(rightOutputs[i], OUTPUT);
    digitalWrite(rightOutputs[i], HIGH);
  }

  //***********Initialize gameScreen************
  gameScreen.createScreen();

  //*************Sprite Initialization***********
  //Create and store a sprite (width, height, colors to use ....)
  sprite_lst[paddle].Sprite(4, 1, 1,
                             1,
                             1,
                             1);

  sprite_lst[ball].Sprite(1, 1, 4);

  score.Sprite(1, 8, 0,
                  0,
                  0,
                  0,
                  0,
                  0,
                  0,
                  0);

  // TODO: Update the origin of the score to be in a more appropriate location
  score.updateOrigin(3, 0);

  //start with the game at the STARTGAME state
  gameState = STARTGAME;

  // Add tones for game's start
  tone(8, 459, 96);
  tone(9, 459, 96);

  if (usbEnabled) {
    // Clear interrupts while performing time-critical operations
    cli();

    // Force re-enumeration so the host will detect us
    usbDeviceDisconnect();
    delayMs(250);
    usbDeviceConnect();

    // Set interrupts again
    sei();
  }

}
