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

#define num_sprites 25
#define paddle      0
#define ball        1
// sprites 2 - 25 are bricks

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
DEATH,
PAUSE,
SERVE,
WIN,
STARTGAME
};

possibleStates gameState;

//***********Paddle Global Variables************

// Paddle
int death = 0;
int score = 0;
bool start_dir = 0;
int x_start_pos = 0;
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

sprite_lst[ball].Sprite(1, 1, 2);

// rows of bricks
for (int brick = 2; brick <= 9; brick++) { //Outputs
  sprite_lst[brick].Sprite(1, 1, 1);
}

 for (int brick = 10; brick <= 17; brick++) { //Outputs
   sprite_lst[brick].Sprite(1, 1, 2);
 }

  for (int brick = 18; brick <= 25; brick++) { //Outputs
    sprite_lst[brick].Sprite(1, 1, 4);
  }


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


  Serial.print("CURRENT SCORE: ");
  Serial.print(score);

  Serial.print("CURRENT DEATHS: ");
  Serial.print(death);

  if (score > 20){
    sprite_lst[ball].Sprite(1, 1, 3);
  }

//Refer to the ball state diagram in the learning module
switch (gameState) {

  case UPLEFT:
    Serial.print("---UPLEFT---");
    // Corner case
    if (sprite_lst[ball].x_ == 0 && sprite_lst[ball].y_ ==7) {
      gameState = DWNRIGHT;
      break;
    }
    if (isCollisionResult == true) {
      // collision with brick or ceiling
      Serial.print("***BRICK***");
      int val_index = (sprite_lst[ball].x_ + 2) + ((7 - sprite_lst[ball].y_) * 8);
      sprite_lst[val_index].write(0, 0, 0);
      gameScreen.clearSprite(sprite_lst[val_index]);
      gameScreen.updateMasterScreen(sprite_lst[val_index]);
      score++;
      gameState = DWNLEFT;

    } else if (sprite_lst[ball].isT_boardCollision() == true) {
       // collision with top board
       Serial.print("***TOP***");
       sprite_lst[ball].x_--;
       sprite_lst[ball].y_--;
       gameState = DWNLEFT;
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

    // Corner case
    if (sprite_lst[ball].x_ == 7 && sprite_lst[ball].y_ == 7) {
      gameState = DWNLEFT;
      break;
    }
    if (isCollisionResult == true)  {
      // collision with brick
      Serial.print("***BRICK***");
      int brick_removal_index = (sprite_lst[ball].x_ + 2) + ((7 - sprite_lst[ball].y_ ) * 8);
      sprite_lst[brick_removal_index].write(0, 0, 0);
      gameScreen.clearSprite(sprite_lst[brick_removal_index]);
      gameScreen.updateMasterScreen(sprite_lst[brick_removal_index]);
      score++;
      gameState = DWNRIGHT;
    } else if (sprite_lst[ball].isT_boardCollision() == true) {
      // collision with top board
      Serial.print("***TOP***");
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_--;
      gameState = DWNRIGHT;
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

    // Corner case
    if (sprite_lst[ball].x_ == 0 && sprite_lst[ball].y_ == 0) {
      gameState = DWNRIGHT;
      break;
    }

    if (isCollisionResult == true) {
      if (sprite_lst[ball].y_ < 5) {
        Serial.print("***PADDLE***");
        sprite_lst[ball].y_--;
        // Serial.print("***BRICK***");
        // int brick_removal_index = (sprite_lst[ball].x_ + 2) + ((7 - sprite_lst[ball].y_ ) * 8);
        // sprite_lst[brick_removal_index].write(0, 0, 0);
        // gameScreen.clearSprite(sprite_lst[brick_removal_index]);
        // gameScreen.updateMasterScreen(sprite_lst[brick_removal_index]);
      } else {
        Serial.print("***PADDLE***");
        sprite_lst[ball].y_--;
      }
      gameState = UPLEFT;
    } else if (sprite_lst[ball].isL_boardCollision() == true) {
      // collision with left wall
       Serial.print("***LEFT***");
       sprite_lst[ball].x_++;
       sprite_lst[ball].y_--;
      gameState = DWNRIGHT;
    } else if (sprite_lst[ball].isB_boardCollision() == true) {
      Serial.print("***BOTTOM***");
      gameState = DEATH;
      death++;
    } else {
      // translate the ball normally
      Serial.print("***NORMAL***");
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_--;
    }
    break;

  case DWNRIGHT:
    Serial.print("---DWNRIGHT---");

    // Corner case
    if (sprite_lst[ball].x_ == 7 && sprite_lst[ball].y_ == 0) {
      gameState = DWNLEFT;
      break;
    }

    if (sprite_lst[ball].isB_boardCollision() == true){
      gameState = DEATH;
      death++;
    }
    if (isCollisionResult == true) {
      if (sprite_lst[ball].y_ < 5) {
        Serial.print("***PADDLE***");
        sprite_lst[ball].y_++;
        // Serial.print("***BRICK***");
        // int brick_removal_index = (sprite_lst[ball].x_ + 2) + ((7 - sprite_lst[ball].y_ ) * 8);
        // sprite_lst[brick_removal_index].write(0, 0, 0);
        // gameScreen.clearSprite(sprite_lst[brick_removal_index]);
        // gameScreen.updateMasterScreen(sprite_lst[brick_removal_index]);
      } else {
        Serial.print("***PADDLE***");
        sprite_lst[ball].y_++;
      }
      gameState = UPRIGHT;
      break;

    } else if (sprite_lst[ball].isR_boardCollision() == true) {
      Serial.print("***RIGHT***");
      // collision with right wall
      sprite_lst[ball].x_--;
      sprite_lst[ball].y_--;
      gameState = DWNLEFT;
    } else if (sprite_lst[ball].isB_boardCollision() == true) {
      Serial.print("***BOTTOM***");
      gameState = DEATH;
      death++;
    } else {
      // translate the ball normally
      Serial.print("***NORMAL***");
      sprite_lst[ball].x_++;
      sprite_lst[ball].y_--;
    }
    break;

  case DEATH:
    Serial.print("DEATH");
    gameState = PAUSE;
    break;

  case PAUSE:
    delay(1000);
    gameState = SERVE;
    break;

  case SERVE:
    Serial.print("SERVE");
    start_dir = random(0, 2);
    if (start_dir == 0) {
      x_start_pos = random(0, 4);
      gameState = DWNRIGHT;
    } else {
      x_start_pos = random(4, 8);
      gameState = DWNLEFT;
    }

    sprite_lst[ball].x_ = x_start_pos;
    sprite_lst[ball].y_ = 2;

    break;

  case STARTGAME:
    //*******Game board Specific Sprite starting Position Stuff*********
    Serial.print("STARTGAME");
    sprite_lst[ball].duration = 500;

    sprite_lst[ball].x_ = x_start_pos;
    sprite_lst[ball].y_ = 2;

    death = 0;
    score = 0;

    // Clear the whole screen first.
    gameScreen.clearMasterScreen();


    // Draw on top of a blank canvas.
    sprite_lst[paddle].updateOrigin(paddle_pos, 0);
    gameScreen.updateMasterScreen(sprite_lst[paddle]);

    for (int brick_row = 0, y = 7; brick_row < 3; brick_row++, y--) {
      for (int brick_col = 2, x = 0; brick_col <= 9; brick_col++, x++) { //Outputs
        int brick = brick_col + (brick_row * 8);
        sprite_lst[brick].updateOrigin(x, y);
        gameScreen.updateMasterScreen(sprite_lst[brick]);
      }
    }

    gameState = SERVE;
    break;
  }
}

// helper method for V-USB library
void delayMs(unsigned int ms) {
  for ( int i = 0; i < ms; i++ ) {
    delayMicroseconds(1000);
  }
}
