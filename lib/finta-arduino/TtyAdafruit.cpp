#include <TtyAdafruit.h>
/**
* @file TtyAdafruit.cpp
* @brief SH1106 to tty Stream driver, this->keyboard Stream pointer used for user input
* 
* @author Gerard Forcada Bigas
*
* @date 20/12/22
*/
//-----------------------------------------------classes, si es necessiten dues classes depenents s'han de mencionar a la capçalera

/*
font normal amplada=6 alçada=8, amplada text = 26, alçada=16?, rotacio=1// term serie == 80x24 -> 480*192 de 320 en aquesta config.
*/

// hating compilers is ok


TtyAdafruit::TtyAdafruit() {
  nextCursorBlink = millis()+cursorBlinkTime;
  //commStats = freeComms;
}
void TtyAdafruit::run(){
  cursorBlinkIfNeeded();
  if(millis() > (lastChange+forceRefreshTimeout)){
    refresh(); //test disabled
    lastChange = millis();
  }
}
int TtyAdafruit::available(){
  int tmp;
  /*if(commStats != freeComms)return 0;
  commStats = wireWorking;*/

  if(answerTotal == 0){
    tmp = keyboard->available();
  }else{
    tmp = (answerTotal-answerIndex)+keyboard->available();
  }
  //commStats = freeComms;

  return tmp;
}
void TtyAdafruit::flush(){
  this->refresh();
  lastChange = millis();
  this->keyboard->flush();
}
int TtyAdafruit::peek(){
  if(answerTotal != 0){
    return displayAnswer[answerIndex];
  }
  return keyboard->peek();
}
int TtyAdafruit::read(){
  //unsigned long aasd = millis();
  
  if(answerTotal != 0){
    answerIndex++;
    if(!displayAnswer[answerIndex]){
      answerTotal = 0;
    }
    return displayAnswer[answerIndex-1];
  }
  //if(aasd == millis()){aasd --;}
  return keyboard->read();
}
size_t TtyAdafruit::write(uint8_t b){
  // cal usar variables heretades de terminalParser per saber si es retorna 0 o no!
  if(cursorState)cursorBlink();
  
  bool taste = this->doGuess(b);

  if(!taste && !esc && !controlSequence){/*not 4 screen, but 1st check terminal parser state*/
    return 0;
  }else if(esc || controlSequence){
    return 1;// diu que escriu pro no fa res.
  }
  
  
  
  /*else{
    lastChange = millis();
  }*/
  
  if(currCol == totalColumns){
    if(lineEnded){// ja haviem escrit el últim caracter, toca saltar de linia
      if(currRow == totalRows){// i potser toca scroll
        scrollArray();
        currCol = 1;
        setChar(currCol,currRow,b);
        currCol++;
        /*setinvertedColor(currCol,currRow,writingInverted);
        charArray[currCol-1][currRow-1] = b;
        updateChar(currCol-1,currRow-1);*/
      }else{// o saltar de linia
        currRow += 1;
        currCol = 1;
        setChar(currCol,currRow,b);
        currCol++;
      }
      lineEnded = false;  
    }else{// escrivim ultim caracter, pro no saltem
      lineEnded = true;
      setChar(currCol,currRow,b);
    }
  }else{
    setChar(currCol,currRow,b);
    currCol ++;
  }

  return 1;
}
void TtyAdafruit::init() {
  /*this->indexllistaNumeros =1;
  this->llistaNumeros[0] =6;
  this->dSR(this->indexllistaNumeros,this->llistaNumeros);  */
}

void TtyAdafruit::cursorBlink() {
  // draw or erase cursor
  
  cursorState = !cursorState;
  // now must rewrite this to display
  //updateChar(currCol,currRow);
  //lastChange = millis();
  nextCursorBlink = 0;
}

/**
 * @brief 0 Reset all attributes		
1 Set "bright" attribute		
2 Set "dim" attribute		
3 Set "standout" attribute		
4 Set "underscore" (underlined text) attribute		
5 Set "blink" attribute		
7 Set "reverse" attribute		
8 Set "hidden" attribute					
 * 
 * @param c 
 */
