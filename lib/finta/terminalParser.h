#ifndef __terminalParser__
#define __terminalParser__
#ifdef finta_linux
    #include <cstddef>
    #include "stream.h"
#else
    #include <Arduino.h>
#endif
#define terminalParser_bufferSize 10
/**
 * 1 keystroke keys
*/
enum CONTROL_CARACTERS
{
    BEEP = 0x07,
    BACKSPACE = 0x08,
    HT = 0x09,
    LF = 0x0A,
    CR = 0x0D,
    SO = 0x0E,
    SI = 0x0F,
    CAN = 0x18,
    ESC = 0x1B,
    DEL = 0x7F,
    CSI = 0x9B
};
/**
 * which kind of escape are we going to?
*/
enum ESCAPE_SEQUENCES
{
    RIS = 'c',
    IND = 'D',
    NEL = 'E',
    HTS = 'H',
    RI = 'M',
    DECID = 'Z',
    DECSC = '7',
    DECRC = '8',
    ES_CSI = '[',
    DECPNM = '>',
    DECPAM = '=',
    OSC = ']', // COMANDA s.o.
    selecting_character_set = '%', //<-- ignora 1 de més
    DECALN = '#', //<-- ignora 1 de més
    STARTG0 = '(', //<-- ignora 1 de més
    STARTG1 = ')', //<-- ignora 1 de més
};
/**
 * finalization escape sequence command
*/
enum CSI_SEQUENCES
{
    ICH = '@', //Insert the indicated # of blank characters.
    
    CUU = 'A', //Move cursor up the indicated # of rows.
    CUD = 'B', //Move cursor down the indicated # of rows.
    CUF = 'C', //Move cursor right the indicated # of columns.
    CUB = 'D', //Move cursor left the indicated # of columns.

    CNL = 'E', //Move cursor down the indicated # of rows, to column 1
    CPL = 'F', //Move cursor up the indicated # of rows, to column                     1.
    CHA = 'G', // Move cursor to indicated column in current row.
    CUP = 'H', //Move cursor to the indicated row, column (origin at 1,1).
    ED = 'J', //  ESC [ 1 J: erase from start to cursor.ESC [ 2 J: erase whole display.ESC [ 3 J: erase whole display including scroll- && \033[J clears the part of the screen from the cursor to the end of the screen.
    EL = 'K', //ESC [ 1 K: erase from start of line to cursor.                     ESC [ 2 K: erase whole line.
    IL = 'L',//Insert the indicated # of blank lines, 
    DL = 'M', //Delete  the indicated # of blank lines
    DCH = 'P', // Delete the indicated # of characters on current                     line
    ECH = 'X', // Erase the indicated # of characters on current                     line
    HPR = 'a', //Move cursor right the indicated # of columns, horizontal position relative
    DA = 'c', //Answer ESC [ ? 6 c: "I am a VT102"
    VPA = 'd', //Move cursor to the indicated row, current column.
    VPR = 'e', // Move cursor down the indicated # of rows. vertical position relative
    HVP = 'f', // Move cursor to the indicated row, column.
    TBC = 'g', //Without parameter: clear tab stop at current                     position.ESC [ 3 g: delete all tab stops.
    SM = 'h', //Set Mode (see below).
    /*the Reset Mode sequences are obtained by replacing the final 'h' by 'l'.*/
    RM = 'l', //Reset Mode, 
    SGR = 'm', //Set attributes
    DSR = 'n', // Status report 
    CPR = 'R', // Status report 
    DECLL = 'q', //Set keyboard LEDs.                     ESC [ 0 q: clear all LEDs                     ESC [ 1 q: set Scroll Lock LED                     ESC [ 2 q: set Num Lock LED                     ESC [ 3 q: set Caps Lock LED
    DECSTBM = 'r', //Set scrolling region; parameters are top and bottom                     row.
    Save_cursor_location = 's',
    Restore_cursor_location = 'u',
    HPA = '`', // Move cursor to indicated column in current row.
    LINUX_PRIVATE_CONTROL = ']', //Linux Console Private CSI Sequences
    PRIVATE_CONTROL_SEQUENCE = '?' //Linux Console Private CSI Sequences
};
/**
 * Base class which processes a stream, calling Ansi escape codes named function
*/
class terminalParser : public Stream {


