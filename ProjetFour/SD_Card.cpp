#include "SD_Card.h"
#include "Backlight.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   INIT
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void initSDcard (void) {
  pinMode (SDCARD_CD, INPUT) ;
  updateSDCardStatus () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   STATIC VARIABLES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static SPIClass hspi (HSPI) ;
static SDCardStatus gSDCardStatus = SDCardStatus::noCard ; // Records current SD Card status

//--------------------------------------------------------------------------------------------------------

void updateSDCardStatus (void) {
  const bool inserted = digitalRead (SDCARD_CD) == HIGH ;
  switch (gSDCardStatus) {
  case SDCardStatus::noCard :
    if (inserted) {
      extendBackLightDuration () ;
      const bool mounted = SD.begin (SDCARD_CS, hspi) ; // SS spin, SPIClass -> HSPI
      if (mounted) {
        gSDCardStatus = SDCardStatus::mounted ;
      }else{
        SD.end () ;
        gSDCardStatus = SDCardStatus::insertedNotMounted ;
      }
    }
    break ;
  case SDCardStatus::mounted :
    if (!inserted) {
      extendBackLightDuration () ;
      SD.end () ;
      gSDCardStatus = SDCardStatus::noCard ;
    }
    break ;
  case SDCardStatus::insertedNotMounted :
    if (inserted) {
      const bool mounted = SD.begin (SDCARD_CS, hspi) ; // SS spin, SPIClass -> HSPI
      if (mounted) {
        extendBackLightDuration () ;
        gSDCardStatus = SDCardStatus::mounted ;
      }else{
        SD.end () ;
      }
    }else{
      extendBackLightDuration () ;
      gSDCardStatus = SDCardStatus::noCard ;
    }
    break ;
  }
}

//--------------------------------------------------------------------------------------------------------

SDCardStatus sdCardStatus (void) {
  return gSDCardStatus ;
}

//--------------------------------------------------------------------------------------------------------

bool directoryExists (const String & inPath) {
  bool result = false ;
  File f = SD.open (inPath) ;
  if (f) {
    result = f.isDirectory () ;
  }
  f.close () ;
  return result ;  
}

//--------------------------------------------------------------------------------------------------------

bool createDirectory (const String & inPath) {
  return SD.mkdir (inPath) ;
}

//--------------------------------------------------------------------------------------------------------

File openFileForCreation (const String & inFilePath) {
  return SD.open (inFilePath, FILE_WRITE) ;
}

//--------------------------------------------------------------------------------------------------------

File openFileForAppending (const String & inFilePath) {
  return SD.open (inFilePath, FILE_APPEND) ;
}

/*====================================================================================*
 *                                   writeFile                                        *
 *====================================================================================*
 * This function creates a file (or replace it) and write on it the message given. 
 */
void writeFile(String path, String message) {
    File file = SD.open(path.c_str(), FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(!file.print(message)){
        Serial.println("Write failed");
    }
    file.close();
}

/*====================================================================================*
 *                                  appendFile                                        *
 *====================================================================================*
 * This function appends to the file the message given, or creates a file with the 
 * message.
 */
void appendFile(String path, String message) {
    File file = SD.open(path.c_str(), FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(!file.print(message)){
        Serial.println("Append failed");
    }
    file.close();
}

/*====================================================================================*
 *                                  renameFile                                        *
 *====================================================================================*
 * This function renames the first file given with the second name given.
 */
void renameFile(String path1, String path2) {
    if (!SD.rename(path1.c_str(), path2.c_str())) {
        Serial.println("Rename failed");
    }
}

/*====================================================================================*
 *                                  deleteFile                                        *
 *====================================================================================*
 * This function deletes the file.
 */
void deleteFile(String path) {
    if(!SD.remove(path.c_str())){
        Serial.println("Delete failed");
    }
}

/*====================================================================================*
 *                                    giveTemp                                        *
 *====================================================================================*
 * This function gives the wanted temperature read on the SD card,
 * it gives the temperature corresponding to the time just after the given moment.
 */
float giveTemp(String nameOfCurve, uint16_t t) {
    String path = curvesDir + '/' + nameOfCurve + ".CSV";
    File file = SD.open(path.c_str());
    
    String w = "";
    char c = ' '; // current character in the table
    double temp = 0; 
    uint16_t t0 = 0;

    while(file.available()){
        w = ""; // reset the string w
        c = file.read();
        while (c != ';') {  // ';' separates the two columns of the csv file -> before it is the time 
            w += c;
            c = file.read();
        }
        t0 =  w.toInt(); // string to int

        w = "";  // reset the string w
        c = file.read();
        while (c != '\n') { // '\n' separates two ligns of the csv file -> before it is the temperature
            w += c;
            c = file.read();
        }
        temp = w.toFloat(); // string to float

        if (t0 >= t) {  // return the first temperature with a time greater than the entry t
            file.close();
            return temp;
        }
    }
    file.close();
    return 0;
}

/*====================================================================================*
 *                                  numberFiles                                       *
 *====================================================================================*
 * This function gives the number of files in the given directory.
 */
uint8_t numberFiles(String dirname) {
    File root = SD.open(dirname.c_str());
    Serial.println("Directory opened");
    root.rewindDirectory();
    uint8_t numberOfFiles = 0;
    File file = root.openNextFile();
    while(file){
        Serial.println(file.name());
        numberOfFiles ++;
        file = root.openNextFile();
    }
    return(numberOfFiles);
}

/*====================================================================================*
 *                                getDisplayNames                                     *
 *====================================================================================*
 * This function gives the names of the files in the values directory.
 */
void getDisplayNames(String outFileNameArray []) {
    File root = SD.open(valuesDir.c_str());
    root.rewindDirectory();
    uint8_t len = 0;
    File file = root.openNextFile();
    while(file){
        outFileNameArray [len] = extractValuesNameFile(file.name());
        len ++;
        file = root.openNextFile();
    }
}

/*====================================================================================*
 *                              extractCurveNameFile                                  *
 *====================================================================================*
 * This function gives the name of the curve, by giving the full name 
 * in the curve directory.
 */
String extractCurveNameFile(String fullCurveName) {
    // we remove the /Courbes/ (9 characters) and the .CSV (4)
    return(fullCurveName.substring(9, fullCurveName.length()-4));
}

/*====================================================================================*
 *                              extractValuesNameFile                                 *
 *====================================================================================*
 * This function gives the name of the curve, by giving the full name 
 * in the values directory.
 */
String extractValuesNameFile(String fullValuesName) {
    // we remove the /ValeursCourbes/valeurs (23 characters) and the .CSV (4)
    return(fullValuesName.substring(23, fullValuesName.length()-4));
}

/*====================================================================================*
 *                                    timeMax                                         *
 *====================================================================================*
 * This function gives the maximal time of the given curve.
 */
uint16_t timeMax(String nameOfCurve) {
    String pathValues = valuesDir + "/valeurs" + nameOfCurve + ".CSV";
    File file = SD.open(pathValues.c_str());
    String w = "";
    char c = ' '; // current character in the table
    uint16_t maxTime = 0; // the last value of time

    while(file.available()){
        w = ""; // reset the string w
        c = file.read();
        while (c != ';') {  // ';' separates the two columns of the csv file -> before it is the time 
            w += c;
            c = file.read();
        }
        maxTime = w.toInt(); // string to int

        while (c != '\n') { // '\n' separates two ligns of the csv file -> before it is the temperature
            c = file.read();
        }
    }
    file.close();
    return(maxTime);
}

/*====================================================================================*
 *                                    tempMax                                         *
 *====================================================================================*
 * This function gives the maximal temperature of the given array.
 */
uint16_t tempMax(TimeTemp arrayTimeTemp[], uint8_t len) {
    uint16_t M = 0;
    for (uint8_t i = 0; i < len; i++) {
        if (M < arrayTimeTemp[i].Temp) {
            M = arrayTimeTemp[i].Temp;
        }
    }
    return(M);
}

/*====================================================================================*
 *                                 createValues                                       *
 *====================================================================================*
 * This function creates the file in the values directory with the given name,
 * and writes in it the values of the given array.
 */
void createValues(String displayName, TimeTemp arrayTimeTemp[], uint8_t len) {
    String pathValues = valuesDir + "/valeurs" + displayName + ".CSV";
    
    char lign[10];
    sprintf (lign, "%u;%u\n", arrayTimeTemp[0].Time, (uint16_t) arrayTimeTemp[0].Temp);
    writeFile(pathValues, lign);
    for (uint8_t n = 1; n < len; n ++) {
        sprintf (lign, "%u;%u\n", arrayTimeTemp[n].Time, (uint16_t) arrayTimeTemp[n].Temp);
        appendFile(pathValues, lign);
    }
}

/*====================================================================================*
 *                                 extractValues                                      *
 *====================================================================================*
 * This function extracts the values in the given file and writes them in 
 * the given array and returns the size of the array.
 */
uint8_t extractValues(TimeTemp arrayTimeTemp[], String pathValues) {
    File file = SD.open(pathValues.c_str());
    String w = ""; // char array of the box of the csv table
    char c = ' '; // current character in the table
    uint8_t numberTime = 0;
    
    while(file.available()){
        w = ""; // reset the string w
        c = file.read();
        while (c != ';') {  // ';' separates the two columns of the csv file -> before it is the time 
            w += c;
            c = file.read();
        }
        arrayTimeTemp[numberTime].Time =  w.toInt(); // string to int

        w = ""; // reset the string w
        c = file.read();
        while (c != '\n') { // '\n' separates two ligns of the csv file -> before it is the temperature
            w += c;
            c = file.read();
        }
        arrayTimeTemp[numberTime].Temp =  w.toFloat(); // string to float

        numberTime ++;
    }
    file.close();
    return(numberTime);
}

/*====================================================================================*
 *                                  createCurve                                       *
 *====================================================================================*
 * This function creates the file in the curve directory with the given name,
 * and writes in it the values of temperature and time every minute,
 * by creating the broken lign.
 */
void createCurve(String displayName, TimeTemp arrayTimeTemp[], uint8_t len) {
    String pathCurve = curvesDir + '/' + displayName + ".CSV";
    
    // Writing on the new file the values every minute
    char lign[13];
    sprintf (lign, "%i;%.2f\n", arrayTimeTemp[0].Time, arrayTimeTemp[0].Temp);
    writeFile(pathCurve, lign);
    for (uint8_t n = 0; n < len - 1; n ++) {
        uint16_t timeL = arrayTimeTemp[n].Time;
        uint16_t timeH = arrayTimeTemp[n + 1].Time;
        float tempL = arrayTimeTemp[n].Temp;
        float tempH = arrayTimeTemp[n + 1].Temp;

        for (uint16_t t = timeL+1; t <= timeH; t ++) {
            float temp = (tempH-tempL) / (timeH-timeL) * (t-timeL) + tempL;
            sprintf (lign, "%u;%.2f\n", t, temp); // we keep the 2 digits after the dot for the temperature
            appendFile(pathCurve, lign);
        }
    }
}

/*====================================================================================*
 *                                  createAll                                         *
 *====================================================================================*
 * This function creates the two files in both the values and curve directories.
 */
void createAll(String displayName, TimeTemp arrayTimeTemp[], uint16_t len) {
    createValues(displayName, arrayTimeTemp, len);
    createCurve(displayName, arrayTimeTemp, len);
}

/*====================================================================================*
 *                                 deleteCurve                                        *
 *====================================================================================*
 * This function deletes the two files in both the values and curve directories.
 */
void deleteCurve(String nameOfCurve) {
    deleteFile(valuesDir + "/valeurs" + nameOfCurve + ".CSV");
    deleteFile(curvesDir + '/' + nameOfCurve + ".CSV");
}

/*====================================================================================*
 *                                stringInArray                                       *
 *====================================================================================*
 * This function returns true is the string is in the array.
 */
bool stringInArray(String str, String strArray[], uint8_t len) {
    bool res = false;
    for (uint8_t i = 0; i < len; i ++) {
        if (str == strArray[i]) res = true;
    }
    return(res);
}
