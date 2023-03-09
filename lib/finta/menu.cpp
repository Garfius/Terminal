/**
* @file menu.cpp
* @brief here are the menuSystemOverTty manager oject, and the base objects which to inherit to create custom menus,
*
* @author Gerard Forcada Bigas
*
* @date 20/12/22
*/
#include "menu.h"
#ifdef finta_linux
#include <chrono>
#include <thread>
#include <iostream>
#include <cstring>
#include <string.h>
#include <charconv>
#endif
using namespace std;
/**
Menu item base object, inherit for customization
*/
void menuOption::run(){}
//-----------------------------------------------------screen---------------------------
/**
Menu screen base object, inherit for customization

constructor
*/
screen::screen(){
    totalMenuItems = 0;
    whereICame = -1;
}
/**
Static menu add Menu item

@param menuItem pointer to menuItem
*/
void screen::addMenuItem(menuOption* menuItem){
    if(totalMenuItems == maxOpcionsMenu)return;
    menuIoptionsPoint[totalMenuItems] = menuItem;
    totalMenuItems+=1;
}
/**
Push down on the last item

@return want menu refresh
*/
bool screen::pushDn(){return false;}
/**
Push up on the first item

@return want menu refresh
*/
bool screen::pushUp(){return false;}
/**
Called my menu on esc press

@return override go back?
*/
bool screen::esc(){return true;}
//-----------------------------------------------------screenScroll---------------------------
/**
Scroll menu base constructor
*/
screenScroll::screenScroll(){
    index = 0;
    this->totalMenuItems = maxOpcionsMenu;
    refrescaMenu();
}
/**
Push up on the first item

@return True, wants menu refresh
*/
bool screenScroll::pushDn(){
    index++;
    refrescaMenu();
    return true;
}
/**
Push up on the first item

@return True, wants menu refresh
*/
bool screenScroll::pushUp(){
    index--;
    refrescaMenu();
    return true;
}
/**
menu internal refresh
*/
void screenScroll::refrescaMenu(){
    for(int i=0;i < maxOpcionsMenu;i++){
        menuItems[i] = menuOption();
        menuIoptionsPoint[i] = &menuItems[i];
        std::string s = std::to_string(i+index);// scroll debug
        strncpy(menuItems[i].text, s.c_str(), sizeof(menuItems[i].text) );
    }
}
//-----------------------------------------------------menu---------------------------
/**
Base menu constructor
*/
menu::menu(){
    displaySize[0] = displaySize[1] = 0;
    pantallaActiva = -1;
    totalPantalles = 0;
    exit = false;

}
/**
Show menu on screen, using userTty object member

@param resetCursor move cursor to starting point
*/
void menu::show(bool resetCursor){
    if(totalPantalles == 0)return;// safe
    if(this->pantalles[pantallaActiva]->totalMenuItems == 0) return;// safe
    
    if((displaySize[0] == 0)&&(displaySize[1] == 0)){// 1st time ckeck screen size
        this->moveCursor(999,999);
        askDisplaySize();
        //this->userTty->write(' ');?
        #ifdef finta_linux
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        #else
            delay(250);
        #endif
        while(this->userTty->available() != 0){
            charTmp = userTty->read();
            this->doGuess(charTmp);
        }
    }
    this->clearScreen();
    this->moveCursor(1,1);
    this->userTty->write("\e[7m");//invert colors
    this->userTty->print(this->pantalles[pantallaActiva]->titol);
    this->userTty->write("\e[m");//invert colors
    for(int i=0;i< this->pantalles[pantallaActiva]->totalMenuItems;i++){
        this->moveCursor(2+i,1);
        this->userTty->write('-');
        this->userTty->print(pantalles[pantallaActiva]->menuIoptionsPoint[i]->text);
    }
    
    if(resetCursor){
        selectedMenuItem = 0;
    }
    moveCursor(2+selectedMenuItem,1);
}
/**
 * Cursor position report
*/
void menu::askDisplaySize(){//DSR
   this->userTty->write("\e[6n");
}
/**
Clear screen
*/
void menu::clearScreen(){// gives tty command ESC [ 3 J
    this->userTty->write("\e[3J");
}
/**
Cursor Position, inherited from terminalParser
*/
void menu::moveCursor(int line,int column){// Cursor Position
    sprintf(messageBuffer,"\e[%d;%dH",line,column);
    this->userTty->write(messageBuffer);
    /*this->userTty->indexllistaNumeros = 2;
    this->userTty->llistaNumeros[0] = line;
    this->userTty->llistaNumeros[1] = column;
    this->userTty->cUP(this->userTty->indexllistaNumeros,this->userTty->llistaNumeros);*/
}
/**
 * display answers it's size handle
*/
void menu::cPR(unsigned int argc,int *argv){
    if(argc!=2)return;
    displaySize[0] = argv[0];
    displaySize[1] = argv[1];
}
/**
Cursor Up, inherited from terminalParser
*/
void menu::cUU(unsigned int argc,int *argv){// keyUp
    if(selectedMenuItem < 1){
        if(this->pantalles[pantallaActiva]->pushUp())show(false);// scroll
        return;
    }
    selectedMenuItem--;
    moveCursor(2+selectedMenuItem,1);
}
/**
Cursor Down, inherited from terminalParser
*/
void menu::cUD(unsigned int argc,int *argv){// keyDn
    if((selectedMenuItem+1) >= this->pantalles[pantallaActiva]->totalMenuItems){
        if(this->pantalles[pantallaActiva]->pushDn())show(false);// scroll
        return;
    }
    selectedMenuItem++;
    moveCursor(2+selectedMenuItem,1);
}
/**
Esc pressed twice, going back to previous menu
*/
void menu::_esc(){
    if(this->pantalles[pantallaActiva]->esc()){
        if(this->pantalles[pantallaActiva]->whereICame > -1){
            this->pantallaActiva = this->pantalles[pantallaActiva]->whereICame;
            this->show();
        }
    }
}
/**
Carriage return
*/
void menu::cr(){
    //return;
    this->clearScreen();
    pantalles[pantallaActiva]->menuIoptionsPoint[selectedMenuItem]->run();
    /*while(this->userTty->available() == 0){}// intro debug
    charTmp = userTty->read();*/
#ifdef finta_linux
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
#else
    delay(250);
#endif

    this->show();
}
/**
Add available menu screen to the array

@param pantalla pointer to menu screen
*/
void menu::addscreen(screen* pantalla){
    if(totalPantalles == maxOpcionsMenu)return;
    pantalles[totalPantalles] = pantalla;
    totalPantalles+=1;
    if(totalPantalles == 1)pantallaActiva = 0;
}
/**
Gracefully switch menu screen

@param pantallaDesti where to go
*/
void menu::setscreen(int pantallaDesti){
    pantalles[pantallaDesti]->whereICame = pantallaActiva;// la de back del destí és la actual
    pantallaActiva = pantallaDesti;
}
/**
 * @brief Parses the userTty, check user interaction or display query
 * 
 */
void menu::run(){
    if(this->userTty->available() != 0){
        charTmp = userTty->read();
        this->doGuess(charTmp);
        //std::cout << charTmp << std::flush;
        if(charTmp == 'q')this->exit = true;
    }
}

size_t menu::write(unsigned char){return -1;}
int menu::available(){return -1;}
int menu::read(){return -1;}
int menu::peek(){return -1;}
