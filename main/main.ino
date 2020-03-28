/*
 * pirScroller
 * Preset message will be displayed on a 32x8 LED matrix when a PIR sensor is triggered
*/

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Turn on debug statements to the serial output
#define DEBUG 0
#if DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
#define PIR_PIN   2

// LED matrix variables
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1000; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
char startMessage[BUF_SIZE] = {"Scrolly boi"};
char message[BUF_SIZE] = {"Apoorva sucks"};

//PIR variables
volatile byte motion = 0;

void setup(){
  Serial.begin(115200);
  Serial.println("\nPIR Scrolly boi. Trigger me by walking in front of the sensor");
  pinMode(PIR_PIN, INPUT);
  
  P.begin();
  P.displayText(startMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
//  delay(1000);
  P.displayClear();
//  attachInterrupt(digitalPinToInterrupt(PIR_PIN), motion, RISING);
}

void loop(){
  if (P.displayAnimate()){
    P.displayReset();
    P.displayText(message, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  }
  
  motion = digitalRead(PIR_PIN);
  Serial.print("PIR state: ");
  Serial.println(motion);
}

//void motion() {
//  state = !state;
//}
