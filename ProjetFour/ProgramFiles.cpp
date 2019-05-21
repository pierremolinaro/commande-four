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

static ProgramDescriptor programDescriptor ;

//----------------------------------------------------------------------------------------------------------------------

bool readProgramFile (const String & inFileName) {
  programDescriptor.mPointCount = 0 ;
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
        ok = programDescriptor.mPointCount < PROGRAM_POINT_MAX_COUNT ;
      }
      if (ok) {
        programDescriptor.mPoints [programDescriptor.mPointCount].mTime = timePoint ;
        programDescriptor.mPoints [programDescriptor.mPointCount].mTemperatureReference = temperaturePoint ;
        programDescriptor.mPointCount += 1 ;
      }
    }
  }
  f.close () ;
//--- Check program validity
  if (ok) {
    ok = programDescriptor.mPointCount > 1 ;
  }
  if (ok) {
    ok = programDescriptor.mPoints [0].mTime == 0 ;
  }
  if (ok) {
    for (uint32_t i = 1 ; (i < programDescriptor.mPointCount) && ok ; i++) {
      ok = programDescriptor.mPoints [i - 1].mTime < programDescriptor.mPoints [i].mTime ;
    }
  }
//----
  return ok ;
}

//----------------------------------------------------------------------------------------------------------------------
// DISPLAY PROGRAM
//----------------------------------------------------------------------------------------------------------------------

void plotGraph (void) {
  if (programDescriptor.mPointCount > 1) {
    const uint32_t tmax = programDescriptor.mPoints [programDescriptor.mPointCount - 1].mTime ;
  //--- Compute temperature max
    uint32_t maxTemperature = 0 ;
    uint32_t minTemperature = UINT32_MAX ;
    for (uint32_t i=0 ; i<programDescriptor.mPointCount ; i++) {
      const uint32_t t = programDescriptor.mPoints [i].mTemperatureReference ;
      maxTemperature = max (maxTemperature, t) ;
      minTemperature = min (minTemperature, t) ;
    }
    const uint32_t w = 298 ;
    const uint32_t h = 160 ;
    tft.setCursor (16, 32) ;
    tft.drawRect (tft.getCursorX (), tft.getCursorY (), w, h, TFT_WHITE) ;

    uint16_t x0 = tft.getCursorX () + programDescriptor.mPoints[0].mTime / tmax * w ;
    uint16_t y0 = tft.getCursorY () + h - programDescriptor.mPoints[0].mTemperatureReference / maxTemperature * h ;
    for (uint8_t i = 1 ; i < programDescriptor.mPointCount ; i++) {
      const uint16_t x1 = tft.getCursorX () + ((double) programDescriptor.mPoints[i].mTime / tmax) * w ;
      const uint16_t y1 = tft.getCursorY () + h - ((double) programDescriptor.mPoints[i].mTemperatureReference / maxTemperature) * h ;
      tft.drawLine (x0, y0, x1, y1, TFT_YELLOW) ; 
      x0 = x1 ; y0 = y1 ;
    }
    tft.setTextColor (TFT_YELLOW, TFT_BLACK) ; tft.setTextSize (1) ;
    tft.setCursor ( 12, 192) ; tft.print ("0") ;
    tft.setCursor (  0,  24) ; tft.printf ("%4u" DEGREE_CHAR "C", maxTemperature) ;
    tft.setCursor (280, 192) ; tft.printf("%4umn", tmax) ;
 //   tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
  }
}

//----------------------------------------------------------------------------------------------------------------------
