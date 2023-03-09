// delaysRelesMotors.cpp
//

#include "delaysRelesMotors.h"
#include "superPacket.h"
#include "Arduino.h"

#define LZZ_INLINE inline

static int const posEEPROMdelaysRelesMotors = 1;// gasta numBytesSuperPacket=8 byte ?, mira persianes.cpp

delaysRelesMotors::delaysRelesMotors ()
                       {
        config = superPacket();
		this->recuperaConfig();
    }
void delaysRelesMotors::desaConfig ()
                      {
        this->config.desaEEPROM(posEEPROMdelaysRelesMotors);
    }
void delaysRelesMotors::recuperaConfig ()
                          {
        this->config.recuperaDeEEPROM(posEEPROMdelaysRelesMotors);
    }
unsigned int delaysRelesMotors::getSegonsHolgura ()
                                    {
        return this->config.getEnter16(0);
	}
void delaysRelesMotors::setSegonsHolgura (unsigned int segons)
                                                   {
		this->config.setEnter16(segons, 0);
	}
unsigned long int delaysRelesMotors::getDelayRele ()
                                     {
		return (unsigned long)this->config.getEnter16(2);
	}
void delaysRelesMotors::setDelayReleSeguretat (unsigned long int millis)
                                                         {
		this->config.setEnter16((unsigned int)millis, 2);
	}
unsigned long int delaysRelesMotors::getDelayCanviDireccio ()
                                              {
		return (unsigned long)this->config.getEnter16(4);
	}
void delaysRelesMotors::setDelayCanviDireccio (unsigned long int millis)
                                                         {
		this->config.setEnter16((unsigned int)millis, 4);
	}
unsigned long int delaysRelesMotors::getDelayReactivaRadio ()
                                              {
		return (unsigned long)this->config.getEnter16(6);
	}
void delaysRelesMotors::setDelayReactivaRadio (unsigned long int millis)
                                                         {
		this->config.setEnter16((unsigned int)millis, 6);
	}
void delaysRelesMotors::carregaConfig (superPacket valors)
                                           {
		this->setSegonsHolgura((unsigned int)valors.byteArray[3]);
		this->setDelayReleSeguretat((unsigned long)valors.byteArray[4] * 10);
		this->setDelayCanviDireccio((unsigned long)valors.byteArray[5] * 10);
		this->setDelayReactivaRadio((unsigned long)valors.byteArray[6] * 10);// es fa servir
	}


delaysRelesMotors configRelesMotors;	
#undef LZZ_INLINE

