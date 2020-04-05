/*
 * pirScroller
 * Preset message will be displayed on a 32x8 LED matrix when a PIR sensor is triggered
 * Copyright: Michael Jennings - www.github.com/mjennings061/
 * Last Updated: 05/04/2020
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
#define MAX_DEVICES 4 //number of LED displays
#define CLK_PIN   13  //clock
#define DATA_PIN  11  //data pin
#define CS_PIN    10  //clock select pin
#define PIR_PIN   2   //PIR pin
#define BAT_PIN   A4  //battery pin

// LED matrix variables
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollSpeed = 35;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1000; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
#define NUM_MSGS 6  //update this with the number of messages
char startMessage[BUF_SIZE] = {"Scrolly boi V1"};
char message[NUM_MSGS][BUF_SIZE] = {  //display messages - max length 75 characters
                                    "Welcome to No.42",
                                    "Don't let your memes be dreams",
                                    "The mitochondria are the powerhouses of the cell",
                                    "I am a PIR sensor",
                                    "We are bored students",
                                    "Delivery drivers are great people"
                                    };
volatile byte motion = 0; //ISR trigger for the PIR sensor
bool lowBat = 0;  //set to 1 when battery is below 3.2V

///////////// SETUP //////////////////
void setup(){
  #ifdef DEBUG  // only use UART when DEBUG is enables
    Serial.begin(115200);
  #endif
  PRINTS("\nPIR Scrolly boi. Trigger me by walking in front of the sensor");
  pinMode(PIR_PIN, INPUT);
  pinMode(BAT_PIN, INPUT);
  randomSeed(analogRead(0)); //for the random num generator

  //setip the display
  P.begin();
  P.displayText(startMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect); //starting message
  while(!P.displayAnimate()){;} //scroll the text once
  P.displayClear();
}

///////////// MAIN LOOP //////////////////
void loop(){
  if(motion == 1){
    checkBattery();
    if(lowBat == 0){
      scrollMessage();
    }
  }
  delay(1000);
  goToSleep();
}

// Interrupt service routine
void isr() { 
  motion = 1; //set motion variable
  sleep_disable();  //wake up
  detachInterrupt(digitalPinToInterrupt(PIR_PIN));  //stop the interrupt from being called during processing
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
  motion = 0; //reset the flag
}

// Report battery voltage and set lowBat
void checkBattery(){
  int batIn = analogRead(BAT_PIN);
  float batVoltage = batIn*(5.0/1023.0);
  PRINTS("\nBattery: ");
  PRINTS(batVoltage);
  if(batVoltage < 3.2) {  //scroll "Low Battery" when voltage below 3.2V
    lowBat = 1;
    uint8_t nLoops = 0; //number of times to display the message
    P.displayReset();
    P.displayText("Low Battery", scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);  //display a random message
    while(nLoops < 2){  //wait until the message has displayed twice
      if (P.displayAnimate()){  //update the display (call repeatedly)
        P.displayReset(); 
        nLoops++; //
      }
    }
    motion = 0; //reset the flag
  }
  else {
    lowBat = 0; // battery is not low
  }
}

void goToSleep(){
  PRINTS("\nSleep time\n");
  delay(100);
  sleep_enable(); // enable sleep mode
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), isr, RISING); //start the PIR interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();
}

/*
 * To-do
 * - More noticable low battery warning
 */
