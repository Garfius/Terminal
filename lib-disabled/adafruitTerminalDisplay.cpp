//----------------------------------------variables------------------------------------------- al passar a classes moure declaraciío classe amb les seves amigues ara internes
#include <adafruitTerminalDisplay_beta.h>

#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

Adafruit_SH1106 myDisplay(7, 8, 6); //Adafruit_SH1106(int8_t DC, int8_t RST, int8_t CS);

//-----------------------------------------------classes, si es necessiten dues classes depenents s'han de mencionar a la capçalera
class adafruitTerminalDisplay {
public:
  static const int ALCADA = 8;
  static const int AMPLADA = 18;
  char charArray[AMPLADA][ALCADA];
  bool escaped;

  byte currPosX;
  byte currPosY;

  bool cursorState, saved, displayed, romEnabled;
  unsigned long lastCursorBlink;
  unsigned long lastChange;

  Adafruit_SH1106* pantalla;
  char blank = ' ';
  int saveEepromStartIndex;
  adafruitTerminalDisplay(Adafruit_SH1106* pantallaExt) {
    this->pantalla = pantallaExt;
    /*
    pantalla->width() 128
    pantalla->height() 64
    llavors mida caràcter = 7 alçada i 7 amplada, es fa +1 per separar 
    */
    

  }
  void init(bool useRom = true) {
    // starts the tarminal
    pantalla->begin(SH1106_SWITCHCAPVCC);
    pantalla->clearDisplay();
    pantalla->setCursor(0, 0);
    pantalla->setTextWrap(true);
    pantalla->setTextColor(WHITE);
    pantalla->setTextSize(FONT_SIZE);
    pantalla->println("Greets from: ");
    pantalla->println("garfius@gmail.com");
    pantalla->display();
    delay(cursorBlinkTime * 2);
    netejaArray();
    if (romEnabled)loadState();
    displayArray();
    cursorState = false;
    saved = true;
    lastCursorBlink = millis();
    lastChange = 0;
    saveEepromStartIndex = 0;
    this->romEnabled = useRom;
    escaped = true;
  }
  void scrollArray() {
    // move all up 1 line, discard top, bottom blank
    for (int y = 0; y < (ALCADA - 1); y++) {
      for (int x = 0; x < AMPLADA; x++)
      {
        charArray[x][y] = charArray[x][y + 1];
      }
    }
    netejaLinia(ALCADA - 1);
  }
  void crlf() {
    // crlf cursor
    currPosX = 0;
    currPosY += 1;
    displayed = false;
  }
  byte lastFilledChar() {
    // last non blank character of the line
    for (int x = (AMPLADA - 1); x >= 0; x--)
    {
      if (charArray[x][currPosY] != blank) return x;
    }
    return 0;
  }
  void doBack(bool erase) {
    // backspace cursor
    if (currPosX > 0) {
      currPosX -= 1;
      if(erase)charArray[currPosX][currPosY] = this->blank;
      displayed = false;
    }
    else if (currPosY > 0) {
      currPosY -= 1;
      currPosX = lastFilledChar();
      if ((currPosX < (AMPLADA - 1)) && (currPosX > 0)) {
        currPosX += 1;
      }
      else {
        if(erase)charArray[currPosX][currPosY] = this->blank;
      }
      displayed = false;
    }
  }
  void escriuCaracter(char caracter) {
    // es fa scroll quan s'ha d'escriure fora de la pantalla despres d'un posicionament
    Serial.print(caracter,HEX);
    if(escaped){
      if(caracter == 0x5B)return;
      //Serial.print("*");
        // fletxes 41 amunt,42 avall,43 dre,44 esq
        switch(caracter) {
          case 0x41:
            break;
          case 0x42:
            // code block
            break;
          case 0x43:
            // code block
            break;
          case 0x44:
            if(currPosX !=0)doBack(false);;
            break;
        }
    }
    
    escaped = (caracter == 0x1B);
    
    if(escaped){
      //Serial.print("^");
      return;// detecta escape
    }  

    if ((caracter == 0x0D) || (caracter == 0x0A)) {// cr || lf  
      crlf();
    }

    else if ((caracter == 0x08) || (caracter == 0x7F)) {//backspace
      doBack(true);
    }
    else {
      charArray[currPosX][currPosY] = caracter;
      if (!escaped)currPosX += 1;
      displayed = saved = false;
      this->lastChange = millis();
    }
    if (currPosX >= AMPLADA) {
      crlf();
    }
    if (currPosY >= ALCADA) {
      currPosY = ALCADA - 1;
      scrollArray();
    }
  }

