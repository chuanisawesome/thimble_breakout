import os
import keyboard

if __name__ == "__main__":
  print('Your choices are:')
  print('1. Play thimble breakout')
  print('2. Play google images atari breakout')
  choice = raw_input('Please enter your choice from the following: ')
  if (choice == '2'):
    print("please wait for attendant to start your game :)")
    if keyboard.is_pressed('space'):  # if key 'space' is pressed
        os.system('chrome http://bit.ly/search_atari_breakout --incognito --start-maximized')
        # uses chrome-cli from npm to easily create incognito window with atari breakout for Google Images
