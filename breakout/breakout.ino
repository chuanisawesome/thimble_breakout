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
  POINT,
  DIE
};

possibleStates gameState;

//Starting direction; these are the possibilities for a ball served from the paddle
int startDir[] = {UPLEFT, UP, UPRIGHT};

//***********Paddle Global Variables************

// Paddle
int score = 0;
int paddle_pos                      = 0;
byte leftLeftButton_currentState    = 0;
byte leftLeftButton_previousState   = 0;
byte rightRightButton_currentState  = 0;
byte rightRightButton_previousState = 0;

bool isCollisionResult = 0;

//***********Game Screen and Sprite Global Variables************
//Declare the screen
masterScreen gameScreen;
//Declare the paddles and ball
RGB_Sprite sprite_lst[num_sprites];
//Declare the score counters
RGB_Sprite score_sprite;

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

  score_sprite.Sprite(1, 8, 0,
                  0,
                  0,
                  0,
                  0,
                  0,
                  0,
                  0);

  // TODO: Update the origin of the score to be in a more appropriate location
  score_sprite.updateOrigin(3, 0);

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

void loop() {

  // If you are using the USB mode then update the USb connection
  if (usbEnabled) {
    UsbKeyboard.update();
  }

  //Run the row and column button scans
  buttonScan();

  //If the left button of the leftpad was pressed...
  if (leftLeftButton_currentState != leftLeftButton_previousState && !leftLeftButton_previousState) {
    //Move the paddle left
    paddle_pos--;
    //Left correction for going off the screen
    if (paddle_pos < 0) {
      paddle_pos = 0;
    }
    //These lines erase the sprite, write its new position, then push that to the screen
    gameScreen.clearSprite(sprite_lst[paddle]);
    sprite_lst[paddle].updateOrigin(paddle_pos, 0);
    gameScreen.updateMasterScreen(sprite_lst[paddle]);
  }

  //If the right button of the rightpad was pressed...
  else if (rightRightButton_currentState != rightRightButton_previousState && !rightRightButton_previousState) {
    //Move the paddle right
    paddle_pos++;
    //Left correct
    if (paddle_pos > 4) {
      paddle_pos = 4;
    }
    gameScreen.clearSprite(sprite_lst[paddle]);
    sprite_lst[paddle].updateOrigin(paddle_pos, 0);
    gameScreen.updateMasterScreen(sprite_lst[paddle]);
  }

  //this code checks the current time to see if the ball should move
  sprite_lst[ball].current_time = millis();
  if ((sprite_lst[ball].current_time - sprite_lst[ball].previous_time) >= sprite_lst[ball].duration)
  {
    sprite_lst[ball].previous_time = sprite_lst[ball].current_time;

    if (sprite_lst[ball].x_ > 7) {
      sprite_lst[ball].x_ = 7;
    } else if (sprite_lst[ball].x_ < 0) {
      sprite_lst[ball].x_ = 0;
    }

    if (sprite_lst[ball].y_ > 7) {
      sprite_lst[ball].y_ = 7;
      // TODO: out of bounds ball -> DIE
    } else if (sprite_lst[ball].y_ < 0) {
      sprite_lst[ball].y_ = 0;
    }

    //Checks if the ball hit something
    // TODO: Implement collisions
    //isCollisionResult = sprite_lst[ball].isCollisionNoScreen(num_sprites, ball, sprite_lst, sprite_lst[ball].x_, sprite_lst[ball].y_);

    //if no collision redraw the ball
    // if (isCollisionResult == false) {
    //   //clear Sprite
    //   gameScreen.clearSprite(sprite_lst[ball]);
    //
    //   //update origin
    //   sprite_lst[ball].updateOrigin(sprite_lst[ball].x_, sprite_lst[ball].y_);
    //
    //   //update screen matrix
    //   gameScreen.updateMasterScreen(sprite_lst[ball]);
    // } else {
    //   //Collision with paddle sound
    //   // tone(9, 459, 96);
    //   tone(9, 100, 96);
    // }
    //Figure out the next move of the ball
    stateChange();
    }
  }//End of loop()

  void buttonScan() {
  // Reset varibles
  leftLeftButton_previousState = leftLeftButton_currentState;
  leftLeftButton_currentState = 0;

  for (int y = 0; y < 3; y++) { //Outputs
    digitalWrite(leftOutputs[y], LOW); //Turn column on
    for (int x = 0; x < 2 ; x++) { //Inputs
      if (!digitalRead(leftInputs[x])) { //Read row
        if (x == 0  && y == 0) {
          //Left left is pressed down
          leftLeftButton_currentState = 1;
        }
      }
    }//End of x
    digitalWrite(leftOutputs[y], HIGH); //turn column off
  }//End of y

  rightRightButton_previousState = rightRightButton_currentState;
  rightRightButton_currentState = 0;

  for (int y = 0; y < 3; y++) { //Outputs
    digitalWrite(rightOutputs[y], LOW); //Turn column on
    for (int x = 0; x < 2 ; x++) { //Inputs
      if (!digitalRead(rightInputs[x])) { //Read row
        if (x == 1  && y == 0) {
          rightRightButton_currentState = 1;
        }
      }
    }//End of x
    digitalWrite(rightOutputs[y], HIGH); //turn column off
  }//End of y

}

void stateChange()
{
  sprite_lst[ball].prevState = sprite_lst[ball].state;
  //Refer to the ball state diagram in the learning module
  switch (gameState) {
    case STARTGAME:
      //*******Game board Specific Sprite starting Position Stuff*********

      //gameScreen.clearMasterScreen();
      sprite_lst[ball].duration = 200;
      sprite_lst[ball].x_ = 4;
      sprite_lst[ball].y_ = 1;

      sprite_lst[paddle].updateOrigin(paddle_pos, 0);
      gameScreen.updateMasterScreen(sprite_lst[paddle]);

      gameScreen.updateMasterScreen(sprite_lst[ball]);

      score = 0;
      delay(1000);
      // TODO: Implement clearScore
      //clearScore();
      //This might change to something smarter
      gameState = startDir[1];
      break;
    }
}

void clearScore()
{
  for (byte i = 0; i <= 7; i++) {
    score_sprite.write(0, i, 0); // write to the height of the sprite
  }
  gameScreen.clearSprite(score_sprite);
}

// helper method for V-USB library
void delayMs(unsigned int ms) {
  for ( int i = 0; i < ms; i++ ) {
    delayMicroseconds(1000);
  }
}