        void _printa(char caracter);
        bool _inControlCaracters(char caracter);
        bool _inEscapeSequences(char caracter);
        bool _inCsiSequences(char caracter);
        unsigned int i;
        void _parsejaUnNumero(bool surt = true);
        void _afegeixCaracterNumero(char caracter);// funcions internes
        void _enterCSI();
        void _surt();
        /*bool _executa(void (terminalParser::* func)());
        bool _executaArgs(void (* func)(unsigned int argc,int **argv));*/
    public:
        static const unsigned char CSI_SEQUENCE_LIST[];/**< internal list for iteration*/
        static const unsigned char CONTROL_CARACTER_LIST[];/**< internal list for iteration*/
        static const unsigned char ESCAPE_SEQUENCE_LIST[];/**< internal list for iteration*/
        unsigned int indexllistaNumeros;/**< used for argc */
        unsigned int indexCaractersNumeros;/**< char array index for parsing numbers*/
        unsigned int indexBuffer;/**< buffer char array index*/
        bool controlSequence;/**< internal state*/
        bool esc;/**< internal state, esc pressed*/
        bool privateControlSequence;/**< internal state*/
        char lastTtyCommand[terminalParser_bufferSize];// per poder fer forwarding i anàlisi bugs
        char caractersNumeros[4];/**< buffer to parse number sequences*/
        int llistaNumeros[4];/**< used for argv */
        int ignore;/**< amount of characters to ignore*/

        
        terminalParser();
        bool doGuess(char caracter);

//-----------------------------------capa tty escape codes--------------------------------
        virtual void tab();/**< tabulador*/
        virtual void _esc();/**< double escape*/
        virtual void lf();/**< line forward*/
        virtual void cr();/**< catrriage return*/
        virtual void del();/**< delete pressed*/
        virtual void backSpace();/**< backspace pressed*/
        virtual void beep();        
        virtual void cHA(unsigned int argc,int *argv); /**< CHA escape command @see cUP */
        virtual void cUU(unsigned int argc,int *argv);/**< key up*/
        virtual void cUD(unsigned int argc,int *argv);/**< key down*/
        virtual void cUF(unsigned int argc,int *argv);/**< key forward... right*/
        virtual void cUB(unsigned int argc,int *argv);/**< key backward... left*/
        virtual void eL(unsigned int argc,int *argv);
        //virtual void dECSTBM(unsigned int argc,int *argv);/**< escape command*/
        virtual void sGR(unsigned int argc,int *argv);/**< select graphic rendition*/
        virtual void dSR(unsigned int argc,int *argv);/**< select graphic rendition*/
        //virtual void cPR(unsigned int argc,int *argv);/**< select graphic rendition*/
        virtual void rM(unsigned int argc,int *argv);/**< escape command*/
        virtual void sM(unsigned int argc,int *argv);/**< escape command*/
        virtual void dA(unsigned int argc,int *argv);/**< escape command*/
        virtual void cUP(unsigned int argc,int *argv);/**< cursor position */
        virtual void eD(unsigned int argc,int *argv);/**< escape command*/
        virtual void vPA(unsigned int argc,int *argv);/**< escape command*/
        virtual void dCH(unsigned int argc,int *argv);/**< escape command*/
        
//-----------------------------------sobrecarregades stream?  --------------------------------
        //size_t write(const uint8_t *buffer, size_t size);
        //virtual size_t write(uint8_t c);
        //virtual size_t print(const char c[]);
        /*size_t write(const char *str) {
         size_t print(const char str[])*/
        //virtual int available();
};

#endif
/*
extern terminalParser Parser;
extern terminalParser myParsedTerminal;
*/
