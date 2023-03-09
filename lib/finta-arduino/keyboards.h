#include <Arduino.h>
#include <stdio.h>
#include <Wire.h>


#define keyboardMessageBuffer 10

class baseTtyKeyboard : public Stream{
    public:
    int available();
    int peek();
    int read();
};
// cal crear una baseclass de teclats per evitar mencionar funciobs virtuals puers sense codi assignat
class IICTtyKeyboard :public baseTtyKeyboard {
public: 
    IICTtyKeyboard();
    char displayAnswer[keyboardMessageBuffer];
    unsigned int answerIndex,answerTotal = 0;
    int _toTty(char caracter);
    int available();
    int peek();
    int read();
    size_t write(uint8_t);
};
class PS2TtyKeyboard :public baseTtyKeyboard {
public: 
    PS2TtyKeyboard();
    char displayAnswer[keyboardMessageBuffer];
    int answerIndex;
    int available();
    int peek();
    int read();    
};