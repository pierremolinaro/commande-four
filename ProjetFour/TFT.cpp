// ----------Include the header----------
#include "TFT.h"

// ----------Static variables in the file----------
static TFT_eSPI tft = TFT_eSPI();

// ----------Functions----------
/*====================================================================================*
 *                                  initScreen                                        *
 *====================================================================================*
 * This function initialized the TFT screen and sets the rotation in landscape,
 * and draws the logo of Centrale Nantes.
 */
void initScreen(void) {
    tft.init();
    tft.setRotation(1);  // 0 & 2 Portrait. 1 & 3 landscape
    drawBmp("/LogoECN.bmp");
    delay(1000);
    tft.fillScreen(TFT_BLACK); // black screen
    tft.setTextSize(2);
    tft.setCursor(0, 0);
}

/*====================================================================================*
 *                                   drawBmp                                          *
 *====================================================================================*
 * This function prints on the screen a .bmp image from the SD card.
 */
void drawBmp(String filename) {
    // Open requested file on SD card
    File bmpFS = SD.open(filename.c_str());
    if (!bmpFS) {
      Serial.println("File not found");
      return;
    }
    int16_t x = 0, y = 0;
    uint32_t seekOffset;
    uint16_t w, h, row, col;
    uint8_t  r, g, b;
    uint32_t startTime = millis();
    if (read16(bmpFS) == 0x4D42) {
        read32(bmpFS);
        read32(bmpFS);
        seekOffset = read32(bmpFS);
        read32(bmpFS);
        w = read32(bmpFS);
        h = read32(bmpFS);

        if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
            y += h - 1;
            tft.setSwapBytes(true);
            bmpFS.seek(seekOffset);
            uint16_t padding = (4 - ((w * 3) & 3)) & 3;
            uint8_t lineBuffer[w * 3 + padding];
            for (row = 0; row < h; row++) {
                bmpFS.read(lineBuffer, sizeof(lineBuffer));
                uint8_t*  bptr = lineBuffer;
                uint16_t* tptr = (uint16_t*)lineBuffer;
                // Convert 24 to 16 bit colours
                for (col = 0; col < w; col++) {
                    b = *bptr++;
                    g = *bptr++;
                    r = *bptr++;
                    *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                }
                // Push the pixel row to screen, pushImage will crop the line if needed
                // y is decremented as the BMP image is drawn bottom up
                tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
            }
            Serial.print("Loaded in "); Serial.print(millis() - startTime);
            Serial.println(" ms");
        }
        else Serial.println("BMP format not recognized.");
    }
    bmpFS.close();
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
 *                                   setLign                                          *
 *====================================================================================*
 * This function sets the cursor at the given lign and at the first column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setLign(uint8_t lign, uint8_t textSize) {
    tft.setCursor(1, lign*8*textSize + 1);
}

/*====================================================================================*
 *                                  setColumn                                         *
 *====================================================================================*
 * This function sets the cursor at the same lign and at the given column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setColumn(uint8_t column, uint8_t textSize) {
    tft.setCursor(column*6*textSize + 1, tft.getCursorY());
}

// -------------------------------printPermanent----------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                printPermanent                                      *
 *====================================================================================*
 * This function prints on the screen permanent information which are  date,  time,
 * temperature, if the oven is running or a program is delayed,
 * and if so, the time left.
 */
