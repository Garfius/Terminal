#include <Adafruit_SH1106.h>
#include <Arduino.h>
#include <terminalParser_beta.h>
//----------------------------------------functions-------------------------------------------
#define FONT_SIZE 1
#define fontX 7
#define fontY 8
#define cursorBlinkTime 750

#define FONT_SIZE 1

#define saveInterval 10000
//#define M5StackKeyboardIncluded

#ifdef M5StackKeyboardIncluded
#define CARDKB_ADDR 0x5F
#endif // M5StackKeyboardIncluded

//----------------------------------------variables------------------------------------------- al passar a classes moure declaraciío classe amb les seves amigues ara internes
//----------------------------------------functions-------------------------------------------
#define FONT_SIZE 1
#define fontX 7
#define fontY 8
#define cursorBlinkTime 750



class adafruitTerminalDisplay {
public:
  static Adafruit_SH1106* pantalla;
  static terminalParser* parsejador;
  //static const terminalParser* myParser;
  static const int ALCADA = 8;
  static const int AMPLADA = 18;
  static char charArray[AMPLADA][ALCADA];
  static bool escaped;

  static byte currPosX;
  static byte currPosY;

  static bool cursorState, saved, displayed, romEnabled;
  static unsigned long lastCursorBlink;
  static unsigned long lastChange;

  // Adafruit_SH1106* pantalla;
  static const char blank = ' ';
  static unsigned int saveEepromStartIndex;
  //adafruitTerminalDisplay(Adafruit_SH1106* pantallaExt);
  adafruitTerminalDisplay();
  static void init(bool useRom = true);
  
  static void borram();

  static void scrollArray();
  static void crlf();
  static byte lastFilledChar();
  static void doBack();
  static void write(char caracter);
  static void netejaArray();
  static void netejaLinia(int y);
  static void displayArray();
  static void cursorBlink(bool display = true);
  static void cursorBlinkIfNeeded();
  static void saveState();
  static void loadState();
  static void checkSave();
  static int getpage();
  static void setPage(int page);
  static void displayPage();
  static void run();
  static void init();
};

//extern adafruitTerminalDisplay myAdafruitTerminalDisplay;