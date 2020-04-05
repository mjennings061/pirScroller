# pirScroller
32x8 LED matrix display that scrolls a message when the PIR sensor is triggered. Messages are stored within the messages[][] variable. Ensure you update the NUM_MSGS variable when adding messages

## Function
This project utilised a MAX7219 32x8 LED display, PIR sensor module and lithium battery (3.7V). The MCU is in sleep mode at all times unless triggered by the interrupt (PIR) on D2. It wakes up, scrolls a message and goes back to sleep. If the battery is low, the program will scroll low battery and sleep again

### Dependencies
- MD_Parola.h
- MD_MAX72xx.h

### Contact Details
Michael Jennings: mjennings061@gmail.com