#include <Arduino.h>
#include "C:\Users\garf\Desktop\arduino\terminal\terminal\lib-disabled\adafruitTerminalDisplay\adafruitTerminalDisplay.cpp"

class terminalParser {
    public:
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
        bool controlSequence,esc,layer2;
        char caractersNumeros[3];
        int llistaNumeros[4];
        char indexllistaNumeros,indexCaractersNumeros;
        int ignore;
          
        terminalParser(adafruitTerminalDisplay* pantalla){
            controlSequence = esc = layer2 = false;
            indexCaractersNumeros = indexllistaNumeros = ignore = 0;
            caractersNumeros[0] = '\0';
        }
        void enterCSI()
        {
            controlSequence = true;
            esc = layer2 = false;
            indexCaractersNumeros = indexllistaNumeros = 0;
            caractersNumeros[0] = '\0';
        }
        void afegeixCaracterNumero(char caracter){
          if(indexCaractersNumeros < sizeof(indexCaractersNumeros)){
            indexCaractersNumeros += 1;
          }
          caractersNumeros[indexCaractersNumeros] = caracter;
        }

        bool belongsToScreen(char caracter){
            while (ignore > 0)
            {
                esc = false;
                ignore--;
                return false;
            }
            
            if (!controlSequence && !esc)
            {
                if (caracter == CSI)// equivalent a (esc)[
                {
                    enterCSI();
                }
                else if (caracter == ESC)
                {
                    esc = true;
                }
                else if (caracter == BACKSPACE)
                {
                    //Console.Write("BACKSPACE");
                }
                else if (caracter == HT)
                {
                    //Console.Write("HT");
                }
                else if (caracter == LF)
                {
                    //Console.Write("LF");
                }
                else if (caracter == CR)
                {
                    //Console.Write("CR");
                }
                else if (caracter == CAN)
                { // ABORT SEQUENCE
                    controlSequence = false;
                }
                else if (caracter == DEL)
                {
                    //Console.Write("DEL");
                }
                else if (caracter == SI)
                {
                    //Console.Write("SI");
                }
                else
                {
                    return true;
                }
            }
            else if (esc)
            {
                if (caracter == ES_CSI)
                {
                    enterCSI();
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
                {
                    esc = false;
                    //Console.Write((char)caracter);
                }
                // to-do si no esta en enum printa caracter
            }
            else if (controlSequence)
            {

                // anar parsejant caracters, assignant valors fins encaixar algun CSI
                if (caracter == CHA)
                {
                    //Console.Write("CHA");
                    controlSequence = false;
                    // to do executar funció, vist amb 1 num
                }
                else if (caracter == DECSTBM)
                {
                    //Console.Write("DECSTBM");
                    afegeixCaracterNumero('\0');
                    llistaNumeros[indexllistaNumeros] = atoi(caractersNumeros);
                    indexllistaNumeros++;
                    indexCaractersNumeros = 0;
                    controlSequence = false;
                    // to-do executa funcio, vist en 2 num
                }
                else if (caracter == SGR)
                {
                    //Console.Write("SGR");
                    //llistaNmumeros.Add(int.Parse(caractersNumeros));
                    indexCaractersNumeros = 0;
                    controlSequence = false;
                    // to-do executa funcio, vist amb 2 num, vist sense numeros, vist amb 3 numeros
                }
                else if (caracter == RM)
                {
                    //Console.Write("RM");
                    afegeixCaracterNumero('\0');
                    llistaNumeros[indexllistaNumeros] = atoi(caractersNumeros);
                    indexllistaNumeros++;
                    indexCaractersNumeros = 0;
                    controlSequence = false;
                    if (!layer2)
                    {//pot ser de capa 2 o 1, vist amb 1 num
                        // to-do executa funcio,
                    }
                }
                else if (caracter == SM)
                {
                    //Console.Write("SM");
                    afegeixCaracterNumero('\0');
                    llistaNumeros[indexllistaNumeros] = atoi(caractersNumeros);
                    indexllistaNumeros++;
                    indexCaractersNumeros = 0;
                    controlSequence = false;
                    // to-do executa funcio? no, és de capa 2
                }
                else if (caracter == DA)
                {
                    //Console.Write("DA");
                    afegeixCaracterNumero('\0');
                    llistaNumeros[indexllistaNumeros] = atoi(caractersNumeros);
                    indexllistaNumeros++;
                    indexCaractersNumeros = 0;
                    controlSequence = false;
                    // to-do executa funcio? no, és de capa 2
                }
                else if (caracter == CUP)
                {
                    //Console.Write("CUP");
                    controlSequence = false;
                    // to-do executar funcio, vist amb 2 num
                }
                else if (caracter == ED)
                {
                    //Console.Write("ED");
                    controlSequence = false;
                }
                else if (caracter == VPA)
                {
                    //Console.Write("VPA");
                    controlSequence = false;
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
                    afegeixCaracterNumero('\0');
                    llistaNumeros[indexllistaNumeros] = atoi(caractersNumeros);
                    indexllistaNumeros++;
                    indexCaractersNumeros = 0;
                }
                else if (!isdigit(caracter))
                { // si no és numero 
                    controlSequence = false;
                }
                else if (isdigit(caracter))
                {
                    afegeixCaracterNumero(caracter);
                }
                else
                {
                    controlSequence = true;
                    esc = false;
                    indexllistaNumeros = 0;
                    indexCaractersNumeros = 0;
                    //Console.Write((char)caracter);
                }
            }
            
            return false;
        }
};

