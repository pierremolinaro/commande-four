//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "Defines.h"
#include "ProgramFiles.h"
#include "SDCard.h"
#include "TFT.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static const uint32_t FILE_NAME_ARRAY_SIZE = 100 ;
static String gFileNameArray [FILE_NAME_ARRAY_SIZE] ;
static uint32_t gCurrentFileNameCount ;
static uint32_t gCurrentSDCardMountIndex ;
static bool gSDCardMounted ;

//----------------------------------------------------------------------------------------------------------------------
//   RESET PROGRAM LIST
//----------------------------------------------------------------------------------------------------------------------

void resetProgramList (void) {
  gSDCardMounted = false ;
}

//----------------------------------------------------------------------------------------------------------------------

static void filterFilePath (String & ioFilePath, bool & outRetain) {
//--- Suppress path
  const int idx = ioFilePath.lastIndexOf ('/') ;
  if (idx > 0) {
    ioFilePath = ioFilePath.substring (idx + 1) ;
    outRetain = true ;
  }
//--- Check file extension is ".csv" et ".CSV"
  if (outRetain) {
    const int idx = ioFilePath.indexOf ('.') ;
    outRetain = (idx > 0) && (idx == (ioFilePath.length () - 4)) ;
    if (outRetain) {
      const String extension = ioFilePath.substring (idx + 1) ;
      outRetain = extension.equalsIgnoreCase ("csv") ;
    }
    if (outRetain) {
      ioFilePath = ioFilePath.substring (0, idx) ;
    }
    if (outRetain) {
      outRetain = ioFilePath.length () <= FILE_BASE_NAME_MAX_LENGTH ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

static void swap (String & ioLeft, String & ioRight) {
  const String s = ioLeft ;
  ioLeft = ioRight ;
  ioRight = s ;
}

//----------------------------------------------------------------------------------------------------------------------

static void sortProgramFileNameArray (const int32_t inFirst, const int32_t inLast) {
//--- Sort using 'quick sort' algorithm
  if (inFirst < inLast) {
    int32_t i = inFirst ;
    int32_t j = inLast ;
    while (i < j) {
      while ((i < inLast) && (gFileNameArray [i] <= gFileNameArray [inFirst])) {
        i ++ ;
      }
      while ((j > inFirst) && (gFileNameArray [j] >= gFileNameArray [inFirst])) {
        j -- ;
      }
      if (i < j) {
        swap (gFileNameArray [i], gFileNameArray [j]) ;
      }
    }
    swap (gFileNameArray [j], gFileNameArray [inFirst]) ;
    sortProgramFileNameArray (inFirst, j-1) ;
    sortProgramFileNameArray (j+1, inLast) ;
  }  
}

//----------------------------------------------------------------------------------------------------------------------
//   BUILD PROGRAM FILE ARRAY
//----------------------------------------------------------------------------------------------------------------------

void buildProgramFileNameArray (void) {
  bool build = false ;
  const bool mounted = sdCardStatus () == SDCardStatus::mounted ;
  if (mounted && !gSDCardMounted) {
    build = true ;
    gSDCardMounted = true ; 
  }else if (gSDCardMounted  && !mounted) {
    gCurrentFileNameCount = 0 ;
    gSDCardMounted = false ; 
  }else if (gSDCardMounted && mounted && (gCurrentSDCardMountIndex != mountIndex ())) {
    gCurrentSDCardMountIndex = mountIndex () ;
    build = true ;
  }
//--- Build file name list ?
  if (build) {
    gCurrentFileNameCount = 0 ;
    File root = SD.open (PROFILES_DIRECTORY);
    root.rewindDirectory () ;
    File file = root.openNextFile () ;
    while (file && (gCurrentFileNameCount < FILE_NAME_ARRAY_SIZE)) {
      String filePath = file.name () ;
      bool retain = false ;
      filterFilePath (filePath, retain) ;
      if (retain) {
        gFileNameArray [gCurrentFileNameCount] = filePath ;
        gCurrentFileNameCount += 1 ;
      }
      file = root.openNextFile () ;
    }
    file.close () ;
    root.close () ;
    sortProgramFileNameArray (0, ((int32_t) gCurrentFileNameCount) - 1) ; 
  }
}

//----------------------------------------------------------------------------------------------------------------------

uint32_t programFileCount (void) {
  return gCurrentFileNameCount ;
}

//----------------------------------------------------------------------------------------------------------------------

String programFileNameAtIndex (const uint32_t inIndex) {
  return (inIndex < gCurrentFileNameCount) ? gFileNameArray [inIndex] : "" ;
}

//----------------------------------------------------------------------------------------------------------------------
//  READ PROGRAM FILE
//----------------------------------------------------------------------------------------------------------------------

static void passSpaces (File & inFile, char & ioCurrentChar, bool & ioOk) {
  bool loop = ioCurrentChar == ' ' ;
  while (loop && ioOk) {
    ioOk = inFile.available () ;
    if (ioOk) {
      ioCurrentChar = inFile.read () ;
      loop = ioCurrentChar == ' ' ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------

static uint16_t analyzeUnsignedInteger (File & inFile, char & ioCurrentChar, bool & ioOk) {
  ioOk = (ioCurrentChar >= '0') && (ioCurrentChar <= '9') ;
  uint16_t result = ioCurrentChar - '0' ;
  bool loop = true ;
  while (loop && ioOk) {
    ioOk = inFile.available () ;
    if (ioOk) {
      ioCurrentChar = inFile.read () ;
      if ((ioCurrentChar >= '0') && (ioCurrentChar <= '9')) {
        result *= 10 ;
        result += ioCurrentChar - '0' ;
      }else{
        loop = false ;
      }
    }
  }
  return result ;
}

//----------------------------------------------------------------------------------------------------------------------

typedef struct {
    uint16_t mTime ; // In minutes
    uint16_t mTemperatureReference ; // In Celcius
} ProgramPoint ;

//.....................................................................................................................

typedef struct {
    uint32_t mPointCount ;
    ProgramPoint mPoints [PROGRAM_POINT_MAX_COUNT] ;
} ProgramDescriptor ;

//.....................................................................................................................

static ProgramDescriptor gProgramDescriptor ;
static String gFileName ;

//----------------------------------------------------------------------------------------------------------------------

String programName (void) {
  return gFileName ;
}

//----------------------------------------------------------------------------------------------------------------------

uint32_t programDurationInMinutes (void) {
  uint32_t result = 0 ;
  if (gProgramDescriptor.mPointCount > 0) {
    result = gProgramDescriptor.mPoints [gProgramDescriptor.mPointCount - 1].mTime ;
  }
  return result ;
}

//----------------------------------------------------------------------------------------------------------------------

double programTemperatureReferenceForRunningTime (const uint32_t inRunningTimeInSeconds) {
  double result = 0.0 ;
  if (gProgramDescriptor.mPointCount > 0) {
    uint32_t idx = 1 ;
    while ((idx < gProgramDescriptor.mPointCount) && (inRunningTimeInSeconds > (gProgramDescriptor.mPoints [idx].mTime * 60))) {
      idx += 1 ;
    }
    if (idx < gProgramDescriptor.mPointCount) {
      const double referenceStart = (double) gProgramDescriptor.mPoints [idx - 1].mTemperatureReference ;
      const uint32_t timeStart = gProgramDescriptor.mPoints [idx - 1].mTime * 60 ;
      const double referenceEnd = (double) gProgramDescriptor.mPoints [idx].mTemperatureReference ;
      const uint32_t timeEnd = gProgramDescriptor.mPoints [idx].mTime * 60 ;
      const double t = (double) (inRunningTimeInSeconds - timeStart) ;
      const double stepDuration = (double) (timeEnd - timeStart) ;
      result = referenceStart + (referenceEnd - referenceStart) * t / stepDuration ;
    }
  }
  return result ;
}

//----------------------------------------------------------------------------------------------------------------------

bool readProgramFile (const String & inFileName) {
  gFileName = inFileName ;
  gProgramDescriptor.mPointCount = 0 ;
  const String filePath = String (PROFILES_DIRECTORY) + '/' + inFileName + ".CSV" ;
  // Serial.printf ("File path '%s'\n", filePath.c_str ()) ;
  File f = SD.open (filePath, FILE_READ) ;
  bool ok = f && f.available () ;
  // Serial.printf ("  Open %d\n", ok) ;
  if (ok) {
    while (ok && f.available ()) {
      char currentChar = f.read () ; // current character in the file
    //--- Time
      passSpaces (f, currentChar, ok) ;
      // Serial.printf ("  passSpaces %d\n", ok) ;
      const uint16_t timePoint = analyzeUnsignedInteger (f, currentChar, ok) ;
      // Serial.printf ("  analyzeUnsignedInteger %d\n", ok) ;
      passSpaces (f, currentChar, ok) ;
      // Serial.printf ("  passSpaces %d\n", ok) ;
    //--- Separator ';'
      ok = currentChar == ';' ;
      if (ok) {
        ok = f.available () ;
        if (ok) {
          currentChar = f.read () ;
        }
      }
    //--- Temperature
      // Serial.printf ("  separator %d\n", ok) ;
      passSpaces (f, currentChar, ok) ;
      // Serial.printf ("  passSpaces %d\n", ok) ;
      const uint16_t temperaturePoint = analyzeUnsignedInteger (f, currentChar, ok) ;
      // Serial.printf ("  temperaturePoint %d\n", ok) ;
      passSpaces (f, currentChar, ok) ;
      // Serial.printf ("  passSpaces %d\n", ok) ;
   //--- End of line
      if (ok && (currentChar == '\r')) {
        ok = f.available () ;
        if (ok) {
          currentChar = f.read () ;
        }
      }
      if (ok) {
        ok = currentChar == '\n' ;       
      }
    //--- Enter point
      if (ok) {
        ok = gProgramDescriptor.mPointCount < PROGRAM_POINT_MAX_COUNT ;
      }
      if (ok) {
        gProgramDescriptor.mPoints [gProgramDescriptor.mPointCount].mTime = timePoint ;
        gProgramDescriptor.mPoints [gProgramDescriptor.mPointCount].mTemperatureReference = temperaturePoint ;
        gProgramDescriptor.mPointCount += 1 ;
      }
    }
  }
  f.close () ;
//--- Check program validity
  if (ok) {
    ok = gProgramDescriptor.mPointCount > 1 ;
  }
  if (ok) {
    ok = gProgramDescriptor.mPoints [0].mTime == 0 ;
  }
  if (ok) {
    for (uint32_t i = 1 ; (i < gProgramDescriptor.mPointCount) && ok ; i++) {
      ok = gProgramDescriptor.mPoints [i - 1].mTime < gProgramDescriptor.mPoints [i].mTime ;
    }
  }
//----
  return ok ;
}

//----------------------------------------------------------------------------------------------------------------------
// PLOT GRAPH
//----------------------------------------------------------------------------------------------------------------------

void plotGraph (void) {
  setLineColumnForTextSize (0, 0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (true, false) ;
  tft.print ("Suivant") ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" ") ;
  tft.print (gFileName) ;
  if (gProgramDescriptor.mPointCount > 1) {
    const uint32_t tmax = gProgramDescriptor.mPoints [gProgramDescriptor.mPointCount - 1].mTime ;
  //--- Compute temperature max
    uint32_t maxTemperature = 0 ;
    uint32_t minTemperature = UINT32_MAX ;
    for (uint32_t i=0 ; i<gProgramDescriptor.mPointCount ; i++) {
      const uint32_t t = gProgramDescriptor.mPoints [i].mTemperatureReference ;
      maxTemperature = max (maxTemperature, t) ;
      minTemperature = min (minTemperature, t) ;
    }
    const uint32_t w = 298 ;
    const uint32_t h = 160 ;
    tft.setCursor (16, 32) ;
    tft.drawRect (tft.getCursorX (), tft.getCursorY (), w, h, TFT_WHITE) ;

    uint16_t x0 = tft.getCursorX () + gProgramDescriptor.mPoints[0].mTime / tmax * w ;
    uint16_t y0 = tft.getCursorY () + h - gProgramDescriptor.mPoints[0].mTemperatureReference / maxTemperature * h ;
    for (uint8_t i = 1 ; i < gProgramDescriptor.mPointCount ; i++) {
      const uint16_t x1 = tft.getCursorX () + ((double) gProgramDescriptor.mPoints[i].mTime / tmax) * w ;
      const uint16_t y1 = tft.getCursorY () + h - ((double) gProgramDescriptor.mPoints[i].mTemperatureReference / maxTemperature) * h ;
      tft.drawLine (x0, y0, x1, y1, TFT_YELLOW) ; 
      x0 = x1 ; y0 = y1 ;
    }
    tft.setTextColor (TFT_YELLOW, TFT_BLACK) ; tft.setTextSize (1) ;
    tft.setCursor ( 12, 192) ; tft.print ("0") ;
    tft.setCursor (  0,  24) ; tft.printf ("%4u" DEGREE_CHAR "C", maxTemperature) ;
    tft.setCursor (280, 192) ; tft.printf("%4umn", tmax) ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// PRINT TABLE
//----------------------------------------------------------------------------------------------------------------------

static void drawGrid (void) {
  tft.setTextSize(2); tft.setTextColor(TFT_WHITE);
  const uint16_t smallLineColor = TFT_NAVY ;
  const uint16_t largeLineColor = TFT_LIGHTGREY ;
// ----------Write the name of the table lines----------
  setLineColumnForTextSize ( 5, 1, 1) ; tft.print ("Temps(min)");
  setLineColumnForTextSize ( 7, 1, 1) ; tft.print ("Temp" LOWERCASE_E_ACUTE "rature(" DEGREE_CHAR "C)") ;
// ----------Draw the small lines----------
  setLineColumnForTextSize ( 7, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, smallLineColor);
  setLineColumnForTextSize (11, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, smallLineColor);
  setLineColumnForTextSize (23, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  1*2*8, smallLineColor);
  setLineColumnForTextSize ( 9, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1,  5*2*6,  8*2*8, smallLineColor);
  setLineColumnForTextSize ( 5, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1,  5*2*6, 10*2*8, smallLineColor);
// ----------Draw the large lines of the table----------
  setLineColumnForTextSize ( 5, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, largeLineColor);
  setLineColumnForTextSize ( 9, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  6*2*8, largeLineColor);
  setLineColumnForTextSize (17, 1, 1) ; tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, largeLineColor);
  setLineColumnForTextSize ( 5, 1, 1) ; tft.drawFastVLine(tft.getCursorX()-1, tft.getCursorY()-1, 2*2*8, largeLineColor);
}

//----------------------------------------------------------------------------------------------------------------------

static void setCursorForTable (const uint8_t numTime, const bool TimeOrTemp) { // TimeOrTemp : 0 -> time, 1 -> temp
  const uint8_t lign   = (numTime+3)/5 ;
  const uint8_t column = (numTime+3)%5 ;
  setLineColumnForTextSize (5+lign*2*2 + TimeOrTemp*2, 2+column*5*2, 1) ;
}

//----------------------------------------------------------------------------------------------------------------------

void printTable (const char * inTitle) {
  setLineColumnForTextSize (0, 0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (true, false) ;
  tft.print (inTitle) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" ") ;
  tft.print (gFileName) ;
//---
  drawGrid () ;
//--- Print values
  tft.setTextSize (2) ;
  for (uint32_t i = 0 ; i < min (gProgramDescriptor.mPointCount, 26) ; i++) {
    tft.setTextColor (TFT_YELLOW) ; setCursorForTable (i, false) ; tft.printf("%4u", gProgramDescriptor.mPoints[i].mTime) ;
    tft.setTextColor (TFT_PINK) ; setCursorForTable (i, true) ; tft.printf("%4u", gProgramDescriptor.mPoints[i].mTemperatureReference) ;
  }

}

//----------------------------------------------------------------------------------------------------------------------
