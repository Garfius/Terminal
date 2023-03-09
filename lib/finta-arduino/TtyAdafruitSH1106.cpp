#include <TtyAdafruitSH1106.h>


TtyAdafruitSH1106::TtyAdafruitSH1106(Adafruit_SH1106_kbv* meowDisplay) {
  ttyDisplay = meowDisplay;
  totalRows = SH1106rows;
  totalColumns = SH1106Cols;
}
void TtyAdafruitSH1106::init() {
  TtyAdafruit::init();
  //ttyDisplay->begin(SH1106_SWITCHCAPVCC);
  ttyDisplay->begin(SSD1306_SWITCHCAPVCC, 0x3C); // prova-ho
  ttyDisplay->clearDisplay();
  ttyDisplay->setCursor(0, 0);
  ttyDisplay->setTextWrap(true);
  ttyDisplay->setTextColor(WHITE,BLACK);
  ttyDisplay->setTextSize(1);
  refresh();
}
/**
 * @brief from charMap+ColorMap to display (display needs update?)
 * 
 * @param x 
 * @param y 
 */
void TtyAdafruitSH1106::updateChar(unsigned int col,unsigned int row ,bool forceFullRefresh){

    if(getinvertedColor(col,row)){          
      ttyDisplay->setTextColor( BLACK,WHITE)  ;
    }else{
      ttyDisplay->setTextColor( WHITE, BLACK)  ;
    }
    ttyDisplay->setCursor((col-1)*fontX, (row-1)*fontY);
    ttyDisplay->write(charArray[col-1][row-1]);
}
void TtyAdafruitSH1106::setChar(unsigned int col,unsigned int row,uint8_t b){
  charArray[col-1][row-1]= b;
  setinvertedColor(col,row,writingInverted);
}
void TtyAdafruitSH1106::copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest){
      charArray[colDest-1][rowDest-1] = charArray[colOr-1][rowOr-1];
      setinvertedColor(colDest,rowDest,getinvertedColor(colOr,rowOr));
}

bool TtyAdafruitSH1106::getBit (unsigned int posicio, unsigned int bit){
		return  0 != (bitWiseBW[posicio] & (1 << bit));
	}
void TtyAdafruitSH1106::setBit (unsigned int posicio,unsigned  int bit, bool value){// retorna el valor d'un bit en una posicio concreta del byte en una posicio del byteArray
    if (value) {
        bitWiseBW[posicio] |= 1UL << bit;
    }
    else {
        bitWiseBW[posicio] &= ~(1UL << bit);
    }
}
void TtyAdafruitSH1106::setinvertedColor(unsigned int column, unsigned int row,bool inverted){// començant a 1,1
    int offset = ((row-1)*totalColumns)+(column-1);
    setBit(offset / 8,offset % 8,inverted);
}
bool TtyAdafruitSH1106::getinvertedColor(unsigned int column, unsigned int row){// començant a 1,1
    int offset = ((row-1)*totalColumns)+(column-1);
    //int bytepos = offset / 8;    int bitOffset = offset % 8;
    return getBit(offset / 8,offset % 8);
}
void TtyAdafruitSH1106::cursorBlink(){
  setinvertedColor(currCol,currRow,!cursorState);
  TtyAdafruit::cursorBlink();
}
/**
 * @brief TO-DO , improve api
 * 
 * @param c 
 */
void TtyAdafruitSH1106::setTextAttribute(unsigned int c){
  this->writingInverted = (c == 7);

}
void TtyAdafruitSH1106::refresh(){
  ttyDisplay->display();
  TtyAdafruit::refresh();
}