void TtyAdafruit::setTextAttribute(unsigned int c){}
/**
 * @brief 0 Set foreground to color #0 - black		
1 Set foreground to color #1 - red		
2 Set foreground to color #2 - green		
3 Set foreground to color #3 - yellow		
4 Set foreground to color #4 - blue		
5 Set foreground to color #5 - magenta		
6 Set foreground to color #6 - cyan		
7 Set foreground to color #7 - white		
9 Set default color as foreground color		
 * 
 * @param c 
 */
void TtyAdafruit::setBgColor(unsigned int c){}
/**
 * @brief 0 Set foreground to color #0 - black		
1 Set foreground to color #1 - red		
2 Set foreground to color #2 - green		
3 Set foreground to color #3 - yellow		
4 Set foreground to color #4 - blue		
5 Set foreground to color #5 - magenta		
6 Set foreground to color #6 - cyan		
7 Set foreground to color #7 - white		
9 Set default color as foreground color		
 * 
 * @param c 
 */
void TtyAdafruit::setFgColor(unsigned int c){}
void TtyAdafruit::setChar(unsigned int col,unsigned int row,uint8_t b){
}
/**
 * @brief copy all the pixel characteristics from one to another
 * 
 */
void TtyAdafruit::copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest){

}
/**
 * @brief from char array to Display
 * 
 * @param x offset, not position
 * @param y offset, not position
 */
void TtyAdafruit::updateChar(unsigned int col,unsigned int row,bool forceFullRefresh){
   
}
/**
 * @brief show terminal array on screen
 * 
 */
void TtyAdafruit::refresh() {
  /*while(commStats != freeComms){delay(50);}// latency latencia
  commStats = spiWorking;*/
  for (unsigned int y = 1; y <= totalRows; y++) {//row
    for (unsigned int x = 1; x <= totalColumns; x++)//column
    {
      updateChar(x,y); 
    }
  }
  //commStats = freeComms;
}

void TtyAdafruit::clearRow(unsigned int row) {
  // assign blank to each character of the line
  for (unsigned int x = 1; x <= totalColumns; x++)
  {
    setChar(x,row,backGroundChar);
  }
}
void setBgColor(int col,int row){

}
void TtyAdafruit::scrollArray() {
  // move all up 1 line, discard top, bottom blank
  for (unsigned int y = 1; y <= (totalRows - 1); y++) {
    for (unsigned int x = 1; x <= totalColumns; x++)
    {
      copyChar(x,y+1,x,y);
    }
  }
  clearRow(totalRows);
}


