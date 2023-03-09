#include <terminalParser_beta.h>
//#define linuxDebug_terminalParser_beta
#include <stdlib.h>

#ifdef linuxDebug_terminalParser_beta
#include <stdio.h>
#include <cctype>
#include <iostream>
#endif

enum CONTROL_CARACTERS
{
    BEEP = 0x07,
    BACKSPACE = 0x08,
    HT = 0x09,
    LF = 0x0A,
    CR = 0x0D,
/*    SO = 0x0E,
    SI = 0x0F,*/
    CAN = 0x18,
    ESC = 0x1B,
    DEL = 0x7F,
    CSI = 0x9B
};
enum ESCAPE_SEQUENCES
{//ctrl dret = 1B4F43 ctrl esq = 1B4F44
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
enum CSI_SEQUENCES
{// insert = 1B5B327E
    ICH = '@', //Insert the indicated # of blank characters.
    CUU = 'A', //Move cursor up the indicated # of rows.
    CUD = 'B', //Move cursor down the indicated # of rows.
    CUF = 'C', //Move cursor right the indicated # of columns.
    CUB = 'D', //Move cursor left the indicated # of columns.
    CNL = 'E', //Move cursor down the indicated # of rows, to column 1
    CPL = 'F', //Move cursor up the indicated # of rows, to column                     1.
    CHA = 'G', // Move cursor to indicated column in current row.
    CUP = 'H', //Move cursor to the indicated row, column (origin at                     1,1).
    ED = 'J', //  ESC [ 1 J: erase from start to cursor.                     ESC [ 2 J: erase whole display.                     ESC [ 3 J: erase whole display including scroll-
    EL = 'K', //ESC [ 1 K: erase from start of line to cursor.                     ESC [ 2 K: erase whole line.
    IL = 'L',//Insert the indicated # of blank lines, 
    DL = 'M', //Delete  the indicated # of blank lines
    DCH = 'P', // Delete the indicated # of characters on current                     line
    ECH = 'X', // Erase the indicated # of characters on current                     line
    HPR = 'a', //Move cursor right the indicated # of columns
    DA = 'c', //Answer ESC [ ? 6 c: "I am a VT102"
    VPA = 'd', //Move cursor to the indicated row, current column.
    VPR = 'e', // Move cursor down the indicated # of rows.
    HVP = 'f', // Move cursor to the indicated row, column.
    TBC = 'g', //Without parameter: clear tab stop at current                     position.ESC [ 3 g: delete all tab stops.
    SM = 'h', //Set Mode (see below).
    /*the Reset Mode sequences are obtained by replacing the final 'h' by 'l'.*/
    RM = 'l', //Reset Mode, 
    SGR = 'm', //Set attributes
    DSR = 'n', // Status report 
    DECLL = 'q', //Set keyboard LEDs.                     ESC [ 0 q: clear all LEDs                     ESC [ 1 q: set Scroll Lock LED                     ESC [ 2 q: set Num Lock LED                     ESC [ 3 q: set Caps Lock LED
    DECSTBM = 'r', //Set scrolling region; parameters are top and bottom                     row.
    Save_cursor_location = 's',
    Restore_cursor_location = 'u',
    HPA = '`', // Move cursor to indicated column in current row.
    LINUX_PRIVATE_CONTROL = ']', //Linux Console Private CSI Sequences
    PRIVATE_CONTROL_SEQUENCE = '?' //Linux Console Private CSI Sequences
};
/*
Coses falten per parsejar
insert = 1B5B327E
ctrl dret = 1B4F43 
ctrl esq = 1B4F44
*/
const unsigned char terminalParser::CONTROL_CARACTER_LIST[]{
    BEEP,		//  0x07
    BACKSPACE,//  0x08
    HT,		//  0x09
    LF,		//  0x0A
    CR,		//  0x0D
/*    SO,		//  0x0E
    SI,		//  0x0F*/
    CAN,	//  0x18
    ESC,	//  0x1B
    DEL,	//  0x7F
    CSI,	//  0x9B
};
const unsigned char terminalParser::ESCAPE_SEQUENCE_LIST[]{
    RIS,	  	//  'c'
    IND,	  	//  'D'
    NEL,	  	//  'E'
    HTS,	  	//  'H'
    RI,	    	//  'M'
    DECID,		//  'Z'
    DECSC,		//  '7'
    DECRC,		//  '8'
    ES_CSI,		//  '['
    DECPNM,		//  '>'
    DECPAM,		//  '='  
    OSC,      // ']'  -  COMANDA s.o.
    selecting_character_set,// '%'  - <-- ignora 1 de més
    DECALN,		// '#'  - <-- ignora 1 de més
    STARTG0,	// '('  - <-- ignora 1 de més
    STARTG1   // ')'  - <-- ignora 1 de més
};

const unsigned char terminalParser::CSI_SEQUENCE_LIST[]={
    ICH,		// '@'  - Insert the indicated # of blank characters.
    CUU,		// 'A'  - Move cursor up the indicated # of rows.
    CUD,		// 'B'  - Move cursor down the indicated # of rows.
    CUF,		// 'C'  - Move cursor right the indicated # of columns.
    CUB,		// 'D'  - Move cursor left the indicated # of columns.
    CNL,		// 'E'  - Move cursor down the indicated # of rows, to column 1
    CPL,		// 'F'  - Move cursor up the indicated # of rows, to column                     1.
    CHA,		// 'G'  -  Move cursor to indicated column in current row.
    CUP,		// 'H'  - Move cursor to the indicated row, column (origin at                     1,1).
    ED, 		// 'J'  -   ESC [ 1 J: erase from start to cursor.                     ESC [ 2 J: erase whole display.                     ESC [ 3 J: erase whole display including scroll-
    EL, 		// 'K'  - ESC [ 1 K: erase from start of line to cursor.                     ESC [ 2 K: erase whole line.
    IL, 		// 'L' - Insert the indicated # of blank lines, 
    DL, 		// 'M'  - Delete  the indicated # of blank lines
    DCH,		// 'P'  -  Delete the indicated # of characters on current                     line
    ECH,		// 'X'  -  Erase the indicated # of characters on current                     line
    HPR,		// 'a'  - Move cursor right the indicated # of columns
    DA,	  	// 'c'  - Answer ESC [ ? 6 c: "I am a VT102"
    VPA,		// 'd'  - Move cursor to the indicated row, current column.
    VPR,		// 'e'  -  Move cursor down the indicated # of rows.
    HVP,		// 'f'  -  Move cursor to the indicated row, column.
    TBC,		// 'g'  - Without parameter: clear tab stop at current                     position.ESC [ 3 g: delete all tab stops.
    SM,	  	// 'h'  - Set Mode (see below).
    /*the Reset Mode sequences are obtained by replacing the final 'h' by 'l'.*/
    RM,	  	// 'l'  - Reset Mode, 
    SGR,		// 'm'  - Set attributes
    DSR,		// 'n'  -  Status report 
    DECLL,	// 'q'  - Set keyboard LEDs.                     ESC [ 0 q: clear all LEDs                     ESC [ 1 q: set Scroll Lock LED                     ESC [ 2 q: set Num Lock LED                     ESC [ 3 q: set Caps Lock LED
    DECSTBM,// 'r'  - Set scrolling region; parameters are top and bottom                     row.
    Save_cursor_location,//  's'
    Restore_cursor_location,//  'u'
    HPA,		// '`'  -  Move cursor to indicated column in current row.
    LINUX_PRIVATE_CONTROL,// ']'  - Linux Console Private CSI Sequences
    PRIVATE_CONTROL_SEQUENCE,// '?'  - Linux Console Private CSI Sequences
};
    
//terminalParser::terminalParser(adafruitTerminalDisplay* pantallaExt){
    terminalParser::terminalParser(){
    //this->pantalla = pantallaExt;
    controlSequence = esc = layer2 = false;
    indexCaractersNumeros = indexllistaNumeros = ignore = 0;
    caractersNumeros[0] = '\0';
}
void terminalParser::_enterCSI()
{
    controlSequence = true;
    esc = layer2 = false;
    indexCaractersNumeros = indexllistaNumeros = 0;
    caractersNumeros[0] = '\0';
}
void terminalParser::_parsejaUnNumeroISurt(){
    _afegeixCaracterNumero('\0');
    llistaNumeros[indexllistaNumeros] = atoi(caractersNumeros);
    indexllistaNumeros++;
    indexCaractersNumeros = 0;
    controlSequence = false;
}
void terminalParser::_afegeixCaracterNumero(char caracter){
    if(indexCaractersNumeros < sizeof(indexCaractersNumeros)){
    indexCaractersNumeros += 1;
    }
    caractersNumeros[indexCaractersNumeros] = caracter;
}
bool terminalParser::inControlCaracters(char caracter){
    for(i = 0; i < sizeof(CONTROL_CARACTER_LIST); i++){
        if(CONTROL_CARACTER_LIST[i] == caracter){
            return true;
        }
    }
    return false;
}
bool terminalParser::inEscapeSequences(char caracter){
    for(i = 0; i < sizeof(ESCAPE_SEQUENCE_LIST); i++){
        if(ESCAPE_SEQUENCE_LIST[i] == caracter){
            return true;
        }
    }
    return false;
}
bool terminalParser::inCsiSequences(char caracter){
    for(i = 0; i < sizeof(CSI_SEQUENCE_LIST); i++){
        if(CSI_SEQUENCE_LIST[i] == caracter){
            return true;
        }
    }
    return false;
}

bool terminalParser::doGuess(char caracter){// do the think you pointed at, and guess if shall be screened
    while (ignore > 0)
    {
        esc = false;
        ignore--;
        return;
    }
    
    if (!controlSequence && !esc && (this->inControlCaracters(caracter)))
    {

        if (caracter == CSI)// equivalent a (esc)[
        {
            _enterCSI();
        }
        else if (caracter == ESC)
        {
            esc = true;
        }
        else if (caracter == BACKSPACE)
        {
            if(this->backSpace != nullptr)this->backSpace();
        }
        else if (caracter == HT)
        {
            if(this->tab != nullptr)this->tab();
        }
        else if (caracter == LF)
        {
            if(this->lf != nullptr)this->lf();
        }
        else if (caracter == CR)
        {//
            if(this->cr != nullptr)this->cr();
        }
        else if (caracter == CAN)
        { // ABORT SEQUENCE
            controlSequence = false;
        }
        else if (caracter == DEL)
        {
            if(this->del != nullptr)this->del();
        }
        else
        {// els no contemplats
            esc = false;
            this->printa(caracter);
            return;
        }
    }
    else if (esc && (this->inEscapeSequences(caracter)))
    {
        if (caracter == ES_CSI)
        {
            _enterCSI();
        }
        else if (caracter == selecting_character_set)
        {
            ignore = 1;
        }
        else if (caracter == STARTG0)
        {
            ignore = 1;
        }
        else if (caracter == STARTG1)
        {
            ignore = 1;
        }
        else if (caracter == DECALN)
        {
            ignore = 1;
        }
        else if (caracter == OSC)
        {
            /*no hi ha sistema operatiu, no es parseja, coses de clipboard, notificacions, window title https://blog.vucica.net/2017/07/what-are-osc-terminal-control-sequences-escape-codes.html
                */
            //Serial.println("OSC-WTF");
            esc = false;
        }
        else
        {// els no contemplats?¿
            esc = false;
            //Console.Write((char)caracter);
        }
        // to-do si no esta en enum printa caracter
    }
    else if (controlSequence && (this->inCsiSequences(caracter)))
    {
        //to-do posar amunt, avall dreta i esquerra A B C D
        // anar parsejant caracters, assignant valors fins encaixar algun CSI
        if (caracter == CHA)// Move cursor to indicated column in current row.
        {
            controlSequence = false;
            if(this->moveCursorX != nullptr)this->moveCursorX(llistaNumeros,indexllistaNumeros);// vist amb 1 num
        }
        else if (caracter == DECSTBM)//Set scrolling region; parameters are top and bottom row.
        {
            _parsejaUnNumeroISurt();
            if(this->dECSTBM != nullptr)this->dECSTBM(llistaNumeros,indexllistaNumeros);// vist amb 1 num// to-do executa funcio, vist en 2 num
        }
        else if (caracter == SGR)//Set SELECT_GRAPHIC_RENDITION           '0' = RESET.           '10' = DEFAULT_FONT.           '7' = INVERSE.
        {// fer color invers
            _parsejaUnNumeroISurt();
            if(this->sGR != nullptr)this->sGR(llistaNumeros,indexllistaNumeros);// vist amb 1 num// to-do executa funcio, vist en 2 num// to-do executa funcio, vist amb 2 num, vist sense numeros, vist amb 3 numeros
        }
        else if (caracter == RM)
        {
            _parsejaUnNumeroISurt();
            if (!layer2)
            {//pot ser de capa 2 o 1, vist amb 1 num
                // to-do executa funcio,
                if(this->rM1 != nullptr)this->rM1(llistaNumeros,indexllistaNumeros);// vist amb 1 num// to-do executa funcio, vist en 2 num// to-do executa funcio, vist amb 2 num, vist sense numeros, vist amb 3 numeros
            }
            if(this->rM2 != nullptr)this->rM2(llistaNumeros,indexllistaNumeros);// vist amb 1 num// to-do executa funcio, vist en 2 num// to-do executa funcio, vist amb 2 num, vist sense numeros, vist amb 3 numeros
        }
        else if (caracter == SM)
        {
            _parsejaUnNumeroISurt();
            if(this->sM != nullptr)this->sM(llistaNumeros,indexllistaNumeros);// to-do executa funcio? no, és de capa 2
        }
        else if (caracter == DA)
        {
            _parsejaUnNumeroISurt();
            if(this->dA != nullptr)this->dA(llistaNumeros,indexllistaNumeros);// to-do executa funcio? no, és de capa 2
            // to-do executa funcio? no, és de capa 2
        }
        else if (caracter == CUP)
        {
            _parsejaUnNumeroISurt();
            if(this->cUP != nullptr)this->cUP(llistaNumeros,indexllistaNumeros);// to-do executa funcio? no, és de capa 2
            // to-do executar funcio, vist amb 2 num
        }
        else if (caracter == ED)
        {
            _parsejaUnNumeroISurt();
            if(this->eD != nullptr)this->eD(llistaNumeros,indexllistaNumeros);// to-do executa funcio? no, és de capa 
        }
        else if (caracter == VPA)
        {
            _parsejaUnNumeroISurt();
            if(this->vPA != nullptr)this->vPA(llistaNumeros,indexllistaNumeros);// to-do executa funcio? no, és de capa 
            // to do executar funció, vist amb 1 num
        }
        else if (caracter == PRIVATE_CONTROL_SEQUENCE) // entra a capa 2
        {
            //Console.Write("PCS");
            controlSequence = true;
            indexllistaNumeros = 0;
            indexCaractersNumeros = 0;
            layer2 = true;
            esc = false;
        }
        else if (caracter == 0x3b)// ; 
        {// toca parsejar numero
            _parsejaUnNumeroISurt();
        }
        else if (!isDigit(caracter))// arduino caps linux no caps
        { // si no és numero 
            controlSequence = false;
        }
        else if (isDigit(caracter))
        {
            _afegeixCaracterNumero(caracter);
        }
        else
        {// els no contemplats?
            controlSequence = true;
            esc = false;
            indexllistaNumeros = 0;
            indexCaractersNumeros = 0;
            this->printa( caracter);
        }
    }
}

#ifdef linuxDebug_terminalParser_beta
void terminalParser::printa(char caracter){
    printf("%c",caracter);
}
#else
void terminalParser::printa(char caracter){
    Serial.print(caracter);
}
#endif
//void (terminalParser::*backSpace) () = nullptr;  
//void terminalParser::*backSpace = nullptr;cannot declare pointer to 'void' member
void (*backSpace)() = NULL;