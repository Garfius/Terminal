#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#define linuxDebug_terminalParser_beta // passat a tasks.json
#include <unistd.h>
#include <termios.h>
#include <future>
#include <thread>
#include <chrono>

//#include <terminalParser_beta.h>
#include "LinuxTty.h"

#include <fstream>
// sudo mount -t drvfs g: /mnt/g -o metadata
// https://wokwi.com/projects/346935389841261140
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

//LinuxTty linuxTty = LinuxTty();
terminalParser parserTest = terminalParser();
char aaa;
int main(int argc, char *argv[])
{
    ifstream f("/home/garfius/volcatSerie.bin");
    
    char x1;
    f.seekg(0);
    while(f.get(x1))
    {
        if(parserTest.doGuess(x1)){
            std::cout << x1;
        }
    }

    printf("boot\n");
    set_conio_terminal_mode();
    while (true) {
        if(kbhit()){
            aaa = getch();
            std::cout << aaa<< std::flush;
        }
    }
    
   exit(EXIT_SUCCESS);
}