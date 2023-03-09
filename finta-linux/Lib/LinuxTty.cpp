#include "LinuxTty.h"
#include <iostream>
#include <termios.h>
#include <string.h>
#include <unistd.h>

using namespace std;

struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

LinuxTty::LinuxTty() :terminalParser(){
    set_conio_terminal_mode();
    peaked = false;
}

// sobrecarregues la funcio que no ha de passar i omples bufferOut <-- opció de teclat, no present en parser, pro si a pantallaAmbTeclat
size_t LinuxTty::write(unsigned char byte,bool flush)
{
    // primer cal fer un doGuess per si cal executar algo k calgui retornar, la funció de pantalla pot omplir buffer teclat
    std::cout << byte ;
    if(flush)std::cout<< std::flush;
    return 1;
}
int LinuxTty::read()// contemplar buffer
{
    if(peaked){
        peaked = false;
    }else if(kbhit()){
        charTmp = getch();
    }else{
        return -1;
    }
    return charTmp;
}
int LinuxTty::peek(){
    if(peaked)return charTmp;
    if(kbhit()){
        charTmp = getch();
        return charTmp;
        peaked = true;
    }
    return -1;
}
int LinuxTty::available(){
    return kbhit();
}
void LinuxTty::eD(unsigned int argc,int *argv){
    if(argc !=1)return;// només accepta 1 argument
    std::cout << "\e["<<argv[0]<< "J" << std::flush;
}
void LinuxTty::cUP(unsigned int argc,int *argv){// es sobrecarrega terminalParser amb la implementació concreta d'aquesta interfície de pantalla
    
    if(argc == 0){
        std::cout << "\e[H";
    }
    std::cout << "\e["<<argv[0]<< ";"<<argv[1] << "H"<< std::flush;
}
int LinuxTty::write(char* holas){

}
int LinuxTty::write(char holas){

}
int LinuxTty::print(char* holas){

}