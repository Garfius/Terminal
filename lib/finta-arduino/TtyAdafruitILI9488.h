#include <Arduino.h>
//#include <Adafruit_GFX.h>
//#include <ILI9488.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <TtyAdafruit.h>


#define fontX 6
#define fontY 8
#define ILI9488rows 24
#define ILI9488Cols 80

#define totalColors 8

class TtyAdafruitILI9488: public TtyAdafruit  {
public:
    int colors[totalColors] = {TFT_BLACK,TFT_RED,TFT_GREEN,TFT_YELLOW,TFT_BLUE,TFT_MAGENTA,TFT_CYAN,TFT_WHITE};// girar blau grana
    
    
    TFT_eSPI* ttyDisplay = nullptr;
    TtyAdafruitILI9488 (TFT_eSPI* meowDisplay);
    
    volatile char charArray[ILI9488Cols][ILI9488rows];
    volatile uint8_t charColors[ILI9488Cols][ILI9488rows]; // use merge
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