void printPermanent(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
                      double temp, bool isRunning, uint16_t timeLeft, bool isDelayed, uint16_t timeBeforeStart) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
    // ----------Printing the date----------
    setLign(nbLign - 2);
    tft.printf("%02u/%02u/%04u", day, month, year);
    // ----------Printing the time----------
    setLign(nbLign - 1);
    tft.printf("%02u:%02u:%02u", hour, minute, second);
    // ----------Printing the temperature----------
    setLign(nbLign - 1); setColumn(nbColumn - 6);
    tft.printf("%4u%cC", (uint16_t) temp, 247); // (char)247 -> °
    // ----------Printing ON/OFF/Delayed----------
    if (isRunning) { // it is running
        tft.setTextSize(2);
        setLign(nbLign - 2); setColumn(nbColumn - 4, 1); tft.print(" ");
        setLign(nbLign - 1); setColumn(nbColumn - 4, 1); tft.print(" ");
        setLign(nbLign - 2); setColumn(nbColumn + 6, 1); tft.print("  ");
        setLign(nbLign - 1); setColumn(nbColumn + 6, 1); tft.print("  ");
        tft.setTextColor(TFT_GREEN, TFT_BLACK); tft.setTextSize(4);
        setLign(nbLign - 2); setColumn(nbColumn - 2, 1);
        tft.print("ON");
    }
    else if (!isDelayed) { // it is not running
        tft.setTextSize(2);
        setLign(nbLign - 2); setColumn(nbColumn + 8, 1); tft.print(" ");
        setLign(nbLign - 1); setColumn(nbColumn + 8, 1); tft.print(" ");
        tft.setTextColor(TFT_RED, TFT_BLACK); tft.setTextSize(4);
        setLign(nbLign - 2); setColumn(nbColumn - 4, 1);
        tft.print("OFF");
    }
    else { // it is delayed
        setLign(nbLign*2 - 4, 1); setColumn(nbColumn - 4, 1); tft.setTextSize(1); tft.print("            ");
        setLign(nbLign*2 - 1, 1); setColumn(nbColumn - 4, 1); tft.setTextSize(1); tft.print("            ");
        tft.setTextColor(TFT_ORANGE, TFT_BLACK); tft.setTextSize(2);
        setLign(nbLign*2 - 3, 1); setColumn(nbColumn - 4, 1);
        tft.printf("Diff%cr%c", 130, 130); // (char)130 -> é
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);

    // ----------Printing time left if a program is running----------
    if (isRunning) {
        setLign(nbLign*2 - 5, 1); setColumn(nbColumn*2 - 12, 1); tft.setTextSize(1);
        tft.print("    restant:");
        setLign(nbLign - 2); setColumn(nbColumn - 8); tft.setTextSize(2);
        tft.printf("%3uh%02umn", timeLeft/60, timeLeft%60);
    }
    // ----------Printing delay left if a program is delayed----------
    else if (isDelayed) {
        setLign(nbLign*2 - 5, 1); setColumn(nbColumn*2 - 12, 1); tft.setTextSize(1);
        tft.printf("avant d%cbut:", 130); // (char)130 -> é
        setLign(nbLign - 2); setColumn(nbColumn - 8); tft.setTextSize(2);
        tft.printf("%3uh%02umn", timeBeforeStart/60, timeBeforeStart%60);
    }
}

/*====================================================================================*
 *                             clearPrintPermanent                                    *
 *====================================================================================*
 * This function clears the bottom of the screen where are printed the permanent
 * information.
 */
void clearPrintPermanent(void) {
    setLign(nbLign - 2); setColumn(nbColumn - 8); tft.setTextSize(2);
    tft.fillRect(tft.getCursorX()-1, tft.getCursorY()-1, 8*2*6, 1*2*8, TFT_BLACK);
    setLign(nbLign*2 - 5, 1); setColumn(nbColumn*2 - 12, 1);
    tft.fillRect(tft.getCursorX()-1, tft.getCursorY()-1, 12*1*6, 1*1*8, TFT_BLACK);
}

// --------------------------------printMainMenu----------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                 printMainMenu                                      *
 *====================================================================================*
 * This function prints on the screen the Main Menu to select the next menu between
 * starting menu, showing information menu, setting time menu and curve managing menu.
 * If the oven has a delayed start, the starting menu turns into a menu to change the
 * value of the delay.
 * If the oven is running, the starting menu turns into a menu to stop the oven.
 */
void printMainMenu(uint16_t encoderPos, bool isRunning, bool isDelayed) {
    tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // ----------Start or Stop or Change Delay----------
    setLign(0, 3);
    if (isRunning) {
        if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.printf(" Arr%cter Four ", 136); // (char)136 -> ê
    } else if (isDelayed) {
        if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.printf(" Changer Prog. ");
    } else {
        if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.printf(" D%cmarrer Four ", 130); // (char)130 -> é
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // ----------Show Information----------
    setLign(2, 3);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Afficher Infos ");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // ----------Set Time----------
    setLign(4, 3);
    if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" R%cgler Heure ", 130); // (char)130 -> é
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // ----------Manage Curves----------
    setLign(6, 3);
    if (encoderPos == 3) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" G%crer Programmes", 130); // (char)130 -> é
    tft.setTextSize(1);
    setLign(18, 1); setColumn(17, 3); tft.print(' ');
    setLign(19, 1); setColumn(17, 3); tft.print(' ');
    setLign(20, 1); setColumn(17, 3); tft.print(' ');
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// ----------------------------printSelectCurveMenu-------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                             printSelectCurveMenu                                   *
 *====================================================================================*
 * This function prints on the screen the menu to select the curve between those on
 * the SD card.
 */
