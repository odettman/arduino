// UTFT Experiments
// Screen Resolution is 320x240



#include <UTFTGLUE.h>              // class methods are in here
#include <Adafruit_GFX.h>
#include <gfxfont.h>
//#include <Adafruit_TFTLCD.h>
#include <Fonts/FreeSerif18pt7b.h>
#include <FreeDefaultFonts.h>


#define PORTRAIT 0
#define LANDSCAPE 1
#define PORTRAIT_FLIP 2
#define LANDSCAPE_FLIP 3

UTFTGLUE myGLCD(0x0154,A2,A1,A3,A4,A0);

// Declare which fonts we will be using
#if !defined(SmallFont)
extern uint8_t SmallFont[];    //.kbv GLUE defines as GFXFont ref
#endif

#if !defined(BigFont)
extern uint8_t BigFont[];
#endif

void setup() {
  randomSeed(analogRead(5));   //.kbv Due does not like A0
    pinMode(A0, OUTPUT);       //.kbv mcufriend have RD on A0
    digitalWrite(A0, HIGH);
  
// Setup the LCD
  myGLCD.InitLCD(LANDSCAPE_FLIP);
  myGLCD.setFont(BigFont);
  myGLCD.clrScr();

    
  myGLCD.setColor(0,255,0);
  //myGLCD.fillRect(50,50,280,200);

  myGLCD.setColor(255,255,255);
  myGLCD.print("Hello World", CENTER, 90);
}

void loop() {
  int xmax=320;
  int ymax=240;

  
  delay(1000);
}