void TtyAdafruit::cursorBlinkIfNeeded() {
  // check internal timer and calls cursor blink if needed
  if(!cursorEnabled)return;
  //if(currRow >= totalRows )return;
  if (millis() > nextCursorBlink) {
    cursorBlink();
    nextCursorBlink = millis()+cursorBlinkTime;
  }
  else if ((nextCursorBlink + cursorBlinkTime) < nextCursorBlink) {// clock loop
    cursorBlink();
    
  }
}
/**
 * @brief assign argv[0] to currCol
 * 
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::cHA(unsigned int argc,int *argv){
  if((argc != 1) ||((unsigned int)argv[0] ==0))return;
  currCol = argv[0];
   if(totalColumns < currCol){
    currCol = totalColumns;
  }
  
}
/**
 * @brief assign argv[0] to currRow
 * 
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::vPA(unsigned int argc,int *argv){
  if((argc != 1) ||((unsigned int)argv[0] ==0))return;
  currRow = argv[0];
  if(totalRows < currRow){
    currRow = totalRows;
  }
}
/**
 * @brief ESC +...
     [J clears the part of the screen from the cursor to the end of the screen.
    [ 1 J erase from start to cursor
    [ 2 J erase whole display
     [ 3 J erase whole display including scroll
 * 
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::eD(unsigned int argc,int *argv) {
  if(argc ==0){
    for (unsigned int x = currCol; x <= totalColumns; x++) {
      setChar(x,currRow,backGroundChar);
    }
    for (unsigned int y = currRow+1; y <= totalRows; y++) {
      clearRow(y);
    }
  }else if(argc ==1){
    if(argv[0] == 1){
      for (unsigned int x = currCol; x >= 1; x--) {
        setChar(x,currRow,backGroundChar);
      }
      for (unsigned int y = 1; y < currRow; y++) {
        clearRow(y);
      }
    }else if(argv[0] == 2){
      for (unsigned int y = 1; y <= totalRows; y++) {
        clearRow(y);
      }
    }else if(argv[0] == 3){
      for (unsigned int y = 1; y <= totalRows; y++) {// to-do scroll area
        clearRow(y);
      }
    }
  }
}
/**
 * @brief to-do private mode?¿
 * 
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::dA(unsigned int argc,int *argv){
  if((totalRows >= 24) && (totalColumns >= 80)){
    strcpy(displayAnswer,"\e[?6c");  
    answerTotal = strlen(displayAnswer);
    answerIndex = 0;
  }
}
/**
 * @brief // 'P'  -  Delete the indicated # of characters on current line
 * delete press...
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::dCH(unsigned int argc,int *argv){
  if(cursorState)cursorBlink();
  if(argc ==0)argv[0] =1;
  argv[1] = ((int)totalColumns - (int)currCol)-argv[0];// argv[1] is the amount of characters to copy
  // argv[0] is distance 
  if(argv[1] > 0 ){
    unsigned int desplacament = (unsigned int)argv[0];
    unsigned int ultimDesti = currCol+desplacament;
    if(desplacament==0)desplacament++;
    
    for (unsigned int copesFetes = 0; copesFetes <= desplacament ; copesFetes++) {
      if((currCol+copesFetes+desplacament) > totalColumns)break;
      ultimDesti = currCol+copesFetes;
		  copyChar( currCol+copesFetes+desplacament,currRow,currCol+copesFetes,currRow);
      //Serial.println("or"+(String)(currCol+copesFetes+desplacament)+"->"+(String)(currCol+copesFetes));
    }
    for (unsigned int x = ultimDesti+1; x <= totalColumns ; x++) {
        setChar(x,currRow,backGroundChar);
        //Serial.println("clear"+(String)x);
    }
  
  }else{
      //EL        Erase line (default: from cursor to end of line).
    for (unsigned int x = currCol; x <= totalColumns; x++) {
      setChar(x,currRow,backGroundChar);
    }
  }
  
}
/**
 * @brief K   EL        Erase line (default: from cursor to end of line).
                     ESC [ 1 K erase from start of line to cursor.
                     ESC [ 2 K erase whole line.
 * 
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::eL(unsigned int argc,int *argv){
  if(lineEnded)currRow++;
  if(argc ==0){
    for (unsigned int x = currCol; x <= totalColumns; x++) {
      setChar(x,currRow,backGroundChar);
    }
  }else if(argc ==1){
    if(argv[0] == 1){
      for (unsigned int x = currCol; x >= 1; x--) {
        setChar(x,currRow,backGroundChar);
      }
    }else if(argv[0] == 2){
      clearRow(currRow);
    }
  }
  if(lineEnded)currRow--;
}
void TtyAdafruit::cUP(unsigned int argc,int *argv){// cursor position
  if(cursorState)cursorBlink();
  
  if(argc == 0){
    currCol = currRow = 1;
    return;
  }
  
  if(argc !=2)return;
  this->indexllistaNumeros = 1;
  this->llistaNumeros[0] = argv[0];
  this->vPA(indexllistaNumeros,llistaNumeros);
  this->llistaNumeros[0] = argv[1];
  this->cHA(indexllistaNumeros,llistaNumeros);
  /*currCol = argv[1];
  currRow = argv[0];*/
}
//-------------------------------------------------stream/serial functions--------------
//currCol,currRow




























