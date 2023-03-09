#include <TtyAdafruitILI9488.h>

//


TtyAdafruitILI9488::TtyAdafruitILI9488(TFT_eSPI* meowDisplay) {
  ttyDisplay = meowDisplay;
  totalRows = ILI9488rows;
  totalColumns = ILI9488Cols;
}
void TtyAdafruitILI9488::init(){
  TtyAdafruit::init();
  // init values
  ttyDisplay->init();
  ttyDisplay->setRotation(1);
  ttyDisplay->fillScreen(TFT_BLACK);
  ttyDisplay->setCursor(0, 0);
  ttyDisplay->setTextWrap(true);
  ttyDisplay->setTextColor(TFT_WHITE);
  
  fgColor = 7;
  bgColor = 0;
  // init array values via erase display via tty escape command
  this->indexllistaNumeros =1;
  this->llistaNumeros[0] =3;
  this->eD(this->indexllistaNumeros,this->llistaNumeros);
  // refresh screen
  refresh();
}
void TtyAdafruitILI9488::cursorBlink(){ 
  charColors[currCol-1][currRow-1] = merge(getB(charColors[currCol-1][currRow-1]),getA(charColors[currCol-1][currRow-1]));// inverse colors
  TtyAdafruit::cursorBlink();
}
void TtyAdafruitILI9488::setTextAttribute(unsigned int c){
  this->writingInverted = (c == 7);
  if(c == 27) writingInverted = false;
  //
  if(c== 0){
    fgColor = 7;
    bgColor = 0;
  }
}
void TtyAdafruitILI9488::setBgColor(unsigned int c){
  if(c > 7)c=0;
  bgColor = (uint8_t)c;
}
void TtyAdafruitILI9488::setFgColor(unsigned int c){
  if(c > 7)c=7;
  fgColor = (uint8_t)c;
}
void TtyAdafruitILI9488::setChar(unsigned int col,unsigned int row,uint8_t b){
  charArray[col-1][row-1]= b;
  if(writingInverted){
    charColors[col-1][row-1] = merge(fgColor,bgColor);
  }else{
    charColors[col-1][row-1] = merge(bgColor,fgColor);
  }
}
void TtyAdafruitILI9488::copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest){
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
void TtyAdafruitILI9488::updateChar(unsigned int col,unsigned int row ,bool forceFullRefresh){
  int a,b;
  a = (int)getB(charColors[col-1][row-1]);
  b = (int)getA(charColors[col-1][row-1]);
  ttyDisplay->setTextColor( 
    colors[a],
    colors[b]
  );
  //ttyDisplay->setCursor((col-1)*fontX, (row-1)*fontY);
  ttyDisplay->drawChar(charArray[col-1][row-1],(col-1)*fontX, (row-1)*fontY);// ili9488 does not update print pointer
  //ttyDisplay->write(charArray[col-1][row-1]);
}
//void TtyAdafruitILI9488::refresh(){}
//---------------------ram optimization, to-doi get 4 bit binary faster
uint8_t TtyAdafruitILI9488::merge(uint8_t a,uint8_t b){
    return ((a*totalColors)+b);
}
uint8_t TtyAdafruitILI9488::getA(uint8_t composite){
    return composite / totalColors;
}
uint8_t TtyAdafruitILI9488::getB(uint8_t composite){
    return composite % totalColors;
}
