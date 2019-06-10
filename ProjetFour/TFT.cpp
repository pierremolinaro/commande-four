#include "TFT.h"
#include "TemperatureSensor.h"
#include "MinFreeHeap.h"
#include "SplashScreen.h"

//----------------------------------------------------------------------------------------------------------------------

TFT_eSPI tft = TFT_eSPI () ;

//----------------------------------------------------------------------------------------------------------------------

void printWithPadding (const char * inString, const uint32_t inSize) {
  tft.print (inString) ;
  for (uint32_t i = strlen (inString) ; i < inSize ; i++) {
    tft.print (" ") ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   DRAW SPLASH SCREEN
//----------------------------------------------------------------------------------------------------------------------

static void drawSplashScreen (void) {
  int16_t y = SPLASH_SCREEN_HEIGHT - 1 ;
  const uint16_t x = 0 ;
  uint16_t buffer [SPLASH_SCREEN_WIDTH] ;
  for (uint16_t row = 0 ; row < SPLASH_SCREEN_HEIGHT ; row++) {
    for (uint32_t i= 0 ; i<SPLASH_SCREEN_WIDTH ; i++) {
      const uint16_t color = colorTable [splashScreen [SPLASH_SCREEN_WIDTH * row + i]] ;
      buffer [i] = color ;
    }
    tft.pushImage (x, y, SPLASH_SCREEN_WIDTH, 1, buffer);
    y -= 1 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   INIT
//----------------------------------------------------------------------------------------------------------------------
// This function initialized the TFT screen and sets the rotation in landscape, and draws the logo of Centrale Nantes.

void initScreen(void) {
//--- Reset TFT
  pinMode (TFT_RESET, OUTPUT) ;
  digitalWrite (TFT_RESET, LOW) ;
  delay (10) ;
  digitalWrite (TFT_RESET, HIGH) ;
//--- Init
  tft.init () ;
  tft.setRotation (1) ;  // 0 & 2 Portrait. 1 & 3 landscape
 //--- Splash screen
   drawSplashScreen () ;
  delay (2000) ;
//--- Clear screen
  tft.fillScreen (TFT_BLACK); // black screen
  tft.setTextSize (2) ;
  tft.setCursor (0, 0);
}

/*====================================================================================*
 *                                 clearScreen                                        *
 *====================================================================================*
 * This function clears all the screen except the bottom where are printed the
 * permanent information.
 */
void clearScreen(void) {
    tft.fillRect(0, 0, tft.width(), 201, TFT_BLACK);
}

/*====================================================================================*
 *                                   setLineColumnForTextSize                         *
 *====================================================================================*
 * This function sets the cursor at the given lign and at the first column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setLineColumnForTextSize (const uint8_t inLine,
                               const uint8_t inColumn,
                               const uint8_t inTextSize,
                               const bool inAddHalfLine) {
  uint16_t y = inLine * 8 ;
  if (inAddHalfLine) {
    y += 4 ;
  }
  tft.setCursor (inColumn * 6 * inTextSize + 1, y * inTextSize + 1);
}

/*====================================================================================*
 *                                  setColumn                                         *
 *====================================================================================*
 * This function sets the cursor at the same lign and at the given column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setColumnForTextSize (uint8_t column, uint8_t textSize) {
    tft.setCursor(column*6*textSize + 1, tft.getCursorY());
}

//----------------------------------------------------------------------------------------------------------------------

void setMenuColor (const bool inIsSelected, const bool inIsEditing) {
  if (inIsEditing && inIsSelected) {
    tft.setTextColor (TFT_BLACK, TFT_YELLOW) ;
  }else if (inIsSelected) {
    tft.setTextColor (TFT_BLACK, TFT_CYAN) ;
  }else{
    tft.setTextColor (TFT_GREEN, TFT_BLACK) ;
  } 
}

//----------------------------------------------------------------------------------------------------------------------

void afficherCompteurErreurs (const char * inMessage, const uint32_t inNombreErreurs) {
  tft.setTextColor (TFT_WHITE, TFT_BLACK);    
  tft.print (inMessage) ;
  if (inNombreErreurs == 0) {
    tft.setTextColor (TFT_GREEN, TFT_BLACK);
    tft.print ("0") ;
  }else if (inNombreErreurs < 1000) {
    tft.setTextColor (TFT_ORANGE, TFT_BLACK);
    tft.printf ("%u", inNombreErreurs) ;
  }else{
    tft.setTextColor (TFT_RED, TFT_BLACK);
    tft.print ("999+") ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

void printColoredStatus (const uint32_t inValue) {
  if (inValue == 0) {
    tft.setTextColor (TFT_GREEN, TFT_BLACK) ;
    tft.print ("0") ;    
  }else if (inValue < 1000) {
    tft.setTextColor (TFT_ORANGE, TFT_BLACK) ;
    tft.print (inValue) ;    
  }else{
    tft.setTextColor (TFT_RED, TFT_BLACK) ;
    tft.print ("999+") ;    
  }
}

//----------------------------------------------------------------------------------------------------------------------