//---------------------terminalParser funcions
void TtyAdafruit::cr() {
  if(cursorState)cursorBlink();
  currCol = 1;
}
void TtyAdafruit::lf() {
  if(cursorState)cursorBlink();
  currRow ++;
  if(currRow > totalRows){
    currRow = totalRows;// last line
    scrollArray();
  }
}


void TtyAdafruit::rM(unsigned int argc,int *argv){//Reset Mode, 
  if(argv[0]!=25)return;
  if(cursorState)cursorBlink();
  cursorEnabled = false;
}
void TtyAdafruit::sM(unsigned int argc,int *argv){//Set Mode (see below).
  if(argv[0]!=25)return;
  if(cursorState)cursorBlink();
  cursorEnabled = true;
}
/**
 * @brief internal
 * 
 * @param colorCode 
 */
void TtyAdafruit::setColor(unsigned int colorCode){
  if((colorCode > 29) &&(colorCode < 40)){
    setFgColor(colorCode % 30);
  }else if((colorCode > 39) &&(colorCode < 50)){
    setBgColor(colorCode % 40);
  }
}
/**
 * @brief 
 * [7m || [0;10;7m <-- colors inversos
   [0m || [m || <-- colors normals
   [39;49m  <-- colors normals i treu subrallat...
   esc = (b'\x1b').decode("utf-8") <-- python test
 * @param argc 
 * @param argv 
 */
void TtyAdafruit::sGR(unsigned int argc,int *argv){
  switch (argc)
  {
  case 0:
    setTextAttribute(0);
    break;
    case 1:
    if((unsigned int)argv[0] > 8){
      this->setColor((unsigned int)argv[0]);
    }else{
      setTextAttribute((unsigned int)argv[0]);
    }
    break;
    case 2:
    setTextAttribute((unsigned int)argv[0]);
    this->setColor((unsigned int)argv[1]);
    break;
    case 3:
    setTextAttribute((unsigned int)argv[0]);
    this->setColor((unsigned int)argv[1]);
    this->setColor((unsigned int)argv[2]);
    break;
  
  default:
    break;
  }
}

void TtyAdafruit::cUU(unsigned int argc,int *argv){//Move cursor up the indicated # of rows.
  if(currRow == 1)return;
  if(cursorState)cursorBlink();

  if(argc == 0){
    currRow --;
    
  }else if(argc == 1){
    if((unsigned int)argv[0] >= currRow){
      currRow =1;  
    }else{
      currRow = currRow - (unsigned int)argv[0];  
    }
  }
}
void TtyAdafruit::cUD(unsigned int argc,int *argv){//Move cursor down the indicated # of rows.
  if(currRow == totalRows)return;
  if(cursorState)cursorBlink();

  if(argc == 0){
    currRow ++;
  }else if(argc == 1){
    if((currRow+(unsigned int)argv[0]) > totalRows){
      currRow = totalRows;  
    }else{
      currRow = currRow + (unsigned int)argv[0];  
    }
  }
}
void TtyAdafruit::cUF(unsigned int argc,int *argv){//Move cursor right the indicated # of columns.
  if(currCol == totalColumns)return;
  
  if(cursorState)cursorBlink();

  if(argc == 0){
    currCol ++;
  }else if(argc == 1){
    if((currCol+(unsigned int)argv[0]) > totalColumns){
      currCol = totalColumns;  
    }else{
      currCol = currCol + (unsigned int)argv[0];  
    }
  }
}
void TtyAdafruit::cUB(unsigned int argc,int *argv){//Move cursor left the indicated # of columns.
  if(currCol == 1)return;
  if(cursorState)cursorBlink();

  if(argc == 0){
    currCol --;
    
  }else if(argc == 1){
    if((unsigned int)argv[0] >= currCol){
      currCol =1;  
    }else{
      currCol = currCol - (unsigned int)argv[0];  
    }
  }
}
void TtyAdafruit::backSpace() {
  // backspace cursor, no delete detected on remmina terminal
  if(cursorState)cursorBlink();
  if (currCol > 0) {
    currCol -= 1;
  }
}
/**
 * @brief  respond cirsor position
 * 
 * @param argc 1
 * @param argv {6} for size, 5 for ping
 */