  void netejaArray() {
    // assign blank to each character, and set cursor to 0,0
    for (int y = 0; y < ALCADA; y++) {
      netejaLinia(y);
    }
    currPosX = 0;
    currPosY = 0;
  }
  void netejaLinia(int y) {
    // assign blank to each character of the line
    for (int x = 0; x < AMPLADA; x++)
    {
      charArray[x][y] = this->blank;
    }
  }
  void displayArray() {
    // show terminal array on screen
    pantalla->clearDisplay();
    for (int y = 0; y < ALCADA; y++) {
      for (int x = 0; x < AMPLADA; x++)
      {
        pantalla->drawChar(x*fontX, y*fontY, charArray[x][y], WHITE, BLACK, FONT_SIZE);
      }
    }
    pantalla->display();
    displayed = true;
  }
  void cursorBlink(bool display = true) {
    // draw or erase cursor
    this->pantalla->drawFastHLine((int16_t)this->currPosX * fontX, (((int16_t)this->currPosY + 1) * fontY) - 1, fontX - 2, (cursorState) ? WHITE : BLACK);
    cursorState = !cursorState;
    if (display)this->pantalla->display();
  }
  void cursorBlinkIfNeeded() {
    // check internal timer and calls cursor blink if needed
    if (millis() > (lastCursorBlink + cursorBlinkTime)) {
      cursorBlink();
      lastCursorBlink = millis();
    }
    else if ((lastCursorBlink + cursorBlinkTime) < lastCursorBlink) {
      cursorBlink();
      lastCursorBlink = 0;
    }
  }
  void saveState() {
    // save terminal buffer to eeprom
    for (int y = 0; y < ALCADA; y++) {
      for (int x = 0; x < AMPLADA; x++)
      {
        EEPROM.write(((y*AMPLADA) + x + saveEepromStartIndex), (uint8_t)charArray[x][y]);
      }
    }
    EEPROM.write(saveEepromStartIndex + (ALCADA*AMPLADA) + 1, currPosX);
    EEPROM.write(saveEepromStartIndex + (ALCADA*AMPLADA), currPosY);
    saved = true;
  }
  void loadState() {
    // load terminal buffer from eeprom
    if ((EEPROM.read(0) == 0x00) || (EEPROM.read(0) == 0xFF)) {
      netejaArray();
      saveState();
      return;
    }

    for (int y = 0; y < ALCADA; y++) {
      for (int x = 0; x < AMPLADA; x++)
      {
        charArray[x][y] = (char)EEPROM.read((y*AMPLADA) + x + saveEepromStartIndex);
      }
    }

    currPosX = EEPROM.read(saveEepromStartIndex + (ALCADA*AMPLADA) + 1);
    currPosY = EEPROM.read(saveEepromStartIndex + (ALCADA*AMPLADA));

    saved = true;
  }
  void checkSave() {
    // internal timed eeprom save
    if (saved) return;
    if (millis() > (this->lastChange + saveInterval)) {
      saveState();
      lastChange = millis();
    }
    else if ((lastChange + saveInterval + 1000) < lastChange) {
      saveState();
      lastChange = 0;
    }
  }
  int getpage() {
    // get active eeprom page
    return saveEepromStartIndex / ((ALCADA * AMPLADA) + 2);
  }
  void setPage(int page) {
    // load,display saved terminal buffer from eeprom
    if (!romEnabled)return;
    if ((page < 0) || (page == getpage())) {
      return;
    }
    if (!saved) {
      saveState();
    }
    saveEepromStartIndex = (page * ((ALCADA * AMPLADA) + 2));
    loadState();
    displayPage();
  }
  void displayPage() {
    // page number splash screen
    pantalla->clearDisplay();
    pantalla->setCursor(0, 0);
    pantalla->print("Pag: ");
    pantalla->print(getpage());
    pantalla->display();
    delay(cursorBlinkTime);
    displayArray();
  }
  void run() {
    // display buffer, checks if eeprom save needed
    if (!displayed) {
      //Serial.println("display");
      if (cursorState)cursorBlink(false);
      displayArray();
    }
    else {
      cursorBlinkIfNeeded();
    }
    if (romEnabled)checkSave();
  }
};
