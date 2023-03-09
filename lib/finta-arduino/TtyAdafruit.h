#include <Arduino.h>
#include <terminalParser.h>

#define cursorBlinkTime 750
#define forceRefreshTimeout 250

#define displayMessageBuffer 10

class TtyAdafruit: public terminalParser  {
    void setColor(unsigned int colorCode);
    //multiCoreCommStatusList commStats;
  public:
    static Stream* keyboard;
    //char charArray[1][1];
    char backGroundChar = ' ';
    char displayAnswer[displayMessageBuffer];
    unsigned int answerIndex,answerTotal,fgColor,bgColor = 0;
    unsigned int currCol = 1;
    unsigned int currRow = 1;// position, no offset
    unsigned int totalColumns,totalRows;
    volatile bool cursorState,lineEnded,writingInverted = false;
    volatile bool cursorEnabled = true;
    unsigned long nextCursorBlink,lastChange = 0;
    
    TtyAdafruit();    
    // Stream functions
    int available();/**< requiered to blink & more! */
    void flush();/**for immediate refresh if needed*/
    int peek();
    int read();
    size_t write(uint8_t b);/**< in here goes doguess*/
    
    void cursorBlinkIfNeeded();/* cap dins run*/
    void scrollArray();
    void clearRow(unsigned int row);
    virtual void cursorBlink();
    virtual void init();
    void run();
    
    // generic interface to display adapters
    virtual void setTextAttribute(unsigned int c);
    virtual void setBgColor(unsigned int c);
    virtual void setFgColor(unsigned int c);
    virtual void setChar(unsigned int col,unsigned int row,uint8_t b);// use fgColor,bgColor 
    virtual void copyChar(unsigned int colOr,unsigned int rowOr,unsigned int colDest,unsigned int rowDest);
    // refresh functions
    virtual void updateChar(unsigned int col,unsigned int row ,bool forceFullRefresh = false);
    virtual void refresh();

    // inherited functions from terminalParser to acquire tty command triggers
    void sGR(unsigned int argc,int *argv);    
    void cHA(unsigned int argc,int *argv);
    void eD(unsigned int argc,int *argv);
    void dA(unsigned int argc,int *argv);
    void eL(unsigned int argc,int *argv);
    void cUP(unsigned int argc,int *argv);
    void vPA(unsigned int argc,int *argv);
    void rM(unsigned int argc,int *argv);
    void sM(unsigned int argc,int *argv);
    void cUU(unsigned int argc,int *argv);
    void cUD(unsigned int argc,int *argv);
    void cUF(unsigned int argc,int *argv);
    void cUB(unsigned int argc,int *argv);
    void dSR(unsigned int argc,int *argv);
    void dCH(unsigned int argc,int *argv);
    void cr();
    void lf();
    void backSpace();
    //--------------------per aprovar--------------
    
};


//extern adafruitTerminalDisplay SH1106Termialfunctions;