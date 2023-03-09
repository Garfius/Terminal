#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <TtyAdafruit.h>

// font amplada=6 alçada=8, amplada text = 26, alçada=16?, rotacio=1// term serie == 80x24 -> 480*192
#define fontX 6
#define fontY 8
#define ST7735rows 16
#define ST7735Cols 26

#define totalColors 8

class TtyAdafruitST7735: public TtyAdafruit  {
public:
    int colors[totalColors] = {ST77XX_BLACK,ST77XX_RED,ST77XX_GREEN,ST77XX_YELLOW,ST77XX_BLUE,ST77XX_MAGENTA,ST77XX_CYAN,ST77XX_WHITE};// girar blau grana
    
    
    Adafruit_ST7735* ttyDisplay = nullptr;
    TtyAdafruitST7735 (Adafruit_ST7735* meowDisplay);
    
    char charArray[ST7735Cols][ST7735rows];
    uint8_t charColors[ST7735Cols][ST7735rows]; // use merge
    bool writingInverted = false;
    uint8_t fgColor;// index
    uint8_t bgColor;

    void setBgColor(unsigned int c);
    void setFgColor(unsigned int c);
    // init
    void init();
    // display interafce functions
    void cursorBlink();

    void setTextAttribute(unsigned int c);
    void setChar(unsigned int col,unsigned int row,uint8_t b);// use fgColor,bgColor 
    void copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest);
    // refresh functions
    void updateChar(unsigned int col,unsigned int row ,bool forceFullRefresh = false);
    //void refresh();// not needed 4 this screen model

    
    uint8_t merge(uint8_t a,uint8_t b);
    uint8_t getA(uint8_t composite);
    uint8_t getB(uint8_t composite);
    
};