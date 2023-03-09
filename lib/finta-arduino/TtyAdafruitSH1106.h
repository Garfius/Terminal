#include <Arduino.h>
#include <TtyAdafruit.h>
#include <Adafruit_SH1106_kbv.h>

#define fontX 6
#define fontY 8
#define SH1106rows 8
#define SH1106Cols 21
#define BWbitWise ((SH1106rows * SH1106Cols)/8)+1
/*
Per fer subrallat
pantalla1->drawFastHLine((int16_t)(currCol-1) * fontX, ((int16_t)currRow  * fontY) - 1, fontX - 2, (cursorState) ? BLACK : WHITE);
*/

class TtyAdafruitSH1106: public TtyAdafruit  {
public:
    bool getBit (unsigned int posicio,unsigned  int bit);
    void setBit (unsigned int posicio,unsigned  int bit, bool value);
    void setinvertedColor(unsigned int column, unsigned int row,bool inverted);
    bool getinvertedColor(unsigned int column, unsigned int row);
    uint8_t charArray[SH1106Cols][SH1106rows];
    uint8_t bitWiseBW[BWbitWise];
    Adafruit_SH1106_kbv* ttyDisplay = nullptr;
    bool writingInverted = false;
    TtyAdafruitSH1106 (Adafruit_SH1106_kbv* meowDisplay);
    void init();
    
    void setTextAttribute(unsigned int c);
    void copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest);
    void updateChar(unsigned int col,unsigned int row ,bool forceFullRefresh = false);
    void setChar(unsigned int col,unsigned int row,uint8_t b);

    void cursorBlink();
    void refresh();
};