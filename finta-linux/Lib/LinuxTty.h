#include "terminalParser.h"

/*
El dummy de linux ha d'enviar a pantalla, tot 
*/
#define _SS_MAX_RX_BUFF 10
class LinuxTty : public terminalParser {
public:
  CSI_SEQUENCES llista_CSI;
  char charTmp;
  bool peaked;
  void _addBuffer(unsigned char byte);
  LinuxTty();
  char keyboardBuffer[terminalParser_bufferSize];// per poder donar respostes sobrecaregant funció pantalla
  int keyboardBufferIndex;
  int read();
  int peek(); // get 1 extra buffer if peek
  int available();
  size_t write(unsigned char byte,bool flush = true);
  //size_t print(const char c[]);
  // tty operations
  void cUP(unsigned int argc,int *argv);
  void eD(unsigned int argc,int *argv);
  int write(char* holas);
  int write(char holas);
  int print(char* holas);

};
#ifdef finta_linux
    #include <cstddef>
    #include "stream.h"
#else
    #include <Arduino.h>
#endif