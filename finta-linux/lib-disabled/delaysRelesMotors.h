#include "Arduino.h"
#include "superPacket.h"

#ifndef LZZ_delaysRelesMotors_h
#define LZZ_delaysRelesMotors_h
#define LZZ_INLINE inline
class delaysRelesMotors
{
public:
  superPacket config;
  delaysRelesMotors ();
  void desaConfig ();
  void recuperaConfig ();
  unsigned int getSegonsHolgura ();
  void setSegonsHolgura (unsigned int segons);
  unsigned long int getDelayRele ();
  void setDelayReleSeguretat (unsigned long int millis);
  unsigned long int getDelayCanviDireccio ();
  void setDelayCanviDireccio (unsigned long int millis);
  unsigned long int getDelayReactivaRadio ();
  void setDelayReactivaRadio (unsigned long int millis);
  void carregaConfig (superPacket valors);
};
#undef LZZ_INLINE
#endif

//extern LoRaClass LoRa;
extern delaysRelesMotors configRelesMotors;	