/*
 * pirScroller
 * Preset message will be displayed on a 32x8 LED matrix when a PIR sensor is triggered
 * Copyright: Michael Jennings - www.github.com/mjennings061/
 * Last Updated: 13/04/2020
*/

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <avr/sleep.h>

// Turn on debug statements to the serial output
#define DEBUG 0 // Switch debug output on and off by 1 or 0
#if DEBUG
#define PRINTS(s)   { Serial.print(s); }
#else
#define PRINTS(s)
#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW //display hardware
#define MAX_DEVICES   4   //number of LED displays
#define CLK_PIN       13  //clock
#define DATA_PIN      11  //data pin
#define CS_PIN        10  //clock select pin
#define BAT_PIN       A5  //battery pin

// LED matrix variables
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollSpeed = 35;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1000; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	40  //max length of messages
#define NUM_MSGS 17  //update this with the number of messages
char startMessage[BUF_SIZE] = {"Scrolly boi V1"};
char message[NUM_MSGS][BUF_SIZE] = {  //display messages - max length 75 characters
                                    "Welcome, now leave",
                                    "Your face makes onions cry",
                                    "Careful, you're shaking the floor",
                                    "Not you again..",
                                    "If stupidity was painful, you would be in agony",
                                    "Oh lawd he comin",
                                    "Can you not take a hint?",
                                    "I only sense oxygen-thieves",
                                    "Get COVID-19 please",
                                    "Make like a tree and leaf",
                                    "Make like a saw and buzz off",
                                    "Be like me, inanimate",
                                    "You sure you want to be seen like that?",
                                    "If you had brains, you'd be dangerous",
                                    "Get rekt fgt",
                                    "You are about one bit short of a byte",
                                    "I do desire we may be better strangers"
                                    };
uint8_t adcReg = 0; // ADC register's startup state
float batVolt = 0;  //global battery voltage

///////////// SETUP //////////////////
void setup(){
  #ifdef DEBUG  // only use UART when DEBUG is enables
    Serial.begin(115200);
  #endif
  PRINTS("\nPIR Scrolly boi. Trigger me by walking in front of the sensor");
  randomSeed(analogRead(0)); //for the random num generator
  

  //setup the display
  batVolt = checkBattery(); //check the battery voltage during startup
  String batString = "Battery: " + String(batVolt) + "V";  // make a string with the battery voltage
  char batChar[BUF_SIZE];
  batString.toCharArray(batChar,BUF_SIZE);  //convert string to char array

  // Welcome message
  P.begin();
  P.displayText(startMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect); //starting message
  while(!P.displayAnimate()){;} //scroll the text once
  P.displayClear();
  delay(500);

  // Show battery
  P.displayReset();
  P.displayText(batChar, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect); //starting message
  while(!P.displayAnimate()){;} //scroll the text once
  P.displayClear();
  delay(1000);
}

///////////// MAIN LOOP //////////////////
void loop(){
  batVolt = checkBattery();       // check the battery voltage
  if(batVolt <= 3.5 && batVolt > 3.2){  // if its above 3.3V but below 3.1V 
    lowBattery(batVolt);                // display a low battery message
  } else if(batVolt > 3.3){             // if its in normal range
    scrollMessage();                    // scroll a random message
  }
  sleepTime();  //put the CPU to sleep
  //// MCU IS ASLEEP HERE ////
}

// Interrupt service routine
void wake (){
  sleep_disable();  // cancel sleep as a precaution
  ADCSRA = adcReg;  //turn the ADC back on
  detachInterrupt (0); // precautionary while we do other stuff
}  // end of wake

// Put the MCU to sleep
void sleepTime(){
  ADCSRA = 0; // disable ADC
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();
  noInterrupts (); // Do not interrupt before we go to sleep
  attachInterrupt (0, wake, RISING); // will be called when pin D2 goes high 
  EIFR = bit(INTF0);  // clear flag for interrupt 0
  interrupts ();  // one cycle, allows next line's execution before sleeping
  sleep_cpu ();   // one cycle
}

// Random message scroller
void scrollMessage(){
  PRINTS("\nTriggered - REEEE\t");
  uint8_t iMsg = random(NUM_MSGS);  //pick a random message to display
  PRINTS("Message no.: ");
  PRINTS(iMsg);
  uint8_t nLoops = 0; //number of times to display the message
  P.displayReset();
  P.displayText(message[iMsg], scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);  //display a random message
  while(nLoops < 2){  //wait until the message has displayed twice
    if (P.displayAnimate()){  //update the display (call repeatedly)
      P.displayReset(); 
      nLoops++; //
    }
  }
  delay(1000);
}

// Report battery voltage
float checkBattery(){
  int batIn = analogRead(BAT_PIN);
  float batIn_float = float(batIn);
  float batVoltage = batIn_float*(5.0/1023.0);
  PRINTS("\nBattery: ");
  PRINTS(batVoltage);
  PRINTS("V\tRaw: ");
  PRINTS(batIn)
  return batVoltage;
}

// Low battery warning
void lowBattery(float voltage){
  uint8_t nLoops = 0; //number of times to display the message
  String outString = "Low Battery: " + String(voltage);  // make a string with the battery voltage
  char outChar[20];
  outString.toCharArray(outChar,20);  //convert string to char array
  
  P.displayReset();
  P.displayText(outChar, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);  //display a random message
  while(nLoops < 2){  //wait until the message has displayed twice
    if (P.displayAnimate()){  //update the display (call repeatedly)
      P.displayReset(); 
      nLoops++; //
    }
  }
  delay(1000);
}

/*
 * To-do
 * - More noticable low battery warning
 */
