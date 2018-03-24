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
// sprites 2 - 33 are bricks

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
int startDir[] = {UPLEFT, UPRIGHT};

//***********Paddle Global Variables************

// Paddle
int score = 0;
int paddle_pos = 2;
byte leftLeftButton_currentState = 0;
byte leftLeftButton_previousState = 0;
byte rightRightButton_currentState = 0;
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
Serial.begin(9600);

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

// rows of bricks
for (int brick = 2; brick <= 9; brick++) { //Outputs
  sprite_lst[brick].Sprite(1, 1, 2);
}

//  for (int brick = 10; brick <= 17; brick++) { //Outputs
//    sprite_lst[brick].Sprite(1, 1, 3);
//  }
//
//  for (int brick = 18; brick <= 25; brick++) { //Outputs
//    sprite_lst[brick].Sprite(1, 1, 4);
//  }
//
//  for (int brick = 26; brick <= 33; brick++) { //Outputs
//    sprite_lst[brick].Sprite(1, 1, 5);
//  }

// TODO: Update the origin of the score to be in a more appropriate location
score_sprite.updateOrigin(3, 0);

//start with the game at the STARTGAME state
gameState = STARTGAME;
Serial.print("SETUP INITIALIZING");

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
  }

  if (sprite_lst[ball].y_ > 7) {
    sprite_lst[ball].y_ = 7;
  } else if (sprite_lst[ball].y_ < 1) {
    // TODO: out of bounds ball -> DIE

  }

  //Checks if the ball hit something
  isCollisionResult = sprite_lst[ball].isCollisionNoScreen(num_sprites, ball, sprite_lst, sprite_lst[ball].x_, sprite_lst[ball].y_);
  //if no collision redraw the ball
  if (isCollisionResult == false) {
    //clear Sprite
    gameScreen.clearSprite(sprite_lst[ball]);
    //update origin
    sprite_lst[ball].updateOrigin(sprite_lst[ball].x_, sprite_lst[ball].y_);

    //update screen matrix
    gameScreen.updateMasterScreen(sprite_lst[ball]);

   } else {
  //Collision with paddle sound
  // tone(9, 459, 96);
     tone(9, 100, 96);
   }
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

  case UPLEFT:
    Serial.print("---UPLEFT---");
    if (isCollisionResult == true) {
      // collision with brick or ceiling
      Serial.print("***BRICK***");
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_--;
      gameState = DWNLEFT;
    } (sprite_lst[ball].isT_boardCollision() == true) {
       // collision with top board
       Serial.print("***TOP***");
       // TODO: Implement top board collision
    } else if (sprite_lst[ball].isL_boardCollision() == true) {
      Serial.print("***LEFT WALL***");
       sprite_lst[ball].x_++;
       sprite_lst[ball].y_++;
      gameState = UPRIGHT;
    } else {
      Serial.print("***NORMAL***");
      // translate ball normally
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_++;
    }
    break;

  case UPRIGHT:
    Serial.print("---UPRIGHT---");
    if (isCollisionResult == true)  {
      // collision with brick
      Serial.print("***BRICK***");
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_--;
      gameState = DWNRIGHT;
    } (sprite_lst[ball].isT_boardCollision() == true) {
      // collision with top board
      Serial.print("***TOP***");
      // TODO: Implement top board collision
    } else if (sprite_lst[ball].isR_boardCollision() == true) {
      Serial.print("***RIGHT***");
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_++;
      gameState = UPLEFT;
   } else {
      Serial.print("***NORMAL***");
      // translate ball normally
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_++;
    }
    break;

  case DWNLEFT:
    Serial.print("---DWNLEFT---");
    if (isCollisionResult == true) {
      Serial.print("***PADDLE***");
      // paddle collision
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_++;
      gameState = UPLEFT;
    } else if (sprite_lst[ball].isL_boardCollision() == true) {
      // collision with left wall
       Serial.print("***LEFT***");
       sprite_lst[ball].x_++;
       sprite_lst[ball].y_--;
      gameState = DWNRIGHT;
    } else if (sprite_lst[ball].isB_boardCollision() == true) {
      Serial.print("***BOTTOM***");
      // TODO: This is temporary for testing; remove when validated
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_++;
      gameState = UPLEFT;
    } else {
      // translate the ball normally
      Serial.print("***NORMAL***");
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_--;
    }
    break;

  case DWNRIGHT:
    Serial.print("---DWNRIGHT---");
    if (isCollisionResult == true) {
      Serial.print("***PADDLE***");
      // paddle collision
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_++;
      gameState = UPRIGHT;
    } else if (sprite_lst[ball].isR_boardCollision() == true) {
      Serial.print("***RIGHT***");
      // collision with right wall
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_--;
      gameState = DWNLEFT;
    } else if (sprite_lst[ball].isB_boardCollision() == true) {
      Serial.print("***BOTTOM***");
      // TODO: This is temporary for testing; remove when validated
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_++;
      gameState = UPRIGHT;
    } else {
      // translate the ball normally
      Serial.print("***NORMAL***");
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_--;
    }
    break;

  case SERVE:
    Serial.print("SERVE");
    sprite_lst[ball].x_ = 4;
    sprite_lst[ball].y_ = 1;
    gameState = startDir[random(1, 3)];
    break;

  case STARTGAME:
    //*******Game board Specific Sprite starting Position Stuff*********
    Serial.print("STARTGAME");
    sprite_lst[ball].duration = 200;
    sprite_lst[ball].x_ = 4;
    sprite_lst[ball].y_ = 1;

    score = 0;
    clearScore();

    sprite_lst[paddle].updateOrigin(paddle_pos, 0);
    gameScreen.updateMasterScreen(sprite_lst[paddle]);

    // make rows and cols of bricks
    // for (int brick_row = 0, y = 7; brick_row <= 3; brick_row++, y--) {
    //   for (int brick_col = 2, x = 0; brick_col <= 9; brick_col++, x++) { //Outputs
    //     int brick = brick_col + (brick_row * 8);
    //     sprite_lst[brick].updateOrigin(x, y);
    //     gameScreen.updateMasterScreen(sprite_lst[brick]);
    //   }
    // }

    // make rows and cols of bricks
    for (int brick_row = 0, y = 7; brick_row <= 0; brick_row++, y--) {
      for (int brick_col = 2, x = 0; brick_col <= 9; brick_col++, x++) { //Outputs
        int brick = brick_col + (brick_row * 8);
        sprite_lst[brick].updateOrigin(x, y);
        gameScreen.updateMasterScreen(sprite_lst[brick]);
      }
    }

    //This might change to something smarter
    // gameState = startDir[random(1, 3)];
    // gameState = startDir[random(1, 3)];
    gameState = UPRIGHT;
    delay(3000);
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
