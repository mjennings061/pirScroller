/*
 * pirScroller
 * Preset message will be displayed on a 32x8 LED matrix when a PIR sensor is triggered
*/

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Turn on debug statements to the serial output
#define DEBUG 1// enables debug serial prints
#ifdef DEBUG
#define DEBUG_PRINT(x)      Serial.print (x)
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
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

// LED matrix variables
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1000; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
#define NUM_MSGS 6
char startMessage[BUF_SIZE] = {"Scrolly boi V1"};
char message[NUM_MSGS][BUF_SIZE] = {  //display messages - max length 75 characters
                                    "UU > QUB",
                                    "Welcome to No.42",
                                    "The mitochondria are the powerhouse of the cell",
                                    "I am a PIR sensor",
                                    "I can see you ;)",
                                    "Target spotted"
                                    };
volatile byte motion = 0; //ISR trigger for the PIR sensor

void setup(){
  Serial.begin(115200);
  DEBUG_PRINTLN("\nPIR Scrolly boi. Trigger me by walking in front of the sensor");
  pinMode(PIR_PIN, INPUT);
  randomSeed(analogRead(0)); //for the random num generator
  
  P.begin();
  P.displayText(startMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect); //starting message
  while(!P.displayAnimate()){;} //scroll the text once
  P.displayClear();
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), isr, RISING); //start the PIR interrupt
}

void loop(){
  if(motion == 1){
    detachInterrupt(digitalPinToInterrupt(PIR_PIN));  //stop the interrupt from being called during processing
    DEBUG_PRINT("Triggered - REEEE\t");
    uint8_t iMsg = random(NUM_MSGS);  //pick a random message to display
    DEBUG_PRINT("Message no.: ");
    DEBUG_PRINTLN(iMsg);
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
    attachInterrupt(digitalPinToInterrupt(PIR_PIN), isr, RISING); //re-attach interrupt
  }
}

// Interrupt service routine
void isr() { 
  motion = 1;
}
