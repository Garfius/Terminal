// superPacket.cpp
//
#include "Arduino.h"
#include "superPacket.h"
#include "EEPROM.h"
#define LZZ_INLINE inline

int const superPacket::numBytesPacket;
superPacket::superPacket ()
                      {
		this->reset();
		this->esNou = false;
		this->secret = 0;
	}
void superPacket::reset ()
                     {
		for (int i = 0; i <= (numBytesSuperPacket - 1); i++) {
			byteArray[i] = 0;
		}
	}
void superPacket::desaEEPROM (int posicioInici)
                                          {
		for (int i = 0; i <= (numBytesSuperPacket - 1); i++) {
			EEPROM.write(posicioInici + i, this->byteArray[i]);
		}
	}
void superPacket::recuperaDeEEPROM (int posicioInici)
                                                {
		for (int i = 0; i <= (numBytesSuperPacket - 1); i++) {
			this->byteArray[i] = EEPROM.read(posicioInici + i);
		}
	}
bool superPacket::checkSumOk ()
                          {// checksum 8 en ultim byte
		int tmpInt = secret;
		for (int i = 0; i <= (numBytesSuperPacket - 2); i++) {
			tmpInt += byteArray[i];
		}
		return (((uint8_t)(tmpInt % 255)) == byteArray[(numBytesSuperPacket - 1)]);
	}
void superPacket::setCheckSum ()
                           {
		int tmpInt = secret;
		for (int i = 0; i <= (numBytesSuperPacket - 2); i++)
		{
			tmpInt += byteArray[i];
		}
		byteArray[numBytesSuperPacket - 1] = ((uint8_t)(tmpInt % 255));
	}
uint8_t superPacket::get4bit (int byteNum, bool posicioArray)
                                                        {// 0 primera, 1 segona
		if (posicioArray) {
			return ((byteArray[byteNum] & 0XF0) >> 4);
		}
		else {
			return (byteArray[byteNum] & 0X0F);
		}
	}
unsigned int superPacket::getEnter16 (int posicio)
                                                 {
		return (unsigned int)(((unsigned int)byteArray[posicio] << 8) + ((unsigned int)byteArray[posicio + 1]));
	}
int superPacket::getEnter16signe (int posicio)
                                             {
		return (int)(((int)byteArray[posicio] << 8) + ((int)byteArray[posicio + 1]));
	}
float superPacket::getFloat (int posicio)
                                        {
		float floatTmp;
		memcpy(&floatTmp, this->byteArray + posicio, 4);
		return floatTmp;
	}
void superPacket::set4bit (uint8_t valor, uint8_t byteNum, bool posicioArray)
                                                                                {
		//Serial.println("#set4 "+(String)valor+" "+(String)byteNum+" "+(String)posicioArray);
		if (posicioArray)
		{
			byteArray[byteNum] = (uint8_t)((byteArray[byteNum] & 0x0F) | ((valor << 4) & 0xF0));
		}
		else
		{
			byteArray[byteNum] = (uint8_t)((byteArray[byteNum] & 0xF0) | (valor & 0x0F));
		}
		//Serial.print("#");		Serial.println(byteArray[byteNum],HEX);
	}
void superPacket::setEnter16 (unsigned int valor, int posicio)
                                                             {
		byteArray[posicio] = (uint8_t)((valor >> 8) & 0xffFF);
		byteArray[posicio + 1] = (uint8_t)(valor & 0xffFF);
	}
void superPacket::setEnter16signe (int valor, int posicio)
                                                         {
		byteArray[posicio] = (uint8_t)((valor >> 8) & 0xffFF);
		byteArray[posicio + 1] = (uint8_t)(valor & 0xffFF);
	}
void superPacket::setFloat (float floatTmp, int posicio)
                                                       {
		memcpy(this->byteArray + posicio, &floatTmp, 4);
	}
void superPacket::parseBufferHex (char (cadena) [])
                                           {
		char charTmp[2];
		for (int i = 0; i <= (numBytesSuperPacket - 1); i++)
		{
			charTmp[0] = cadena[i * 2];
			charTmp[1] = cadena[(i * 2) + 1];
			this->byteArray[i] = hex8(charTmp);
		}
	}
int superPacket::hex8 (char * in)
        {
		uint8_t c, h;
		c = in[0];

		if (c <= '9' && c >= '0') { c -= '0'; }
		else if (c <= 'f' && c >= 'a') { c -= ('a' - 0x0a); }
		else if (c <= 'F' && c >= 'A') { c -= ('A' - 0x0a); }
		else return(-1);

		h = c;

		c = in[1];

		if (c <= '9' && c >= '0') { c -= '0'; }
		else if (c <= 'f' && c >= 'a') { c -= ('a' - 0x0a); }
		else if (c <= 'F' && c >= 'A') { c -= ('A' - 0x0a); }
		else return(-1);

		return (h << 4 | c);
	}
void superPacket::toHexCharArray (char (cadena) [])
        {
		uint8_t nib1, nib2;
		for (unsigned int i = 0; i < numBytesSuperPacket; i++)
		{
			nib1 = (this->byteArray[i] >> 4) & 0x0F;
			nib2 = (this->byteArray[i] >> 0) & 0x0F;
			cadena[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
			cadena[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
		}
	}
bool superPacket::getBit (uint8_t posicio, uint8_t bit)
                                                  {// retorna el valor d'un bit en una posicio concreta del byte en una posicio del byteArray
		return  0 != (this->byteArray[posicio] & (1 << bit));
	}
void superPacket::setBit (uint8_t posicio, uint8_t bit, bool value)
                                                              {// retorna el valor d'un bit en una posicio concreta del byte en una posicio del byteArray
		if (value) {
			this->byteArray[posicio] |= 1UL << bit;
		}
		else {
			this->byteArray[posicio] &= ~(1UL << bit);
		}
	}
#undef LZZ_INLINE
