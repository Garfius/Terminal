#include "menuOptionsA.h"

// s'han de crear els de Arduino
void fesAlgo::run(){
    digitalWrite(pin, HIGH);
}
fesAlgo::fesAlgo(int pin){
    this->pin = pin;
}

void fesAlgoAltre::run(){
    digitalWrite(pin, LOW);
}
fesAlgoAltre::fesAlgoAltre(int pin){
    this->pin = pin;
}

escriuPerPantalla::escriuPerPantalla(){
}
void escriuPerPantalla::run(){
    this->userTty->write("\e[3J");
    this->userTty->write("\e[1;1H");
    char test = ' ';
    while(test!= 'q'){
        if(this->userTty->available()){
            test = this->userTty->read();
            if(test == 'w')test = '\n';
            this->userTty->write(test);
        }
    }
}

void canviaMenuPrincipal::run(){
    this->menuPantalla->setscreen(1);// així es pot retrocedir
}
canviaMenuPrincipal::canviaMenuPrincipal(menu * menuPantalla){
    this->menuPantalla = menuPantalla;
}
