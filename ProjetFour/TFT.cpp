// ----------Include the header----------
#include "TFT.h"
#include "TemperatureSensor.h"
#include "MinFreeHeap.h"

// ----------Static variables in the file----------
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

static void drawBmp (const char * inFilePath) {
//--- Open requested file on SD card
  File bmpFS = SD.open (inFilePath) ;
  if (!bmpFS) {
    Serial.println ("File not found") ;
  }else{
    const uint32_t startTime = millis();
    if (read16(bmpFS) == 0x4D42) {
      read32(bmpFS);
      read32(bmpFS);
      const uint32_t seekOffset = read32(bmpFS);
      read32(bmpFS);
      const uint16_t w = read32(bmpFS);
      const uint16_t h = read32(bmpFS);
      Serial.printf ("  Width %u, height %u\n", w, h) ;
      if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
        const int16_t x = 0 ;
        int16_t y = h - 1 ;
        tft.setSwapBytes (true);
        bmpFS.seek (seekOffset);
        const uint16_t padding = (4 - ((w * 3) & 3)) & 3;
        uint8_t lineBuffer [w * 3 + padding];
        for (uint16_t row = 0; row < h ; row++) {
          bmpFS.read (lineBuffer, sizeof(lineBuffer));
          uint8_t*  bptr = lineBuffer;
          uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
          for (uint16_t col = 0; col < w; col++) {
            const uint8_t b = *bptr++;
            const uint8_t g = *bptr++;
            const uint8_t r = *bptr++;
            *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
          }
          // Push the pixel row to screen, pushImage will crop the line if needed
          // y is decremented as the BMP image is drawn bottom up
          tft.pushImage (x, y, w, 1, (uint16_t*) lineBuffer);
          y -= 1 ;
        }
        Serial.printf ("Loaded in %lu ms\n", millis () - startTime) ;
      }else{
        Serial.println("BMP format not recognized.");
      }
    }
  }
  bmpFS.close () ;
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
  drawBmp ("/LogoECN.bmp");
  delay (1000) ;
//--- Clear screen
  tft.fillScreen (TFT_BLACK); // black screen
  tft.setTextSize (2) ;
  tft.setCursor (0, 0);
}

/*====================================================================================*
 *                               read16 & read32                                      *
 *====================================================================================*
 * These functions read 16- and 32-bit types from the SD card file,
 * they are used for the drawBMP function.
 * BMP data is stored little-endian, Arduino is little-endian too.
 * May need to reverse subscript order if porting elsewhere.
 */
uint16_t read16(fs::File &f) {
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t read32(fs::File &f) {
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
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
 *                                   setLineForTextSize                               *
 *====================================================================================*
 * This function sets the cursor at the given lign and at the first column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setLineForTextSize (const uint8_t inLine, const uint8_t inTextSize, const bool inAddHalfLine) {
  uint16_t y = inLine * 8 ;
  if (inAddHalfLine) {
    y += 4 ;
  }
  tft.setCursor (1, y * inTextSize + 1);
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
