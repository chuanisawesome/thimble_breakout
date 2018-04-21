import os
import time

if __name__ == "__main__":
  print('Your choices are:')
  print('1. Play thimble breakout')
  print('2. Play google images atari breakout')
  print('3. Play Pong')
  print('4. Watch Steve Wozniak talk about the creation of breakout')
  choice = raw_input('Please enter your choice from the following: ')
  if (choice == '1'):
      print("Please ask attendant for Breakout board")
      time.sleep(30)
      os.system('clear')
  if (choice == '2'):
    print("Please wait for attendant to start your game :)")
    time.sleep(30)
    os.system('clear')
    #if keyboard.is_pressed('space'):  # if key 'space' is pressed
    os.system('chrome http://bit.ly/search_atari_breakout --incognito --start-maximized')
    time.sleep(30)
    os.system('clear')
        # uses chrome-cli from npm to easily create incognito window with atari breakout for Google Images
  if (choice == '3'):
    print("Please ask attendant for Pong board")
    time.sleep(30)
    os.system('clear')
  if (choice == '4'):
    print("Enjoy learning about the creation of Breakout")
  #if keyboard.is_pressed('space'):  # if key 'space' is pressed
    os.system('chrome https://www.youtube.com/watch?v=17eUExffa5w --incognito --start-maximized')
    os.system('clear')
