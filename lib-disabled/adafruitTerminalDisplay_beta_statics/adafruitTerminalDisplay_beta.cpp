#include <adafruitTerminalDisplay_beta.h>
//#include <terminalParser_beta.h>
#include <EEPROM.h>

//-----------------------------------------------classes, si es necessiten dues classes depenents s'han de mencionar a la capçalera
// comentada lina 169 de Adafruit_SH1106.h per accés mètodes interns

//adafruitTerminalDisplay::adafruitTerminalDisplay(Adafruit_SH1106* pantallaExt) {
adafruitTerminalDisplay::adafruitTerminalDisplay() {
  //pantalla = pantallaExt;
  /*
  pantalla->width() 128
  pantalla->height() 64
  llavors mida caràcter = 7 alçada i 7 amplada, es fa +1 per separar 
  */
}
void adafruitTerminalDisplay::init(bool useRom) {
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
  romEnabled = useRom;
  escaped = true;
  // set terminal parser and set pointers to it
  //parsejador->backSpace = &doBack;
}
void adafruitTerminalDisplay::scrollArray() {
  // move all up 1 line, discard top, bottom blank
  for (int y = 0; y < (ALCADA - 1); y++) {
    for (int x = 0; x < AMPLADA; x++)
    {
      charArray[x][y] = charArray[x][y + 1];
    }
  }
  netejaLinia(ALCADA - 1);
}
void adafruitTerminalDisplay::crlf() {
  // crlf cursor
  currPosX = 0;
  currPosY += 1;
  displayed = false;
}
byte adafruitTerminalDisplay::lastFilledChar() {
  // last non blank character of the line
  for (int x = (AMPLADA - 1); x >= 0; x--)
  {
    if (charArray[x][currPosY] != blank) return x;
  }
  return 0;
}
void adafruitTerminalDisplay::borram(){
  
}
void adafruitTerminalDisplay::doBack() {
  // backspace cursor

  if (currPosX > 0) {
    currPosX -= 1;
    //if(erase)charArray[currPosX][currPosY] = blank;
    displayed = false;
  }
  else if (currPosY > 0) {
    currPosY -= 1;
    currPosX = lastFilledChar();
    if ((currPosX < (AMPLADA - 1)) && (currPosX > 0)) {
      currPosX += 1;
    }
    else {
      //if(erase)charArray[currPosX][currPosY] = blank;
    }
    displayed = false;
  }
}
void adafruitTerminalDisplay::write(char caracter) {
  // es fa scroll quan s'ha d'escriure fora de la pantalla despres d'un posicionament
  /*
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

  if ((caracter == 0x08) || (caracter == 0x7F)) {//backspace en telcat usb dona 0x08
    doBack(true);
  }*/
  charArray[currPosX][currPosY] = caracter;
  if (!escaped)currPosX += 1;
  displayed = saved = false;
  lastChange = millis();
  
  if (currPosX >= AMPLADA) {
    crlf();
  }
  if (currPosY >= ALCADA) {
    currPosY = ALCADA - 1;
    scrollArray();
  }
}

void adafruitTerminalDisplay::netejaArray() {
  // assign blank to each character, and set cursor to 0,0
  for (int y = 0; y < ALCADA; y++) {
    netejaLinia(y);
  }
  currPosX = 0;
  currPosY = 0;
}
void adafruitTerminalDisplay::netejaLinia(int y) {
  // assign blank to each character of the line
  for (int x = 0; x < AMPLADA; x++)
  {
    charArray[x][y] = blank;
  }
}
void adafruitTerminalDisplay::displayArray() {
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
void adafruitTerminalDisplay::cursorBlink(bool display) {
  // draw or erase cursor
  pantalla->drawFastHLine((int16_t)currPosX * fontX, (((int16_t)currPosY + 1) * fontY) - 1, fontX - 2, (cursorState) ? WHITE : BLACK);
  cursorState = !cursorState;
  if (display)pantalla->display();
}
void adafruitTerminalDisplay::cursorBlinkIfNeeded() {
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
void adafruitTerminalDisplay::saveState() {
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
void adafruitTerminalDisplay::loadState() {
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
void adafruitTerminalDisplay::checkSave() {
  // internal timed eeprom save
  if (saved) return;
  if (millis() > (lastChange + saveInterval)) {
    saveState();
    lastChange = millis();
  }
  else if ((lastChange + saveInterval + 1000) < lastChange) {
    saveState();
    lastChange = 0;
  }
}
int adafruitTerminalDisplay::getpage() {
  // get active eeprom page
  return saveEepromStartIndex / ((ALCADA * AMPLADA) + 2);
}
void adafruitTerminalDisplay::setPage(int page) {
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
void adafruitTerminalDisplay::displayPage() {
  // page number splash screen
  pantalla->clearDisplay();
  pantalla->setCursor(0, 0);
  pantalla->print("Pag: ");
  pantalla->print(getpage());
  pantalla->display();
  delay(cursorBlinkTime);
  displayArray();
}
void adafruitTerminalDisplay::run() {
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

//adafruitTerminalDisplay myAdafruitTerminalDisplay;
Adafruit_SH1106* adafruitTerminalDisplay::pantalla = nullptr;
terminalParser* adafruitTerminalDisplay::parsejador = nullptr;
byte adafruitTerminalDisplay::currPosX =0;
byte adafruitTerminalDisplay::currPosY =0;
char adafruitTerminalDisplay::charArray[AMPLADA][ALCADA] = {0};
bool adafruitTerminalDisplay::escaped = false;
bool adafruitTerminalDisplay::saved = false;
bool adafruitTerminalDisplay::displayed = false;
unsigned long adafruitTerminalDisplay::lastChange = 0;
bool adafruitTerminalDisplay::cursorState = false;
bool adafruitTerminalDisplay::romEnabled = false;
unsigned long adafruitTerminalDisplay::lastCursorBlink = 0;
unsigned int adafruitTerminalDisplay::saveEepromStartIndex = 0;