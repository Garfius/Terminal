// superPacket.h
//
#include "Arduino.h"
#ifndef LZZ_superPacket_h
#define LZZ_superPacket_h
#define LZZ_INLINE inline

#ifndef numBytesSuperPacket
#define numBytesSuperPacket 8
#endif

class superPacket
{
public:
  static int const numBytesPacket = numBytesSuperPacket;
  bool esNou;
  uint8_t (byteArray) [numBytesSuperPacket];
  uint8_t secret;
  superPacket ();
  void reset ();
  void desaEEPROM (int posicioInici);
  void recuperaDeEEPROM (int posicioInici);
  bool checkSumOk ();
  void setCheckSum ();
  uint8_t get4bit (int byteNum, bool posicioArray);
  unsigned int getEnter16 (int posicio = 0);
  int getEnter16signe (int posicio = 0);
  float getFloat (int posicio = 0);
  void set4bit (uint8_t valor, uint8_t byteNum, bool posicioArray = false);
  void setEnter16 (unsigned int valor, int posicio = 0);
  void setEnter16signe (int valor, int posicio = 0);
  void setFloat (float floatTmp, int posicio = 0);
  void parseBufferHex (char (cadena) []);
  int hex8 (char * in);
  void toHexCharArray (char (cadena) []);
  bool getBit (uint8_t posicio, uint8_t bit);
  void setBit (uint8_t posicio, uint8_t bit, bool value);
};
#undef LZZ_INLINE
#endif
