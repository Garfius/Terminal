#include <TtyAdafruitST7735.h>

// Some ready-made 16-bit ('565') color settings:


TtyAdafruitST7735::TtyAdafruitST7735(Adafruit_ST7735* meowDisplay) {
  ttyDisplay = meowDisplay;
  totalRows = ST7735rows;
  totalColumns = ST7735Cols;
}
void TtyAdafruitST7735::init(){
  TtyAdafruit::init();
  // init values
  ttyDisplay->initR(INITR_GREENTAB);      // Init ST7735S chip, green tab
  ttyDisplay->setRotation(1);
  ttyDisplay->fillScreen(ST77XX_BLACK);
  ttyDisplay->setCursor(0, 0);
  ttyDisplay->setTextWrap(true);
  ttyDisplay->setTextColor(ST77XX_WHITE);
  fgColor = 7;
  bgColor = 0;
  // init array values via erase display via tty escape command
  this->indexllistaNumeros =1;
  this->llistaNumeros[0] =3;
  this->eD(this->indexllistaNumeros,this->llistaNumeros);
  // refresh screen
  refresh();
}
void TtyAdafruitST7735::cursorBlink(){ 
  charColors[currCol-1][currRow-1] = merge(getB(charColors[currCol-1][currRow-1]),getA(charColors[currCol-1][currRow-1]));// inverse colors
  TtyAdafruit::cursorBlink();
}
void TtyAdafruitST7735::setTextAttribute(unsigned int c){
  this->writingInverted = (c == 7);
  if(c == 27) writingInverted = false;
  //
  if(c== 0){
    fgColor = 7;
    bgColor = 0;
  }
}
void TtyAdafruitST7735::setBgColor(unsigned int c){
  if(c > 7)c=0;
  bgColor = (uint8_t)c;
}
void TtyAdafruitST7735::setFgColor(unsigned int c){
  if(c > 7)c=7;
  fgColor = (uint8_t)c;
}
void TtyAdafruitST7735::setChar(unsigned int col,unsigned int row,uint8_t b){
  charArray[col-1][row-1]= b;
  if(writingInverted){
    charColors[col-1][row-1] = merge(fgColor,bgColor);
  }else{
    charColors[col-1][row-1] = merge(bgColor,fgColor);
  }
}
void TtyAdafruitST7735::copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest){
  /*
  if((colOr > totalColumns) ||( colDest > totalColumns) ||(colOr ==0 )||(colDest ==0 )){
    Serial.println("aslkdjfghaksljdhf");
  }
  if((rowOr > totalColumns) ||( rowDest > totalColumns) ||(rowOr ==0 )||(rowDest ==0 )){
    Serial.println("aslkdas12312312hf");
  }
  */
  charArray[colDest-1][rowDest-1] = charArray[colOr-1][rowOr-1];
  charColors[colDest-1][rowDest-1] = charColors[colOr-1][rowOr-1];
}
void TtyAdafruitST7735::updateChar(unsigned int col,unsigned int row ,bool forceFullRefresh){
  ttyDisplay->setTextColor( 
    colors[getB(charColors[col-1][row-1])],
    colors[getA(charColors[col-1][row-1])]
  );
  //ttyDisplay->setCursor((col-1)*fontX, (row-1)*fontY);
  ttyDisplay->drawChar((col-1)*fontX, (row-1)*fontY,charArray[col-1][row-1],colors[getB(charColors[col-1][row-1])],colors[getA(charColors[col-1][row-1])],1,1);
  //ttyDisplay->write(charArray[col-1][row-1]);
}
//void TtyAdafruitST7735::refresh(){}
//---------------------ram optimization, to-doi get 4 bit binary faster
uint8_t TtyAdafruitST7735::merge(uint8_t a,uint8_t b){
    return ((a*totalColors)+b);
}
uint8_t TtyAdafruitST7735::getA(uint8_t composite){
    return composite / totalColors;
}
uint8_t TtyAdafruitST7735::getB(uint8_t composite){
    return composite % totalColors;
}
