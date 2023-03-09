#include <Arduino.h>

class terminalParser {
        void printa(char caracter);
        bool inControlCaracters(char caracter);
        bool inEscapeSequences(char caracter);
        bool inCsiSequences(char caracter);
        unsigned int i;
        void _parsejaUnNumeroISurt();
        //adafruitTerminalDisplay* pantalla;
    public:
        void _afegeixCaracterNumero(char caracter);
        //terminalParser(adafruitTerminalDisplay* pantallaExt);
        terminalParser();
        bool controlSequence,esc,layer2;
        char caractersNumeros[3];
        int llistaNumeros[4];
        unsigned int indexllistaNumeros,indexCaractersNumeros;
        int ignore;
        void _enterCSI();
        bool doGuess(char caracter);

        static const unsigned char CSI_SEQUENCE_LIST[];
        static const unsigned char CONTROL_CARACTER_LIST[];
        static const unsigned char ESCAPE_SEQUENCE_LIST[];
        
        static void (*moveCursorX)(int[],unsigned int);  // function pointer declaration
        static void (*dECSTBM)(int[],unsigned int);  // function pointer declaration
        static void (*sGR)(int[],unsigned int);  // function pointer declaration
        static void (*rM1)(int[],unsigned int);  // function pointer declaration
        static void (*rM2)(int[],unsigned int);  // function pointer declaration
        static void (*sM)(int[],unsigned int);  // function pointer declaration
        static void (*dA)(int[],unsigned int);  // function pointer declaration
        static void (*cUP)(int[],unsigned int);  // function pointer declaration
        static void (*eD)(int[],unsigned int);  // function pointer declaration
        static void (*vPA)(int[],unsigned int);  // function pointer declaration

        static void (*backSpace)();  // function pointer declaration
        static void (*tab)();  // function pointer declaration
        static void (*lf)();  // function pointer declaration
        static void (*cr)();  // function pointer declaration
        static void (*del)();  // function pointer declaration        
};
//extern terminalParser Parser;