void printSelectCurveMenu(uint16_t encoderPos, uint8_t nbCurves, String arrayDisplayNames[], uint8_t numPage) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2);
    if (numPage > 0 && nbCurves <= 6*(numPage+1)) {
        if (encoderPos == nbCurves - 6*numPage + 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    else {
        if (encoderPos == min(7, nbCurves)) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    tft.print("Retour");

    // ----------Select the curve----------
    for (uint8_t numeroOfCurve = 0; numeroOfCurve < min(6, nbCurves-6*numPage); numeroOfCurve ++) {
        setLign(3*numeroOfCurve+3, 1); setColumn(2); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        String displayName = arrayDisplayNames[numeroOfCurve + 6*numPage];
        if (encoderPos == numeroOfCurve) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.print(' ' + displayName + ' ');
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // ----------Next page----------
    if (nbCurves - 6*numPage > 6) {
        setLign(21, 1); setColumn(18); tft.setTextSize(2);
        if (encoderPos == 6) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.print(" Suivant");
    }
    // ----------First page----------
    else if (numPage > 0) {
        setLign(21, 1); setColumn(12); tft.setTextSize(2);
        if (encoderPos == nbCurves - 6*numPage) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.printf(" Premi%cre page", 138); // (char)138 -> è
    }
}

// -----------------------------printShowValuesMenu-------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                  setCursorTab                                      *
 *====================================================================================*
 * This function set the cursor in order to print a table of time and temperature
 * of 22 columns maximum.
 */
void setCursorTab(uint8_t numTime, bool TimeOrTemp) { // TimeOrTemp : 0 -> time, 1 -> temp
    uint8_t lign   = (numTime+3)/5;
    uint8_t column = (numTime+3)%5;
    setLign(5+lign*2*2 + TimeOrTemp*2, 1);
    setColumn(2+column*5*2, 1);
}

/*====================================================================================*
 *                                printTabTimeTemp                                    *
 *====================================================================================*
 * This function prints the values of a table of time and temperature
 * of 22 columns maximum.
 */
void printTabTimeTemp(TimeTemp arrayTimeTemp[], uint8_t len) {
    // ----------Print the values----------
    tft.setTextSize(2);
    for (uint8_t i = 0; i < len; i ++) {
        tft.setTextColor(TFT_YELLOW); setCursorTab(i, 0); tft.printf("%4u", arrayTimeTemp[i].Time);
        tft.setTextColor(TFT_PINK); setCursorTab(i, 1); tft.printf("%4u", (uint16_t) arrayTimeTemp[i].Temp);
    }
}

/*====================================================================================*
 *                                     drawTab                                        *
 *====================================================================================*
 * This function prints the ligns of a table of time and temperature of 22 columns.
 */
void drawTab(void) {
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE);
    uint32_t smallTraitsColor = TFT_NAVY;
    uint32_t largeTraitsColor = TFT_LIGHTGREY;

    // ----------Write the name of the table ligns----------
    setLign( 5, 1); setColumn( 1, 1); tft.print ("Temps(min)");
    setLign( 7, 1); setColumn( 1, 1); tft.printf("Temp%crature(%cC)", 130, 247); // (char) 130 -> é, (char) 247 -> °
    // ----------Draw the small ligns----------
    setLign( 7, 1); setColumn( 1, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, smallTraitsColor);
    setLign(11, 1); setColumn( 1, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, smallTraitsColor);
    setLign(23, 1); setColumn( 1, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  1*2*8, smallTraitsColor);
    setLign( 9, 1); setColumn(11, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1,  5*2*6,  8*2*8, smallTraitsColor);
    setLign( 5, 1); setColumn(31, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1,  5*2*6, 10*2*8, smallTraitsColor);
    // ----------Draw the large ligns of the table----------
    setLign( 5, 1); setColumn( 1, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, largeTraitsColor);
    setLign( 9, 1); setColumn( 1, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  6*2*8, largeTraitsColor);
    setLign(17, 1); setColumn( 1, 1); tft.drawRect(tft.getCursorX()-1, tft.getCursorY()-1, 25*2*6,  4*2*8, largeTraitsColor);
    setLign( 5, 1); setColumn(31, 1); tft.drawFastVLine(tft.getCursorX()-1, tft.getCursorY()-1, 2*2*8, largeTraitsColor);
}

/*====================================================================================*
 *                              printShowValuesMenu                                   *
 *====================================================================================*
 * This function prints the menu showing the table of values of the selected curve.
 */
void printShowValuesMenu(uint16_t encoderPos) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");

    // ----------Next----------
    setLign(0); setColumn(7); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Suivant ");
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/*====================================================================================*
 *                                   printValues                                      *
 *====================================================================================*
 * This function prints the table of values of the selected curve.
 */
void printValues(String nameOfCurve) {
    // ----------Print the values of the table----------
    TimeTemp arrayTimeTemp[22];
    String pathValues = valuesDir + "/valeurs" + nameOfCurve + ".CSV";
    uint8_t len = extractValues(arrayTimeTemp, pathValues);
    printTabTimeTemp(arrayTimeTemp, len);

    // ----------Print all the ligns of the table----------
    drawTab();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// ------------------------------printPlotGraphMenu-------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                   printCurve                                       *
 *====================================================================================*
 * This function prints the curve of temperature in function of time.
 */
void printCurve(TimeTemp arrayTimeTemp[], uint8_t len) {
    uint16_t tmax = arrayTimeTemp[len-1].Time;
    uint16_t tempmax = tempMax(arrayTimeTemp, len);
    uint16_t w = 298, h = 160;
    tft.setCursor(16, 32);
    tft.drawRect(tft.getCursorX(), tft.getCursorY(), w, h, TFT_WHITE);

    uint16_t x0 = tft.getCursorX() + arrayTimeTemp[0].Time/tmax*w;
    uint16_t y0 = tft.getCursorY() + h - arrayTimeTemp[0].Temp/tempmax*h;
    for (uint8_t i = 1; i < len; i++) {
        uint16_t x1 = tft.getCursorX() + ((double)arrayTimeTemp[i].Time/tmax)*w;
        uint16_t y1 = tft.getCursorY() + h - ((double)arrayTimeTemp[i].Temp/tempmax)*h;
        tft.drawLine(x0, y0, x1, y1, TFT_YELLOW);
        x0 = x1; y0 = y1;
    }
    tft.setTextColor(TFT_YELLOW, TFT_BLACK); tft.setTextSize(1);
    tft.setCursor( 12, 192); tft.print("0");
    tft.setCursor(  0,  24); tft.printf("%4u%cC", tempmax, 247); // (char)247 -> °
    tft.setCursor(280, 192); tft.printf("%4umn", tmax);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);
}

/*====================================================================================*
 *                               printPlotGraphMenu                                   *
 *====================================================================================*
 * This function prints the menu plotting the graph of the selected curve.
 */
void printPlotGraphMenu(uint16_t encoderPos) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    // ----------Validate----------
    setLign(0); setColumn(7); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Valider ");
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/*====================================================================================*
 *                                   printGraph                                       *
 *====================================================================================*
 * This function prints the graph of the selected curve.
 */
void printGraph(String nameOfCurve) {
    // ----------Plot the graph----------
    TimeTemp arrayTimeTemp[22];
    String pathValues = valuesDir + "/valeurs" + nameOfCurve + ".CSV";
    uint8_t len = extractValues(arrayTimeTemp, pathValues);
    printCurve(arrayTimeTemp, len);
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// ----------------------------printStartOrDelayMenu------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                             printStartOrDelayMenu                                  *
 *====================================================================================*
 * This function prints the menu to select whether we start directly the program or
 * we delay it.
 */
void printStartOrDelayMenu(uint16_t encoderPos, uint16_t tmax, uint8_t hour, uint8_t minute) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");

    // ----------Start cooking----------
    setLign(2, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Lancer Cuisson ");

    // ----------Delayed departure----------
    setLign(4, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" D%cpart Diff%cr%c ", 130, 130, 130); // (char)130 -> é

    // ----------Print end date----------
    uint16_t endM = minute + tmax;
    uint16_t endH = hour;
    uint16_t endJ = 0;
    while (endM >= 60) {
        endH += 1;
        endM -= 60;
    }
    while (endH >= 24) {
        endJ += 1;
        endH -= 24;
    }
    setLign(19, 1); setColumn(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Heure de fin:");
    setLign(18, 1); setColumn(14);
    if (endJ == 1) {
        tft.print("demain      ");
        setLign(20, 1); setColumn(14);
    }
    else if (endJ >= 2) {
        tft.printf("dans %1u jours", endJ);
        setLign(20, 1); setColumn(14);
    }
    else { // endJ == 0
        tft.setTextSize(1); tft.print("                        ");
        setLign(21, 1); setColumn(14); tft.setTextSize(1); tft.print("                  ");
        setLign(19, 1); setColumn(14); tft.setTextSize(2);
    }
    tft.printf("%c %02uh%02umn", 133, endH, endM); // (char)133 -> à
}

// --------------------------------printDelayMenu---------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                 printDelayMenu                                     *
 *====================================================================================*
 * This function prints the menu to select the value of the delay of the program.
 */
void printDelayMenu(uint16_t launchDelay, uint16_t tmax, uint8_t hour, uint8_t minute) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Retour");

    // ----------Start Cooking----------
    setLign(2, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print(" Lancer Cuisson ");

    // ----------Delayed Departure----------
    setLign(4, 3); tft.setTextSize(3); tft.setTextColor(TFT_BLACK, TFT_ORANGE);
    tft.printf(" D%cpart Diff%cr%c ", 130, 130, 130); // (char)130 -> é

    // ----------Selecting Delay----------
    uint16_t h = launchDelay / 60;
    uint16_t m = launchDelay - 60*h;
    setLign(5, 3); setColumn(4, 3); tft.setTextSize(3); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    if (h > 0) tft.printf("%2uh", h);
    else { tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.print("   "); }
    tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%02umn", m);

    // ----------Print end date----------
    uint16_t endM = minute + m + tmax;
    uint16_t endH = hour + h;
    uint16_t endJ = 0;
    while (endM >= 60) {
        endH += 1;
        endM -= 60;
    }
    while (endH >= 24) {
        endJ += 1;
        endH -= 24;
    }
    setLign(19, 1); setColumn(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Heure de fin:");
    setLign(18, 1); setColumn(14);
    if (endJ == 1) {
        tft.print("demain      ");
        setLign(20, 1); setColumn(14);
    }
    else if (endJ >= 2) {
        tft.printf("dans %1u jours", endJ);
        setLign(20, 1); setColumn(14);
    }
    else { // endJ == 0
        tft.setTextSize(1); tft.print("                        ");
        setLign(21, 1); setColumn(14); tft.setTextSize(1); tft.print("                  ");
        setLign(19, 1); setColumn(14); tft.setTextSize(2);
    }
    tft.printf("%c %02uh%02umn", 133, endH, endM); // (char)133 -> à
}

// --------------------------------printStopMenu----------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                 printStopMenu                                      *
 *====================================================================================*
 * This function prints the menu to select whether we stop the running program.
 */
void printStopMenu(uint16_t encoderPos) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");

    // ----------Stop----------
    setLign(3, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" Arr%cter Cuisson ", 136); // (char)136 -> ê

    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// ------------------------------printChangeDelayMenu-----------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                               printChangeDelayMenu                                 *
 *====================================================================================*
 * This function prints the menu to select the new value of the delay of the program.
 */
void printChangeDelayMenu(uint16_t launchDelay, uint16_t tmax, uint8_t hour, uint8_t minute) {
    // ----------Delayed Departure----------
    setLign(2, 3); tft.setTextSize(3); tft.setTextColor(TFT_BLACK, TFT_ORANGE);
    tft.printf(" D%cpart Diff%cr%c ", 130, 130, 130); // (char)130 -> é

    // ----------Selecting Delay----------
    uint16_t h = launchDelay / 60;
    uint16_t m = launchDelay - 60*h;
    setLign(4, 3); setColumn(4, 3); tft.setTextSize(3); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    if (h > 0) tft.printf("%2uh", h);
    else { tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.print("   "); }
    tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%02umn", m);

    // ----------Print end date----------
    uint16_t endM = minute + m + tmax;
    uint16_t endH = hour + h;
    uint16_t endJ = 0;
    while (endM >= 60) {
        endH += 1;
        endM -= 60;
    }
    while (endH >= 24) {
        endJ += 1;
        endH -= 24;
    }
    setLign(16, 1); setColumn(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Heure de fin:");
    setLign(15, 1); setColumn(14);
    if (endJ == 1) {
        tft.print("demain      ");
        setLign(17, 1); setColumn(14);
    }
    else if (endJ >= 2) {
        tft.printf("dans %1u jours", endJ);
        setLign(17, 1); setColumn(14);
    }
    else { // endJ == 0
        tft.setTextSize(1); tft.print("                        ");
        setLign(18, 1); setColumn(14); tft.setTextSize(1); tft.print("                  ");
        setLign(16, 1); setColumn(14); tft.setTextSize(2);
    }
    tft.printf("%c %02uh%02umn", 133, endH, endM); // (char)133 -> à
}

// --------------------------------printInfoMenu----------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                 printInfoMenu                                      *
 *====================================================================================*
 * This function prints the temperature, the command of temperature, the state of the
 * relay and the time left before the end of the running program.
 */
void printInfoMenu(double temp, double command, bool isRunning, uint16_t timeLeft, bool increaseTemp) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    // ----------Temperature----------
    setLign(3); setColumn(1); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("Temp%crature : %7.2f%cC", 130, temp, 247); // (char)130 -> é, (char)247 -> °
    // ----------Time left----------
    setLign(5); setColumn(1); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (isRunning) tft.printf("Temps restant : %3uh%02umn", timeLeft/60, timeLeft%60);
    // ----------Command----------
    setLign(7); setColumn(1);
    if (isRunning) tft.printf("Consigne : %6.2f%cC", command, 247); // (char)247 -> °
    // ----------Relay ON/OFF----------
    setLign(9); setColumn(1);
    if (isRunning) {
        tft.print("Relais : ");
        if (increaseTemp) {
            tft.setTextColor(TFT_GREEN, TFT_BLACK); tft.print("ON ");
        }
        else {
            tft.setTextColor(TFT_RED, TFT_BLACK); tft.print("OFF");
        }
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
}

// ------------------------------printSetTimeMenus--------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                               printSetTimeMenu                                     *
 *====================================================================================*
 * This function prints the menu to select whether to change the date or the time.
 */
void printSetTimeMenu(uint16_t encoderPos, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {
    // ----------Return----------
    setLign(0, 2); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");

    // ----------Time----------
    setLign(2, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" %02u:%02u ", hour, minute);

    // ----------Date----------
    setLign(4, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" %02u/%02u/%04u ", day, month, year);
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/*====================================================================================*
 *                              printSetTimeSubMenu                                   *
 *====================================================================================*
 * This function prints the menu to change the date or the time.
 */
void printSetTimeSubMenu(uint16_t Mode, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Retour");

    setLign(2, 3); setColumn(1, 3); tft.setTextSize(3);
    // ----------Hour----------
    if (Mode == 30) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%02u", hour);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.print(':');

    // ----------Minute----------
    if (Mode == 31) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%02u", minute);
     tft.setTextColor(TFT_WHITE, TFT_BLACK);

    setLign(4, 3); setColumn(1, 3);
    // ----------Day----------
    if (Mode == 34) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%02u", day);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.print('/');

    // ----------Month----------
    if (Mode == 33) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%02u", month);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.print('/');

    // ----------Year----------
    if (Mode == 32) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf("%04u", year);
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// ----------------------------printManageCurvesMenu------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                             printManageCurvesMenu                                  *
 *====================================================================================*
 * This function prints the menu where we select what to do concerning the curves
 * stocked on the SD card: show onformation on them, create a new curve or delete one.
 */
void printManageCurvesMenu(uint16_t encoderPos) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 3) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");

    // ----------Information about curves----------
    tft.setTextSize(3); setLign(2, 2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    setLign(2, 2);
    tft.print(" Infos Programmes");
    tft.setTextSize(1);
    setLign(4, 1); setColumn(17, 3); tft.print(' ');
    setLign(5, 1); setColumn(17, 3); tft.print(' ');
    setLign(6, 1); setColumn(17, 3); tft.print(' ');

    // ----------Create a new curve----------
    tft.setTextSize(3); setLign(5, 2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Nouv. Programme");
    tft.setTextSize(1);
    setLign(10, 1); setColumn(16, 3); tft.print(' ');
    setLign(11, 1); setColumn(16, 3); tft.print(' ');
    setLign(12, 1); setColumn(16, 3); tft.print(' ');

    // ----------Delete a curve----------
    tft.setTextSize(3); setLign(8, 2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Suppr. Programme");
    tft.setTextSize(1);
    setLign(16, 1); setColumn(17, 3); tft.print(' ');
    setLign(17, 1); setColumn(17, 3); tft.print(' ');
    setLign(18, 1); setColumn(17, 3); tft.print(' ');
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/*====================================================================================*
 *                            printMaxNbCurvesErrorMenu                               *
 *====================================================================================*
 * This function prints the menu when we have reached the maximal number of curves
 * on the SD card, and we want to create a new one.
 */
void printMaxNbCurvesErrorMenu(void) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    // ----------Name of the curve----------
    setLign(4, 1); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("Il y a d%cj%c %u programmes", 130, 133, maxnbCurves); // (char)130 -> é, (char)133 -> à
    setLign(7, 1);
    tft.print("sur la carte SD.");
    setLign(11, 1);
    tft.print("Vous avez atteint le max.");
    setLign(15, 1);
    tft.printf("Vous ne pouvez pas cr%cer", 130); // (char)130 -> é
    setLign(18, 1);
    tft.print("un nouveau programme.");
}

// -----------------------------printChooseNameMenu-------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                              printChooseNameMenu                                   *
 *====================================================================================*
 * This function prints the menu where we select the name of the new curve we want
 * to create.
 */
void printChooseNameMenu(uint16_t encoderPos, String displayName, uint8_t MAJminOth) {
    // ----------Curve Name----------
    setLign(0, 3); setColumn(1, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Nom du programme");
    setLign(2, 2); setColumn(nbColumn-maxlength, 1); tft.setTextSize(2);
    for (uint8_t i = 0; i < displayName.length(); i ++) {
        tft.print(displayName[i]);
    }
    if (displayName.length() < maxlength) { // if we haven't already finish the word
        tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.print('_');
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    for (uint8_t i = displayName.length() + 1; i < maxlength; i ++) {
        tft.print('_');
    }

    // ----------MAJ or min----------
    if (MAJminOth < 2) {
        // ----------Print the chars----------
        for (uint8_t i = 0; i < 26; i ++) {
            setLign(4 + 2*(uint8_t)(i/7)); tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if (i<21) setColumn(2 + 2*(i%7));
            else setColumn(4 + 2*(i%7));
            if (displayName.length() == maxlength) {
                tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
            } else if (encoderPos == i) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
            }
            if (MAJminOth == 0) { // MAJ
                tft.printf("%c", 65 + i); // (char)65 -> A - (char)90 -> Z
            } else { // min
                tft.printf("%c", 97 + i); // (char)97 -> a - (char)122 -> z
            }
        }
        // ----------Print the next menu of chars----------
        if (MAJminOth == 0) { // MAJ
            // The next menu of chars is min
            setLign(5); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if (displayName.length() == maxlength) {
                tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
            } else if (encoderPos == 26) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
            }
            tft.print("min");
        } else { // min
            // The next menu of chars is Others
            setLign(5); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if (displayName.length() == maxlength) {
                tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
            } else if (encoderPos == 26) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
            }
            tft.print("Others");
        }
        // ----------Print the delete button----------
        setLign(7); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == 0) {
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        } else if (displayName.length() == maxlength && encoderPos == 2) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (encoderPos == 27) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("Eff.");
        // ----------Print the validate button----------
        setLign(9); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == 0) {
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        } else if (displayName.length() == maxlength && encoderPos == 0) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (encoderPos == 28) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("OK");
        // ----------Print the cancel button----------
        setLign(11); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == 0 && encoderPos == 27) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (displayName.length() == maxlength && encoderPos == 1) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (encoderPos == 29) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("Annuler");
    }
    // ----------Others----------
    else {
        // ----------Print the chars----------
        for (uint8_t i = 0; i < 11; i ++) {
            setLign(5 + 2*(uint8_t)(i/7)); tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if (i<7) setColumn(2 + 2*(i%7));
            else setColumn(5 + 2*(i%7));
            if (displayName.length() == maxlength) {
                tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
            } else if (encoderPos == i) {
                tft.setTextColor(TFT_BLACK, TFT_YELLOW);
            }
            if (i < 10) {
                tft.printf("%c", 48 + i); // (char)48 -> 0 - (char)57 -> 9
            } else {
                tft.printf("%c", 95); // (char)95 -> _
            }
        }
        // ----------Print the next menu of chars: MAJ----------
        setLign(5); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == maxlength) {
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        } else if (encoderPos == 11) { // we change from MAJ to min, min to Other, Other to MAJ
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("MAJ");
        // ----------Print the delete button----------
        setLign(7); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == 0) {
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        } else if (displayName.length() == maxlength && encoderPos == 2) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (encoderPos == maxlength) { // delete the last character
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("Eff.");
        // ----------Print the validate button----------
        setLign(9); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == 0) {
            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        } else if (displayName.length() == maxlength && encoderPos == 0) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (encoderPos == 13) { // Validate
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("OK");
        // ----------Print the cancel button----------
        setLign(11); setColumn(19); tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (displayName.length() == 0 && encoderPos == maxlength) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (displayName.length() == maxlength && encoderPos == 1) {
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else if (encoderPos == 14) { // Cancel
            tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        }
        tft.print("Annuler");
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/*====================================================================================*
 *                              printNameContinueMenu                                 *
 *====================================================================================*
 * The name of the curve is already given.
 * This function prints the menu where we choose between continue with this name
 * or return back to take another name.
 */
void printNameContinueMenu(uint16_t encoderPos, String displayName) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    // ----------Name of the curve----------
    setLign(2, 2); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("   Voulez-vous vraiment \n remplacer ce programme ?");
    setLign(5, 2); setColumn(nbColumn - displayName.length(), 1);
    tft.print(displayName);
    // ----------Replace the curve----------
    setLign(8, 2); setColumn(6, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Oui ");
}

// ------------------------------printSelectValuesMenu----------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                            clearPrintSelectValuesMenu                              *
 *====================================================================================*
 * This function clears the top of the screen when we select values of time or
 * temperature.
 */
void clearPrintSelectValuesMenu(void) {
    tft.fillRect(0, 0, tft.width(), 3*2*6, TFT_BLACK);
}

/*====================================================================================*
 *                               printSelectValuesMenu                                *
 *====================================================================================*
 * This function prints the menu where we select the values of the new curve, to
 * select whether we create a new entry, delete one, validate or cancel.
 */
void printSelectValuesMenu(uint16_t encoderPos, TimeTemp arrayTimeTemp[], uint8_t len) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (len < 2) {
        if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    } else if (len < 22 && arrayTimeTemp[len-1].Time < 1000) {
        if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    } else {
        if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    tft.print("Retour");
    // ----------Delete a column----------
    setLign(0); setColumn(6, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (len < 2) {
        if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    } else if (len < 22 && arrayTimeTemp[len-1].Time < 1000) {
        if (encoderPos == 3) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    } else {
        if (encoderPos == 2) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    //tft.setCursor(tft.getCursorX(), tft.getCursorY()-2);
    tft.print('x');
    // ----------Create a new column----------
    setLign(0); setColumn(8, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (len == 22 || arrayTimeTemp[len-1].Time >= 1000) {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    } else {
        if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    tft.print('+');
    // ----------Validate----------
    setLign(0); setColumn(10, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (len < 2) {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    } else if (len == 22 || arrayTimeTemp[len-1].Time >= 1000) {
        if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    } else {
        if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    }
    tft.print("Valider");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

/*====================================================================================*
 *                                   printValues                                      *
 *====================================================================================*
 * This function prints the table of values of the curve we are creating.
 */
void printValues(TimeTemp arrayTimeTemp[], uint8_t len) {
    // ----------Print the values of the table----------
    printTabTimeTemp(arrayTimeTemp, len);
    // ----------Print all the ligns of the table----------
    drawTab();
}

/*====================================================================================*
 *                              printSelectTimeTempMenu                               *
 *====================================================================================*
 * This function prints the menu where we select the values of the new curve, to
 * show the value we are modifying (time or temperature).
 */
void printSelectTimeTempMenu(TimeTemp arrayTimeTemp[], uint8_t len, uint16_t Mode) {
    // ----------Print the values----------
    tft.setTextSize(2);
    if (Mode == 440) { // we change the time
        setCursorTab(len - 1, 0); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
        tft.printf("%4u", arrayTimeTemp[len - 1].Time);
    } else { // we change the temperature
        setCursorTab(len - 1, 0); tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.printf("%4u", arrayTimeTemp[len - 1].Time);
        setCursorTab(len - 1, 1); tft.setTextColor(TFT_BLACK, TFT_PINK);
        tft.printf("%4u", (uint16_t) arrayTimeTemp[len - 1].Temp);
    }
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// -------------------------------printPlotGraphMenu------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                printPlotGraphMenu                                  *
 *====================================================================================*
 * This function prints the menu where we show the graph we just have created.
 */
void printPlotGraphMenu(uint16_t encoderPos, TimeTemp arrayTimeTemp[], uint8_t len) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    // ----------Create curve----------
    setLign(0); setColumn(5, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.printf(" Cr%cer Prog.", 130); // (char)130 -> é
    tft.setTextSize(1);
    setLign(0, 1); setColumn(17, 3); tft.print(' ');
    setLign(1, 1); setColumn(17, 3); tft.print(' ');
    setLign(2, 1); setColumn(17, 3); tft.print(' ');
    tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // ----------Plot the graph----------
    printCurve(arrayTimeTemp, len);
}

// -------------------------------printCreationMenu-------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                printCreationMenu                                   *
 *====================================================================================*
 * This function prints the menu where we wait for the files to be writen.
 */
void printCreationMenu(void) {
    tft.setTextSize(3);
    setLign(2, 1); setColumn(14, 1);
    tft.printf("Cr%cation", 130); // (char)130 -> é
    setLign(6, 1); setColumn(23, 1);
    tft.print("du");
    setLign(10, 1); setColumn(13, 1);
    tft.print("programme");
    setLign(14, 1); setColumn(11, 1);
    tft.print("de cuisson");
    setLign(18, 1); setColumn(21, 1);
    tft.print("...");
}

// -----------------------------printDeleteCurveMenu------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                              printDeleteCurveMenu                                  *
 *====================================================================================*
 * This function prints the menu where we confirm we want to delete the selected curve.
 */
void printDeleteCurveMenu(uint16_t encoderPos, String nameOfCurve) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 0) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    // ----------Name of the curve----------
    setLign(2, 2); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("   Voulez-vous vraiment \n supprimer ce programme ?");
    setLign(5, 2); setColumn(nbColumn - nameOfCurve.length(), 1);
    tft.print(nameOfCurve);
    // ----------Delete the curve----------
    setLign(8, 2); setColumn(6, 3); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (encoderPos == 1) tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" Oui ");
}

/*====================================================================================*
 *                               printNameErrorMenu                                   *
 *====================================================================================*
 * This function prints the menu when the chosen name of file to delete or create
 * is the one which is running.
 */
void printNameErrorMenu(String nameOfCurve, uint16_t Mode) {
    // ----------Return----------
    setLign(0); tft.setTextSize(2); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print("Retour");
    // ----------Name of the curve----------
    setLign(4, 1); tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.printf("      Ce programme :");
    setLign(7, 1); setColumn(nbColumn - nameOfCurve.length(), 1);
    tft.print(nameOfCurve);
    setLign(10, 1);
    tft.print("est en cours d'utilisation.");
    setLign(16, 1); setColumn(2); tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.print("Vous ne pouvez");
    setLign(20, 1); setColumn(1);
    if (Mode == 431) tft.print("pas le remplacer.");
    else             tft.print("pas le supprimer.");
}

// -------------------------------printEndCycleMenu-------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                printEndCycleMenu                                   *
 *====================================================================================*
 * This function prints the menu when the cycle has finished, when the buzzer is
 * running.
 */
void printEndCycleMenu(void) {
    // ----------Cycle finished----------
    tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    setLign(1, 3); setColumn(1, 2);
    tft.print("Cycle de cuisson");
    setLign(3, 3); setColumn(5, 3);
    tft.printf("termin%c", 130); // (char)130 -> é
    // ----------OK----------
    tft.setTextSize(2);
    setLign(5, 3); setColumn(11, 2); tft.setTextColor(TFT_BLACK, TFT_YELLOW);
    tft.print(" OK ");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   GERER ECONOMISEUR ECRAN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

//static uint32_t gDateExtinction = 1000 ; // 20s après le démarrage

//······················································································································

void gererEconomiseurEcran (void) {

}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
