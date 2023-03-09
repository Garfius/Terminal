#include "menuOptionsL.h"
#include <string.h>
#include <iostream>
// s'han de crear els de Arduino
void fesAlgo::run(){
    std::cout << "fesAlgo" << std::flush;
}
fesAlgo::fesAlgo(){
}
void fesAlgoAltre::run(){
    std::cout << "fesAlgoAltre" << std::flush;
}
fesAlgoAltre::fesAlgoAltre(){
}
void canviaMenuPrincipal::run(){
    this->menuPantalla->setscreen(1);// així es pot retrocedir
}
canviaMenuPrincipal::canviaMenuPrincipal(menu * menuPantalla){
    this->menuPantalla = menuPantalla;
}