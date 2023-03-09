#include "terminalParser.h"
#ifdef finta_linux

#else
    //#include <Arduino.h>
#endif
// OutsideOfInternetEncryption -->OOIE Chat / Finta /Private Intellectual Property
#ifndef __menu__
#define __menu__
#define ttyMessageBufferSize 15
#define allargadaTextMenu 25
/**
 * This is base class is to be inherited, put your custom code at run()
*/
class menuOption{
  public:
    char text[allargadaTextMenu];/**< Caption*/
    virtual void run();
};

#define maxOpcionsMenu 4
/**
 * This class represents a static menu display state, inheritable
*/
class screen{
  public: 
    screen();
    void addMenuItem(menuOption* menuItem);
    virtual bool pushDn();
    virtual bool pushUp();
    virtual bool esc();
    char titol[allargadaTextMenu];/**< menu caption*/
    int totalMenuItems;/**< running time total items/options */
    menuOption *menuIoptionsPoint[maxOpcionsMenu];/**< pointers to menu item/option objects*/
    int whereICame; /**<  screen i am from */
};

/**
 * This class represents a scrollable menu display, inheritable
 * 
 * @see menuItems[maxOpcionsMenu]
*/
class screenScroll: public screen{
  public: 
    menuOption menuItems[maxOpcionsMenu];/**< menu item/option objects, refresh for display pruposes*/
    int index;/**< offset for scroll */
    screenScroll();
    void refrescaMenu();
    virtual bool pushDn();
    virtual bool pushUp();
};

#define maxPantalles 3
/**
 * The menu main container
 * 
 * inherits terminalParser to parse keystrokes
*/
class menu : terminalParser{
    char charTmp;/**< temporal character*/
  public:
    char messageBuffer[ttyMessageBufferSize];
    unsigned int displaySize[2];
    menu();
    void run();
    void show(bool resetCursor = true);
    void clearScreen();
    void moveCursor(int line,int column);
    
    
    void cUU(unsigned int argc,int *argv);
    void cUD(unsigned int argc,int *argv);
    void cr();
    void askDisplaySize();/**< cursor position report*/
    void cPR(unsigned int argc,int *argv);/**< display answers it's size handle*/
    void _esc();
    
    void addscreen(screen* pantalla);
    void setscreen(int screenDesti);
    bool exit;/**< no wants more run() */
    int totalPantalles;/**< total amount of displayable/showAble display menus*/
    int pantallaActiva;/**< the actual displayed menu*/
    int selectedMenuItem;/**< the actual selected menu item on display*/
    Stream *userTty;/**< keyboard and display interface */
    screen *pantalles[maxPantalles];/**< pointers to  display menus*/
    
    //------------sobrecàrregues
    
    size_t write(unsigned char);
    int available() ;
    int read();
    int peek();
    
};

#endif