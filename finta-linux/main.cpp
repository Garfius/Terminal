#include <stdio.h>
#include <iostream>

#include <string.h>
#include <LinuxTty.h>
#include "menu.h"
#include "menuOptionsL.h"
#include <fstream>
using namespace std;

// crea menu
LinuxTty entradaSortida = LinuxTty();
menu elMeuMenu = menu();
// crea pantalles
screen pantalla1 = screen();
screen pantalla2 = screen();
screenScroll pantalla3 = screenScroll();
//---------opcions menu
fesAlgo opciomenu1 = fesAlgo();
fesAlgoAltre opciomenu2 = fesAlgoAltre();
canviaMenuPrincipal opcioForta = canviaMenuPrincipal(&elMeuMenu);

fesAlgo caca;
/*
    to-do
        executar opcio menu
        diferents pantalles de menú
*/

int main ( void )
{
    std::cout << "toca'm els ous" << std::endl;
  /* 
    //
    std::cout << BWbitWise << std::endl;
    
    std::cout << std::hex << '\a' << std::endl << std::flush;

    for (int i = 0; i <= AMPLADA; i++) {
        for (int y = 0; y <= ALCADA; y++) {
            setinvertedColor(i,y,true);
        }
    }
    for (int i = 0; i <= AMPLADA; i++) {
        for (int y = 0; y <= ALCADA; y++) {
            std::cout << std::hex << getinvertedColor(i,y) << std::endl << std::flush;
            
        }
    }
        
    std::cout << std::hex << (int)(uint8_t )bitWiseBW[0] << std::endl << std::flush;
*/
    strncpy(pantalla1.titol, "Menu 1", sizeof(pantalla1.titol) );
    strncpy(pantalla2.titol, "Menu 2", sizeof(pantalla2.titol) );
    strncpy(pantalla3.titol, "Menu 3", sizeof(pantalla3.titol) );

    strncpy(opciomenu1.text, "opcio1", sizeof(opciomenu1.text) );
    strncpy(opciomenu2.text, "opcio 2", sizeof(opciomenu2.text) );
    strncpy(opcioForta.text, "canvi menu?", sizeof(opcioForta.text) );

    elMeuMenu.addscreen(&pantalla1);
    elMeuMenu.addscreen(&pantalla3);

    pantalla1.addMenuItem(&opcioForta);
    pantalla1.addMenuItem(&opciomenu1);
    pantalla2.addMenuItem(&opciomenu2);

    elMeuMenu.userTty = &entradaSortida;// posa pantalla al menu

    //strncpy(elMeuMenu.titol, "Kickass menu", sizeof(opciomenu1.text) );
    
    
    elMeuMenu.show();
    while(!elMeuMenu.exit){
        elMeuMenu.run();
    }

    std::cout << "adeu\n" << std::flush;
#ifdef finta_linux
std::cout << "funciona\n" << std::flush;
#endif
    
    return 0;
}
/*
void testTty(){
    int tmp,tmp2 =0;
    ifstream f("/home/garfius/volcatSerie.bin",ios::binary);
    
    char x1;
    f.seekg(0);
    while(f.get(x1))
    {
        tmp2 = f.tellg();
        tmp++;
        if((tmp%10) ==0){
            tmp = tmp;
        }
        if(entradaSortida.doGuess(x1)){
            std::cout << x1 << std::flush;
        }
    }

}
*/