/**
* @file keyboards.cpp
* @brief this header file will contain all required
* definitions and basic utilities functions.
*
* @author Dr.Zhao Zhang
*
* @date 2/28/2009
*/
#include "keyboards.h"
#define CARDKB_ADDR 0x5F


int baseTtyKeyboard::available(){return false;}
int baseTtyKeyboard::peek(){return false;}
int baseTtyKeyboard::read(){return false;}
/**
 * @brief No use? maybe change caps lock or something
 * 
 * @return size_t 
 */
size_t IICTtyKeyboard::write(uint8_t){
  return false;
}
IICTtyKeyboard::IICTtyKeyboard(){
}
int IICTtyKeyboard::_toTty(char caracter){
  if(caracter == 0x00)return caracter;

  if ((caracter < 184 ) && (caracter > 179)) {
    switch(caracter) {
      case 180:
        strcpy(displayAnswer,"\e[D");
        break;
      case 181:
        strcpy(displayAnswer,"\e[A");
        break;
      case 182:
        strcpy(displayAnswer,"\e[B");
        break;
      case 183:
        strcpy(displayAnswer,"\e[C");
        break;
    }
    answerTotal = strlen(displayAnswer);
    answerIndex = 0;
    return displayAnswer[answerIndex];
  }
  else{
   return caracter;
  }
}
int IICTtyKeyboard::available(){
  /*
  Serial2.println("|");
  Serial2.println(digitalRead(p4));
  Serial2.println(digitalRead(p5));
  */
  
  if(answerTotal == 0){// aqui peta, aqui s'ha de sincronitzar Perque mbed::I2C::lock <-- esta BLOQUEJAT EL IIC!!
    Wire.requestFrom(CARDKB_ADDR, 1);
    if(Wire.available() > 0){
      if(Wire.peek() == 0x00){
        Wire.read();
        return 0;
      }
    }
    return Wire.available();
  }
  return (answerTotal-answerIndex)+Wire.available();
}

int IICTtyKeyboard::peek(){
  if(answerTotal != 0){
    return displayAnswer[answerIndex+1];
  }
  //Wire.requestFrom(CARDKB_ADDR, 1);
  return Wire.peek();
}

int IICTtyKeyboard::read(){
  if(answerTotal != 0){
    answerIndex++;
    if(answerTotal == (answerIndex+1)){
      answerTotal = 0;
    }
    return displayAnswer[answerIndex];
  }
  Wire.requestFrom(CARDKB_ADDR, 1);
  return _toTty((char)Wire.read());
}


int PS2TtyKeyboard::available(){
  return false;
}
int PS2TtyKeyboard::peek(){
return false;
}
int PS2TtyKeyboard::read(){
return false;
}


/*
#define CARDKB_ADDR 0x5F
  Wire.requestFrom(CARDKB_ADDR, 1);

  while (Wire.available())
  {
    charFromSerial = Wire.read();
    if ((charFromSerial == 0xFFFFFFB6) || (charFromSerial == 0xFFFFFFB5)) {
      if (charFromSerial == 0xFFFFFFB6) {
        myTerminal.setPage(myTerminal.getpage() + 1);
      }
      else {
        myTerminal.setPage(myTerminal.getpage() - 1);
      }

      charFromSerial = 0x00000000;
    }
    if (charFromSerial != 0x00) {
      myTerminal.escriuCaracter(charFromSerial);
    }
  }
  
*/