void TtyAdafruit::dSR(unsigned int argc,int *argv){
    if(argc!=1)return;
    if(argv[0] == 5){
      strcpy(displayAnswer,"\e[0n");
    }else if(argv[0] == 6){
      sprintf(displayAnswer,"\e[%d;%dR",currRow,currCol);
    }
    
    answerTotal = strlen(displayAnswer);
    answerIndex = 0;
}

Stream* TtyAdafruit::keyboard = nullptr;

//--------------------------------------------------------bitwise inverted color



//--------------------------------------------deprecated ---------------
/*
// no use, legacy EEPROM operations
void TtyAdafruit::saveState() {
  // save terminal buffer to eeprom
  for (int y = 0; y < totalRows; y++) {
    for (int x = 0; x < totalColumns; x++)
    {
      EEPROM.write(((y*totalColumns) + x + saveEepromStartIndex), (uint8_t)charArray[x][y]);
    }
  }
  EEPROM.write(saveEepromStartIndex + (totalRows*totalColumns) + 1, currCol);
  EEPROM.write(saveEepromStartIndex + (totalRows*totalColumns), currRow);
  saved = true;
}
void TtyAdafruit::loadState() {
  // load terminal buffer from eeprom
  if ((EEPROM.read(0) == 0x00) || (EEPROM.read(0) == 0xFF)) {
    netejaArray();
    saveState();
    return;
  }

  for (int y = 0; y < totalRows; y++) {
    for (int x = 0; x < totalColumns; x++)
    {
      charArray[x][y] = (char)EEPROM.read((y*totalColumns) + x + saveEepromStartIndex);
    }
  }

  currCol = EEPROM.read(saveEepromStartIndex + (totalRows*totalColumns) + 1);
  currRow = EEPROM.read(saveEepromStartIndex + (totalRows*totalColumns));

  saved = true;
}
void TtyAdafruit::checkSave() {
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
int TtyAdafruit::getpage() {
  // get active eeprom page
  return saveEepromStartIndex / ((totalRows * totalColumns) + 2);
}
void TtyAdafruit::setPage(int page) {
  // load,display saved terminal buffer from eeprom
  if (!romEnabled)return;
  if ((page < 0) || (page == getpage())) {
    return;
  }
  if (!saved) {
    saveState();
  }
  saveEepromStartIndex = (page * ((totalRows * totalColumns) + 2));
  loadState();
  displayPage();
}
void TtyAdafruit::displayPage() {
  // page number splash screen
  pantalla1->clearDisplay();
  pantalla1->setCursor(0, 0);
  pantalla1->print("Pag: ");
  pantalla1->print(getpage());
  pantalla1->display();
  delay(cursorBlinkTime);
  displayArray();
}*/
// has de fer write, cup i eD, cursorPosisiom, eraseDisplay i write... no és tant?


//adafruitTerminalDisplay SH1106Termialfunctions;


/*
//terminalParser* TtyAdafruit::parsejador = nullptr;
byte TtyAdafruit::currCol =0;
byte TtyAdafruit::currRow =0;
char TtyAdafruit::charArray[totalColumns][totalRows] = {0};
bool TtyAdafruit::escaped = false;
bool TtyAdafruit::saved = false;
bool TtyAdafruit::displayed = false;
unsigned long TtyAdafruit::lastChange = 0;
bool TtyAdafruit::cursorState = false;
bool TtyAdafruit::romEnabled = false;
unsigned long TtyAdafruit::lastCursorBlink = 0;
unsigned int TtyAdafruit::saveEepromStartIndex = 0;


byte TtyAdafruit::lastFilledChar() {
  // last non blank character of the line
  for (int x = (totalColumns - 1); x >= 0; x--)
  {
    if (charArray[x][currRow] != blank) return x;
  }
  return 0;
}
*/