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

//TODO: Determine number of sprites
//Defines for readability later on
#define num_sprites 3
#define leftPad     0
#define rightPad    1
#define ball        2

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
