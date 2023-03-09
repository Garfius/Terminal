#include <SPI.h>
#include <Arduino.h>

#include <Wire.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <SimpleTimer.h>
#include <keyboards.h>
#include <menuOptionsA.h>
#include <Adafruit_SH1106_kbv.h>
#include <Adafruit_ST7735.h>

IICTtyKeyboard TTYS0_Keyboard = IICTtyKeyboard();// keyboard is not as an argument bc at construction, Wire.begin() fails

TFT_eSPI tft = TFT_eSPI();
SimpleTimer timer;

// Keyboard control 5V voltage
#define KBD_DAT  PB7
#define KBD_CLK  PB6

// Speaker control
#define SPK_PIN  PA8

// LED Controlling
/*
#define LED_01  PB15
#define LED_02  PB14
#define LED_03  PB13
#define LED_04  PB12
*/

// Default communication speed
#define DEFAULT_BAUDRATE 1200

// Keyboard control
//PS2Keyboard keyboard;

// for font management
uint8_t* fontTop;
#define CH_W 5 // horizontal font size
#define CH_H 8 // vertical font size

// for screen management ILI9488=320*480
#define SC_W 80 // character screen horizontal size 80*5=400
#define SC_H 24 // character screen vertical size 24*8 =192
uint16_t M_TOP = 0; // top margin line
uint16_t M_BOTTOM = SC_H - 1; // bottom margin line

// precalculate coordinates and sizes
PROGMEM const uint16_t SCSIZE = SC_W * SC_H; // character screen size
PROGMEM const uint16_t SP_W = SC_W * CH_W; // pixel screen horizontal size
PROGMEM const uint16_t SP_H = SC_H * CH_H; // pixel screen vertical size
PROGMEM const uint16_t MAX_CH_X = CH_W - 1; // font maximum horizontal position
PROGMEM const uint16_t MAX_CH_Y = CH_H - 1; // font maximum vertical position
PROGMEM const uint16_t MAX_SC_X = SC_W - 1; // character screen maximum horizontal position
PROGMEM const uint16_t MAX_SC_Y = SC_H - 1; // character screen maximum vertical position
PROGMEM const uint16_t MAX_SP_X = SP_W - 1; // pixel screen maximum horizontal position
PROGMEM const uint16_t MAX_SP_Y = SP_H - 1; // pixel screen maximum vertical position

// for text attributes
struct TATTR {
  uint8_t Bold  : 1;      // 1
  uint8_t Faint  : 1;     // 2
  uint8_t Italic : 1;     // 3
  uint8_t Underline : 1;  // 4
  uint8_t Blink : 1;      // 5 (Slow Blink)
  uint8_t RapidBlink : 1; // 6
  uint8_t Reverse : 1;    // 7
  uint8_t Conceal : 1;    // 8
};

union ATTR {
  uint8_t value;
  struct TATTR Bits;
};

//int colors[totalColors] = {TFT_BLACK,TFT_RED,TFT_GREEN,TFT_YELLOW,TFT_BLUE,TFT_MAGENTA,TFT_CYAN,TFT_WHITE};// girar blau grana
// for color attributes (RGB565)
PROGMEM const uint16_t aColors[] = {
  // Normal (0..7)
  TFT_BLACK, // black
  TFT_RED, // red
  TFT_GREEN, // green
  TFT_YELLOW, // yellow
  TFT_BLUE, // blue (Dark)
  TFT_MAGENTA, // magenta
  TFT_CYAN, // cyan
  TFT_WHITE, // black
  // Bright (8..15) // to-do 
  TFT_WHITE, // white
  TFT_PINK, // red
  TFT_OLIVE, // green
  TFT_VIOLET, // yellow
  TFT_LIGHTGREY, // blue
  TFT_MAGENTA, // magenta
  TFT_BLUE, // cyan
  TFT_WHITE  // white
};
char backGroundChar = ' ';

PROGMEM const uint8_t clBlack = 0;
PROGMEM const uint8_t clRed = 1;
PROGMEM const uint8_t clGreen = 2;
PROGMEM const uint8_t clYellow = 3;
PROGMEM const uint8_t clBlue = 4;
PROGMEM const uint8_t clMagenta = 5;
PROGMEM const uint8_t clCyan = 6;
PROGMEM const uint8_t clWhite = 7;

struct TCOLOR {
  uint8_t Foreground : 4;
  uint8_t Background : 4;
};
union COLOR {
  uint8_t value;
  struct TCOLOR Color;
};

// for configuration
struct TMODE {
  bool Reserved2 : 1;  // 2
  bool Reserved4 : 1;  // 4:
  bool Reserved12 : 1; // 12:
  bool CrLf : 1;       // 20: LNM (Line feed new line mode)
  bool Reserved33 : 1; // 33:
  bool Reserved34 : 1; // 34:
  uint8_t Reverse : 2;    
};

union MODE {
  uint8_t value;
  struct TMODE Flgs;
};

struct TMODE_EX {
  bool Reserved1 : 1;     // 1 DECCKM (Cursor Keys Mode)
  bool Reserved2 : 1;     // 2 DECANM (ANSI/VT52 Mode)
  bool Reserved3 : 1;     // 3 DECCOLM (Column Mode)
  bool Reserved4 : 1;     // 4 DECSCLM (Scrolling Mode)
  bool ScreenReverse : 1; // 5 DECSCNM (Screen Mode)
  bool Reserved6 : 1;     // 6 DECOM (Origin Mode)
  bool WrapLine  : 1;     // 7 DECAWM (Autowrap Mode)
  bool Reserved8 : 1;     // 8 DECARM (Auto Repeat Mode)
  bool Reserved9 : 1;     // 9 DECINLM (Interlace Mode)
  uint16_t Reverse : 7;    
};

union MODE_EX {
  uint16_t value;
  struct TMODE_EX Flgs;
};

// buffer
uint8_t screen[SCSIZE]; // screen buffer
uint8_t attrib[SCSIZE]; // character attribute buffer
uint8_t colors[SCSIZE]; // color attribute buffer
uint8_t tabs[SC_W]; // tab position buffer

// 状態
PROGMEM enum class em {NONE,  ES, CSI, CSI2, LSC, G0S, G1S};
PROGMEM uint8_t defaultMode = 0b00001000;
PROGMEM uint8_t defaultModeEx = 0b0000000001000000;
PROGMEM const union ATTR defaultAttr = {0b00000000};
PROGMEM const union COLOR defaultColor = {(clBlack << 4) | clWhite}; // back, fore
em escMode = em::NONE;         // エスケープシーケンスモード
bool isShowCursor = false;     // カーソル表示中か？
bool canShowCursor = false;    // カーソル表示可能か？
bool lastShowCursor = false;   // 前回のカーソル表示状態
bool hasParam = false;         // <ESC> [ がパラメータを持っているか？
bool isDECPrivateMode = false; // DEC Private Mode (<ESC> [ ?)
union MODE mode = {defaultMode};
union MODE_EX mode_ex = {defaultModeEx};

// previous position information
int16_t p_XP = 0;
int16_t p_YP = 0;

// current information
int16_t XP = 0;
int16_t YP = 0;
union ATTR cAttr   = defaultAttr;
union COLOR cColor = defaultColor;

// バックアップ情報
int16_t b_XP = 0;
int16_t b_YP = 0;
union ATTR bAttr   = defaultAttr;
union COLOR bColor = defaultColor;

// CSI パラメータ
int16_t nVals = 0;
int16_t vals[10] = {0};

// function
// -----------------------------------------------------------------------------
void swapInt(uint16_t*a, uint16_t *b) {
   uint16_t c = *a;
   *a = *b;
   *b = c;
}
/**
 * @brief Set the Tft Window object from absolute to relative
 * 
 */
void setTftWindow(int x , int y, int x0, int y0){
  tft.setAddrWindow(x, y, x0-x,y0-y );
}
/**
 * @brief És un drawChar de screeenIdx de color d = (pset || prev) ? fore : back; i pots ignorar la font
 * 
 * @param x 
 * @param y 
 */
// Updated display of character at specified position
void sc_updateChar(uint16_t x, uint16_t y) {
  uint16_t idx = SC_W * y + x;
  uint8_t c    = screen[idx];        // Get Character
  uint8_t* ptr = &fontTop[c * CH_H]; // Font data top address
  union ATTR a;
  union COLOR l;
  a.value = attrib[idx];             // Get character attributes
  l.value = colors[idx];             // Get color attribute
  uint16_t fore = aColors[l.Color.Foreground | (a.Bits.Blink << 3)];
  uint16_t back = aColors[l.Color.Background | (a.Bits.Blink << 3)];
  if (a.Bits.Reverse) swapInt(&fore, &back);
  if (mode_ex.Flgs.ScreenReverse) swapInt(&fore, &back);
  uint16_t xx = x * CH_W;
  uint16_t yy = y * CH_H;
  tft.setTextColor(fore,back);
  //Serial.println(screen[idx]);
  tft.drawChar(screen[idx],xx, yy);// to-do bold+underline
  /*
  if (a.Bits.Bold)
    tft.setTextColor(fore,back);
  */  
  /*
  setTftWindow(xx, yy, xx + MAX_CH_X, yy + MAX_CH_Y);// define an area to receive a stream of pixels
  for (uint8_t i = 0; i < CH_H; i++) {
    bool prev = (a.Bits.Underline && (i == MAX_CH_Y));
    for (uint8_t j = 0; j < CH_W; j++) {
      bool pset = ((*ptr) & (0x80 >> j)); // repassada per la font bit a bit
      uint16_t d = (pset || prev) ? fore : back;// toca fore o BackGnd
      
      tft.pushColor(d);//push a single pixel, 
      if (a.Bits.Bold)
        prev = pset;
    }
    ptr++;
  }
  */

}

// drawing the cursor
void drawCursor(uint16_t x, uint16_t y) {
  uint16_t xx = x * CH_W;
  uint16_t yy = y * CH_H;
  setTftWindow(xx, yy, xx + MAX_CH_X, yy + MAX_CH_Y);
  for (uint8_t i = 0; i < CH_H; i++) {
    for (uint8_t j = 0; j < CH_W; j++)
      tft.pushColor(TFT_WHITE);
  }
}

// Show Cursor
void dispCursor(bool forceupdate) {
  if (escMode != em::NONE)
    return;
  if (!forceupdate)
    isShowCursor = !isShowCursor;
  if (lastShowCursor || (forceupdate && isShowCursor))
    sc_updateChar(p_XP, p_YP);
  if (isShowCursor) {
    drawCursor(XP, YP);
    p_XP = XP;
    p_YP = YP;
  }
  if (!forceupdate)
    canShowCursor = false;
  lastShowCursor = isShowCursor;
}

// Reflect specified line on TFT screen
// argument
// ln: line number (0-29)
void sc_updateLine(uint16_t ln) {
  for (uint16_t clm = 0; clm < SC_W; clm++) {    // Loop for the number of horizontal characters
    sc_updateChar(clm,ln);
  }
}
/*
  union ATTR a;
  union COLOR l;
  uint8_t c;
  uint8_t dt;
  uint16_t buf[2][SP_W];// buffer de l'amplada de una línia
  uint16_t cnt, idx;
      
  for (uint16_t i = 0; i < CH_H; i++) {            // Loop for 1 character height
    cnt = 0;
    for (uint16_t clm = 0; clm < SC_W; clm++) {    // Loop for the number of horizontal characters
      idx = ln * SC_W + clm;
      c  = screen[idx];                            // Get Character
      a.value = attrib[idx];                       //Get character attributes
      l.value = colors[idx];                       // Get color attribute
      uint16_t fore = aColors[l.Color.Foreground | (a.Bits.Blink << 3)];
      uint16_t back = aColors[l.Color.Background | (a.Bits.Blink << 3)];
      if (a.Bits.Reverse) swapInt(&fore, &back);
      if (mode_ex.Flgs.ScreenReverse) swapInt(&fore, &back);
      
      dt = fontTop[c * CH_H + i];                  // Acquisition of i-line data within a character
      bool prev = (a.Bits.Underline && (i == MAX_CH_Y));
      for (uint16_t j = 0; j < CH_W; j++) {
        bool pset = dt & (0x80 >> j);
        buf[i & 1][cnt] = (pset || prev) ? fore : back;
        if (a.Bits.Bold)
          prev = pset;
        cnt++;
      }
      
    }
  // Wait for SPI1 DMA transfer (SPI1 DMA1 CH3)
    tft.pushColors(buf[i & 1], SP_W, true);

  }
}
  */

// Move cursor to home position
void setCursorToHome() {
  XP = 0;
  YP = 0;
}
// DECSTBM (Set Top and Bottom Margins): スクロール範囲をPt行からPb行に設定
void setTopAndBottomMargins(int16_t s, int16_t e) {
  if (e <= s) return;
  M_TOP    = s - 1;
  if (M_TOP > MAX_SC_Y) M_TOP = MAX_SC_Y;
  M_BOTTOM = e - 1;
  if (M_BOTTOM > MAX_SC_Y) M_BOTTOM = MAX_SC_Y;
  setCursorToHome();
}

// Cursor Position and Attribute Initialization
void initCursorAndAttribute() {
  cAttr.value = defaultAttr.value;
  cColor.value = defaultColor.value;
  memset(tabs, 0x00, SC_W);
  for (uint8_t i = 0; i < SC_W; i += 8)
    tabs[i] = 1;
  setTopAndBottomMargins(1, SC_H);
  mode.value = defaultMode;
  mode_ex.value = defaultModeEx;
}

// single line scroll
// (DECSTBM affected by)
void scroll() {
   if (mode.Flgs.CrLf) XP = 0;
  YP++;
  if (YP > M_BOTTOM) {
    uint16_t n = SCSIZE - SC_W - ((M_TOP + MAX_SC_Y - M_BOTTOM) * SC_W);
    uint16_t idx = SC_W * M_BOTTOM;
    uint16_t idx2;
    uint16_t idx3 = M_TOP * SC_W;
    memmove(&screen[idx3], &screen[idx3 + SC_W], n);
    memmove(&attrib[idx3], &attrib[idx3 + SC_W], n);
    memmove(&colors[idx3], &colors[idx3 + SC_W], n);
    for (uint8_t x = 0; x < SC_W; x++) {
      idx2 = idx + x;
      screen[idx2] = backGroundChar;
      attrib[idx2] = defaultAttr.value;
      colors[idx2] = defaultColor.value;
    }
    setTftWindow(0, M_TOP * CH_H, MAX_SP_X, (M_BOTTOM + 1) * CH_H - 1);
    for (uint8_t y = M_TOP; y <= M_BOTTOM; y++)
      sc_updateLine(y);
    YP = M_BOTTOM;
  }
}

// パラメータのクリア
void clearParams(em m) {
  escMode = m;
  isDECPrivateMode = false;
  nVals = 0;
  vals[0] = vals[1] = vals[2] = vals[3] = 0;
  hasParam = false;
}




// エスケープシーケンス
// -----------------------------------------------------------------------------

// DECSC (Save Cursor): カーソル位置と属性を保存
void saveCursor() {
  b_XP = XP;
  b_YP = YP;
  bAttr.value = cAttr.value;
  bColor.value = cColor.value;
}

// DECRC (Restore Cursor): 保存したカーソル位置と属性を復帰
void restoreCursor() {
  XP = b_XP;
  YP = b_YP;
  cAttr.value = bAttr.value;
  cColor.value = bColor.value;
}

// DECKPAM (Keypad Application Mode): アプリケーションキーパッドモードにセット
void keypadApplicationMode() {
  Serial.println(F("Unimplement: keypadApplicationMode"));
}

// DECKPNM (Keypad Numeric Mode): 数値キーパッドモードにセット
void keypadNumericMode() {
  Serial.println(F("Unimplement: keypadNumericMode"));
}
// CUD (Cursor Down): カーソルをPl行下へ移動
// (DECSTBM の影響を受ける)
void cursorDown(int16_t v) {
  YP += v;
  if (YP >= M_BOTTOM) YP = M_BOTTOM;
}
// IND (Index): カーソルを一行下へ移動
// (DECSTBM の影響を受ける)
void index(int16_t v) {
  cursorDown(v);
}

// NEL (Next Line): 改行、カーソルを次行の最初へ移動
// (DECSTBM の影響を受ける)
void nextLine() {
  scroll();
}

// HTS (Horizontal Tabulation Set): 現在の桁位置にタブストップを設定
void horizontalTabulationSet() {
  tabs[XP] = 1;
}
// "[" Control Sequence Introducer (CSI) シーケンス
// -----------------------------------------------------------------------------

// CUU (Cursor Up): カーソルをPl行上へ移動
// (DECSTBM の影響を受ける)
void cursorUp(int16_t v) {
  YP -= v;
  if (YP <= M_TOP) YP = M_TOP;
}

// CUF (Cursor Forward): カーソルをPc桁右へ移動
void cursorForward(int16_t v) {
  XP += v;
  if (XP >= SC_W) XP = MAX_SC_X;
}

// CUB (Cursor Backward): カーソルをPc桁左へ移動
void cursorBackward(int16_t v) {
  XP -= v;
  if (XP <= 0) XP = 0;
}

// CUP (Cursor Position): カーソルをPl行Pc桁へ移動
// HVP (Horizontal and Vertical Position): カーソルをPl行Pc桁へ移動
void cursorPosition(uint8_t y, uint8_t x) {
  YP = y - 1;
  if (YP >= SC_H) YP = MAX_SC_Y;
  XP = x - 1;
  if (XP >= SC_W) XP = MAX_SC_X;
}

// 画面を再描画
void refreshScreen() {

  setTftWindow(0, 0, MAX_SP_X, MAX_SP_Y);
  for (uint8_t i = 0; i < SC_H; i++)
    sc_updateLine(i);
}

// ED (Erase In Display): 画面を消去
void eraseInDisplay(uint8_t m) {
  uint8_t sl = 0, el = 0;
  uint16_t idx = 0, n = 0;

  switch (m) {
    case 0:
      // カーソルから画面の終わりまでを消去
      sl = YP;
      el = MAX_SC_Y;
      idx = YP * SC_W + XP;
      n   = SCSIZE - (YP * SC_W + XP);
      break;
    case 1:
      // 画面の始めからカーソルまでを消去
      sl = 0;
      el = YP;
      idx = 0;
      n = YP * SC_W + XP + 1;
      break;
    case 2:
      // 画面全体を消去
      sl = 0;
      el = MAX_SC_Y;
      idx = 0;
      n = SCSIZE;
      break;
  }

  if (m <= 2) {
    memset(&screen[idx], 0x00, n);
    memset(&attrib[idx], defaultAttr.value, n);
    memset(&colors[idx], defaultColor.value, n);
    setTftWindow(0, sl * CH_H, MAX_SP_X, (el + 1) * CH_H - 1);
    for (uint8_t i = sl; i <= el; i++)
      sc_updateLine(i);
  }
}

// EL (Erase In Line): 行を消去
void eraseInLine(uint8_t m) {
  uint16_t slp = 0, elp = 0;

  switch (m) {
    case 0:
      // カーソルから行の終わりまでを消去
      slp = YP * SC_W + XP;
      elp = YP * SC_W + MAX_SC_X;
      break;
    case 1:
      // 行の始めからカーソルまでを消去
      slp = YP * SC_W;
      elp = YP * SC_W + XP;
      break;
    case 2:
      // 行全体を消去
      slp = YP * SC_W;
      elp = YP * SC_W + MAX_SC_X;
      break;
  }

  if (m <= 2) {
    uint16_t n = elp - slp + 1;
    memset(&screen[slp], 0x00, n);
    memset(&attrib[slp], defaultAttr.value, n);
    memset(&colors[slp], defaultColor.value, n);
    setTftWindow(0, YP * CH_H, MAX_SP_X, (YP + 1) * CH_H - 1);
    sc_updateLine(YP);
  }
}

// IL (Insert Line): カーソルのある行の前に Ps 行空行を挿入
// (DECSTBM の影響を受ける)
void insertLine(uint8_t v) {
  int16_t rows = v;
  if (rows == 0) return;
  if (rows > ((M_BOTTOM + 1) - YP)) rows = (M_BOTTOM + 1) - YP;
  int16_t idx = SC_W * YP;
  int16_t n = SC_W * rows;
  int16_t idx2 = idx + n;
  int16_t move_rows = (M_BOTTOM + 1) - YP - rows;
  int16_t n2 = SC_W * move_rows;

  if (move_rows > 0) {
    memmove(&screen[idx2], &screen[idx], n2);
    memmove(&attrib[idx2], &attrib[idx], n2);
    memmove(&colors[idx2], &colors[idx], n2);
  }
  memset(&screen[idx], 0x00, n);
  memset(&attrib[idx], defaultAttr.value, n);
  memset(&colors[idx], defaultColor.value, n);
  setTftWindow(0, YP * CH_H, MAX_SP_X, (M_BOTTOM + 1) * CH_H - 1);
  for (uint8_t y = YP; y <= M_BOTTOM; y++)
    sc_updateLine(y);
}

// DL (Delete Line): カーソルのある行から Ps 行を削除
// (DECSTBM の影響を受ける)
void deleteLine(uint8_t v) {
  int16_t rows = v;
  if (rows == 0) return;
  if (rows > ((M_BOTTOM + 1) - YP)) rows = (M_BOTTOM + 1) - YP;
  int16_t idx = SC_W * YP;
  int16_t n = SC_W * rows;
  int16_t idx2 = idx + n;
  int16_t move_rows = (M_BOTTOM + 1) - YP - rows;
  int16_t n2 = SC_W * move_rows;
  int16_t idx3 = (M_BOTTOM + 1) * SC_W - n;

  if (move_rows > 0) {
    memmove(&screen[idx], &screen[idx2], n2);
    memmove(&attrib[idx], &attrib[idx2], n2);
    memmove(&colors[idx], &colors[idx2], n2);
  }
  memset(&screen[idx3], 0x00, n);
  memset(&attrib[idx3], defaultAttr.value, n);
  memset(&colors[idx3], defaultColor.value, n);
  setTftWindow(0, YP * CH_H, MAX_SP_X, (M_BOTTOM + 1) * CH_H - 1);
  for (uint8_t y = YP; y <= M_BOTTOM; y++)
    sc_updateLine(y);
}

// CPR (Cursor Position Report): カーソル位置のレポート
void cursorPositionReport(uint16_t y, uint16_t x) {
  Serial3.print(F("\e["));
  Serial3.print(String(y, DEC));
  Serial3.print(F(";"));
  Serial3.print(String(x, DEC));
  Serial3.print(F("R")); // CPR (Cursor Position Report)
}

// DA (Device Attributes): 装置オプションのレポート
// オプションのレポート
void deviceAttributes(uint8_t m) {
  Serial3.print(F("\e[?1;0c")); // 0 No options
}

// TBC (Tabulation Clear): タブストップをクリア
void tabulationClear(uint8_t m) {
  switch (m) {
    case 0:
      // 現在位置のタブストップをクリア
      tabs[XP] = 0;
      break;
    case 3:
      // すべてのタブストップをクリア
      memset(tabs, 0x00, SC_W);
      break;
  }
}

// LNM (Line Feed / New Line Mode): 改行モード
void lineMode(bool m) {
  mode.Flgs.CrLf = m;
}

// DECSCNM (Screen Mode): // 画面反転モード
void screenMode(bool m) {
  mode_ex.Flgs.ScreenReverse = m;
  refreshScreen();
}

// DECAWM (Auto Wrap Mode): 自動折り返しモード
void autoWrapMode(bool m) {
  mode_ex.Flgs.WrapLine = m;
}

// SM (Set Mode): モードのセット
void setMode(int16_t *vals, int16_t nVals) {
  for (int16_t i = 0; i < nVals; i++) {
    switch (vals[i]) {
      case 20:
        // LNM (Line Feed / New Line Mode)
        lineMode(true);
        break;
      default:
        Serial.print(F("Unimplement: setMode "));
        Serial.println(String(vals[i], DEC));
        break;
    }
  }
}

// DECSET (DEC Set Mode): モードのセット
void decSetMode(int16_t *vals, int16_t nVals) {
  for (int16_t i = 0; i < nVals; i++) {
    switch (vals[i]) {
      case 5:
        // DECSCNM (Screen Mode): // 画面反転モード
        screenMode(true);
        break;
      case 7:
        // DECAWM (Auto Wrap Mode): 自動折り返しモード
        autoWrapMode(true);
        break;
      default:
        Serial.print(F("Unimplement: decSetMode "));
        Serial.println(String(vals[i], DEC));
        break;
    }
  }
}
// RI (Reverse Index): カーソルを一行上へ移動
// (DECSTBM の影響を受ける)
void reverseIndex(int16_t v) {
  cursorUp(v);
}

// DECID (Identify): 端末IDシーケンスを送信
void identify() {
  deviceAttributes(0); // same as DA (Device Attributes)
}

// RIS (Reset To Initial State) リセット
void resetToInitialState() {
  tft.fillScreen(aColors[defaultColor.Color.Background]);
  initCursorAndAttribute();
  eraseInDisplay(2);
}
// RM (Reset Mode): モードのリセット
void resetMode(int16_t *vals, int16_t nVals) {
  for (int16_t i = 0; i < nVals; i++) {
    switch (vals[i]) {
      case 20:
        // LNM (Line Feed / New Line Mode)
        lineMode(false);
        break;
      default:
        Serial.print(F("Unimplement: resetMode "));
        Serial.println(String(vals[i], DEC));
        break;
    }
  }
}

// DECRST (DEC Reset Mode): モードのリセット
void decResetMode(int16_t *vals, int16_t nVals) {
  for (int16_t i = 0; i < nVals; i++) {
    switch (vals[i]) {
      case 5:
        // DECSCNM (Screen Mode): // 画面反転モード
        screenMode(false);
        break;
      case 7:
        // DECAWM (Auto Wrap Mode): 自動折り返しモード
        autoWrapMode(false);
        break;
      default:
        Serial.print(F("Unimplement: decResetMode "));
        Serial.println(String(vals[i], DEC));
        break;
    }
  }
}

// SGR (Select Graphic Rendition): 文字修飾の設定
void selectGraphicRendition(int16_t *vals, int16_t nVals) {
  uint8_t seq = 0;
  uint16_t r, g, b, cIdx;
  bool isFore = true;
  for (int16_t i = 0; i < nVals; i++) {
    int16_t v = vals[i];
    switch (seq) {
      case 0:
        switch (v) {
          case 0:
            // 属性クリア
            cAttr.value = 0;
            cColor.value = defaultColor.value;
            break;
          case 1:
            // 太字
            cAttr.Bits.Bold = 1;
            break;
          case 4:
            // アンダーライン
            cAttr.Bits.Underline = 1;
            break;
          case 5:
            // 点滅 (明色表現)
            cAttr.Bits.Blink = 1;
            break;
          case 7:
            // 反転
            cAttr.Bits.Reverse = 1;
            break;
          case 21:
            // 二重下線 or 太字オフ
            cAttr.Bits.Bold = 0;
            break;
          case 22:
            // 太字オフ
            cAttr.Bits.Bold = 0;
            break;
          case 24:
            // アンダーラインオフ
            cAttr.Bits.Underline = 0;
            break;
          case 25:
            // 点滅 (明色表現) オフ
            cAttr.Bits.Blink = 0;
            break;
          case 27:
            // 反転オフ
            cAttr.Bits.Reverse = 0;
            break;
          case 38:
            seq = 1;
            isFore = true;
            break;
          case 39:
            // 前景色をデフォルトに戻す
            cColor.Color.Foreground = defaultColor.Color.Foreground;
            break;
          case 48:
            seq = 1;
            isFore = false;
            break;
          case 49:
            // 背景色をデフォルトに戻す
            cColor.Color.Background = defaultColor.Color.Background;
            break;
          default:
            if (v >= 30 && v <= 37) {
              // 前景色
              cColor.Color.Foreground = v - 30;
            } else if (v >= 40 && v <= 47) {
              // 背景色
              cColor.Color.Background = v - 40;
            }
            break;
        }
        break;
      case 1:
        switch (v) {
          case 2:
            // RGB
            seq = 3;
            break;
          case 5:
            // Color Index
            seq = 2;
            break;
          default:
            seq = 0;
            break;
        }
        break;
      case 2:
        // Index Color
        if (v < 256) {
          if (v < 16) {
            // ANSI カラー (16 色のインデックスカラーが使われる)
            cIdx = v;
          } else if (v < 232) {
            // 6x6x6 RGB カラー (8 色のインデックスカラー中で最も近い色が使われる)
            b = ( (v - 16)       % 6) / 3;
            g = (((v - 16) /  6) % 6) / 3;
            r = (((v - 16) / 36) % 6) / 3;
            cIdx = (b << 2) | (g << 1) | r;
          } else {
            // 24 色グレースケールカラー (2 色のグレースケールカラーが使われる)
            if (v < 244)
              cIdx = clBlack;
            else
              cIdx = clWhite;
          }
          if (isFore)
            cColor.Color.Foreground = cIdx;
          else
            cColor.Color.Background = cIdx;
        }
        seq = 0;
        break;
      case 3:
        // RGB - R
        seq = 4;
        break;
      case 4:
        // RGB - G
        seq = 5;
        break;
      case 5:
        // RGB - B
        // RGB (8 色のインデックスカラー中で最も近い色が使われる)
        r = map(vals[i - 2], 0, 255, 0, 1);
        g = map(vals[i - 1], 0, 255, 0, 1);
        b = map(vals[i - 0], 0, 255, 0, 1);
        cIdx = (b << 2) | (g << 1) | r;
        if (isFore)
          cColor.Color.Foreground = cIdx;
        else
          cColor.Color.Background = cIdx;
        seq = 0;
        break;
      default:
        seq = 0;
        break;
    }
  }
}

// DSR (Device Status Report): 端末状態のリポート
void deviceStatusReport(uint8_t m) {
  switch (m) {
    case 5:
      Serial3.print(F("\e[0n"));   // 0 Ready, No malfunctions detected (default) (DSR)
      break;
    case 6:
      cursorPositionReport(XP, YP); // CPR (Cursor Position Report)
      break;
  }
}

// DECLL (Load LEDS): LED の設定
void loadLEDs(uint8_t m) {
  switch (m) {
    case 0:
      // turn off all LEDs
      /*
      digitalWrite(LED_01, LOW);
      digitalWrite(LED_02, LOW);
      digitalWrite(LED_03, LOW);
      digitalWrite(LED_04, LOW);
      */
      break;
    case 1:
      // LED1 turn on
      //digitalWrite(LED_01, HIGH);
      break;
    case 2:
      // LED2 turn on
      //digitalWrite(LED_02, HIGH);
      break;
    case 3:
      // LED3 turn on
      //digitalWrite(LED_03, HIGH);
      break;
    case 4:
      // LED4 turn on
      //digitalWrite(LED_04, HIGH);
      break;
  }
}

// DECTST (Invoke Confidence Test): make a test diagnosis
void invokeConfidenceTests(uint8_t m) {
  //nvic_sys_reset();
}

// "]" Operating System Command (OSC) シーケンス
// -----------------------------------------------------------------------------

// "#" Line Size Command  シーケンス
// -----------------------------------------------------------------------------

// DECDHL (Double Height Line): カーソル行を倍高、倍幅、トップハーフへ変更
void doubleHeightLine_TopHalf() {
  Serial.println(F("Unimplement: doubleHeightLine_TopHalf"));
}

// DECDHL (Double Height Line): カーソル行を倍高、倍幅、ボトムハーフへ変更
void doubleHeightLine_BotomHalf() {
  Serial.println(F("Unimplement: doubleHeightLine_BotomHalf"));
}

// DECSWL (Single-width Line): カーソル行を単高、単幅へ変更
void singleWidthLine() {
  Serial.println(F("Unimplement: singleWidthLine"));
}

// DECDWL (Double-Width Line): カーソル行を単高、倍幅へ変更
void doubleWidthLine() {
  Serial.println(F("Unimplement: doubleWidthLine"));
}

// DECALN (Screen Alignment Display): 画面を文字‘E’で埋める
void screenAlignmentDisplay() {
  setTftWindow(0, 0, MAX_SP_X, MAX_SP_Y);
  memset(screen, 0x45, SCSIZE);
  memset(attrib, defaultAttr.value, SCSIZE);
  memset(colors, defaultColor.value, SCSIZE);
  for (uint8_t y = 0; y < SC_H; y++)
    sc_updateLine(y);
}

// "(" G0 Sets Sequence
// -----------------------------------------------------------------------------

// G0 文字コードの設定
void setG0charset(char c) {
  Serial.println(F("Unimplement: setG0charset"));
}

// "(" G1 Sets Sequence
// -----------------------------------------------------------------------------

// G1 文字コードの設定
void setG1charset(char c) {
  Serial.println(F("Unimplement: setG1charset"));
}

// Unknown Sequence
// -----------------------------------------------------------------------------

// 不明なシーケンス
void unknownSequence(em m, char c) {
  String s = (m != em::NONE) ? "[ESC]" : "";
  switch (m) {
    case em::CSI:
      s = s + " [";
      break;
    case em::LSC:
      s = s + " #";
      break;
    case em::G0S:
      s = s + " (";
      break;
    case em::G1S:
      s = s + " )";
      break;
    case em::CSI2:
      s = s + " [";
      if (isDECPrivateMode) 
        s = s + "?";
      break;
    default:
      // to-do unknownSequence unknownSequence
      break;
  }
  Serial.print(F("Unknown: "));
  Serial.print(s);
  Serial.print(F(" "));
  Serial.print(c);
}

// -----------------------------------------------------------------------------

// timer handler
void handle_timer() {
  canShowCursor = true;
}
// 文字描画
void printChar(char c) {
  // [ESC] キー
  if (c == 0x1b) {
    escMode = em::ES;   // エスケープシーケンス開始
    return;
  }
  // エスケープシーケンス
  if (escMode == em::ES) {
    switch (c) {
      case '[':
        // Control Sequence Introducer (CSI) シーケンス へ
        clearParams(em::CSI);
        break;
      case '#':
        // Line Size Command  シーケンス へ
        clearParams(em::LSC);
        break;
      case '(':
        // G0 セット シーケンス へ
        clearParams(em::G0S);
        break;
      case ')':
        // G1 セット シーケンス へ
        clearParams(em::G1S);
        break;
      default:
        // <ESC> xxx: エスケープシーケンス
        switch (c) {
          case '7':
            // DECSC (Save Cursor): カーソル位置と属性を保存
            saveCursor();
            break;
          case '8':
            // DECRC (Restore Cursor): 保存したカーソル位置と属性を復帰
            restoreCursor();
            break;
          case '=':
            // DECKPAM (Keypad Application Mode): アプリケーションキーパッドモードにセット
            keypadApplicationMode();
            break;
          case '>':
            // DECKPNM (Keypad Numeric Mode): 数値キーパッドモードにセット
            keypadNumericMode();
            break;
          case 'D':
            // IND (Index): カーソルを一行下へ移動
            index(1);
            break;
          case 'E':
            // NEL (Next Line): 改行、カーソルを次行の最初へ移動
            nextLine();
            break;
          case 'H':
            // HTS (Horizontal Tabulation Set): 現在の桁位置にタブストップを設定
            horizontalTabulationSet();
            break;
          case 'M':
            // RI (Reverse Index): カーソルを一行上へ移動
            reverseIndex(1);
            break;
          case 'Z':
            // DECID (Identify): 端末IDシーケンスを送信
            identify();
            break;
          case 'c':
            // RIS (Reset To Initial State): リセット
            resetToInitialState();
            break;
          default:
            // 未確認のシーケンス
            unknownSequence(escMode, c);
            break;
        }
        clearParams(em::NONE);
        break;
    }
    return;
  }

  // "[" Control Sequence Introducer (CSI) シーケンス
  int16_t v1 = 0;
  int16_t v2 = 0;

  if (escMode == em::CSI) {
    escMode = em::CSI2;
    isDECPrivateMode = (c == '?');
    if (isDECPrivateMode) return;
  }

  if (escMode == em::CSI2) {
    if (isdigit(c)) {
      // [パラメータ]
      vals[nVals] = vals[nVals] * 10 + (c - '0');
      hasParam = true;
    } else if (c == ';') {
      // [セパレータ]
      nVals++;
      hasParam = false;
    } else {
      if (hasParam) nVals++;
      switch (c) {
        case 'A':
          // CUU (Cursor Up): カーソルをPl行上へ移動
          v1 = (nVals == 0) ? 1 : vals[0];
          reverseIndex(v1);
          break;
        case 'B':
          // CUD (Cursor Down): カーソルをPl行下へ移動
          v1 = (nVals == 0) ? 1 : vals[0];
          cursorDown(v1);
          break;
        case 'C':
          // CUF (Cursor Forward): カーソルをPc桁右へ移動
          v1 = (nVals == 0) ? 1 : vals[0];
          cursorForward(v1);
          break;
        case 'D':
          // CUB (Cursor Backward): カーソルをPc桁左へ移動
          v1 = (nVals == 0) ? 1 : vals[0];
          cursorBackward(v1);
          break;
        case 'H':
        // CUP (Cursor Position): カーソルをPl行Pc桁へ移動
        case 'f':
          // HVP (Horizontal and Vertical Position): カーソルをPl行Pc桁へ移動
          v1 = (nVals == 0) ? 1 : vals[0];
          v2 = (nVals <= 1) ? 1 : vals[1];
          cursorPosition(v1, v2);
          break;
        case 'G':
          YP = (nVals == 0) ? 1 : vals[0];// usar: XP, YP
          break;
        case 'J':
          // ED (Erase In Display): 画面を消去
          v1 = (nVals == 0) ? 0 : vals[0];
          eraseInDisplay(v1);
          break;
        case 'K':
          // EL (Erase In Line) 行を消去
          v1 = (nVals == 0) ? 0 : vals[0];
          eraseInLine(v1);
          break;
        case 'L':
          // IL (Insert Line): カーソルのある行の前に Ps 行空行を挿入
          v1 = (nVals == 0) ? 1 : vals[0];
          insertLine(v1);
          break;
        case 'M':
          // DL (Delete Line): カーソルのある行から Ps 行を削除
          v1 = (nVals == 0) ? 1 : vals[0];
          deleteLine(v1);
          break;
        case 'c':
          // DA (Device Attributes): 装置オプションのレポート
          v1 = (nVals == 0) ? 0 : vals[0];
          deviceAttributes(v1);
          break;
        case 'g':
          // TBC (Tabulation Clear): タブストップをクリア
          v1 = (nVals == 0) ? 0 : vals[0];
          tabulationClear(v1);
          break;
        case 'h':
          if (isDECPrivateMode) {
            // DECSET (DEC Set Mode): モードのセット
            decSetMode(vals, nVals);
          } else {
            // SM (Set Mode): モードのセット
            setMode(vals, nVals);
          }
          break;
        case 'l':
          if (isDECPrivateMode) {
            // DECRST (DEC Reset Mode): モードのリセット
            decResetMode(vals, nVals);
          } else {
            // RM (Reset Mode): モードのリセット
            resetMode(vals, nVals);
          }
          break;
        case 'm':
          // SGR (Select Graphic Rendition): 文字修飾の設定
          if (nVals == 0)
            nVals = 1; // vals[0] = 0
          selectGraphicRendition(vals, nVals);
          break;
        case 'n':
          // DSR (Device Status Report): 端末状態のリポート
          v1 = (nVals == 0) ? 0 : vals[0];
          deviceStatusReport(v1);
          break;
        case 'q':
          // DECLL (Load LEDS): LED の設定
          v1 = (nVals == 0) ? 0 : vals[0];
          loadLEDs(v1);
          break;
        case 'r':
          // DECSTBM (Set Top and Bottom Margins): スクロール範囲をPt行からPb行に設定
          v1 = (nVals == 0) ? 1 : vals[0];
          v2 = (nVals <= 1) ? SC_H : vals[1];
          setTopAndBottomMargins(v1, v2);
          break;
        case 'y':
          // DECTST (Invoke Confidence Test): テスト診断を行う
          if ((nVals > 1) && (vals[0] = 2))
            invokeConfidenceTests(vals[1]);
          break;
        case 'd'://VPA = 'd', //Move cursor to the indicated row, current column.
          YP = (nVals == 0) ? 1 : vals[0];// usar: XP, YP
          break;
        default:
          
            // unconfirmed sequence
          unknownSequence(escMode, c);
          
          break;
      }
      clearParams(em::NONE);
    }
    return;
  }

  // "#" Line Size Command  シーケンス
  if (escMode == em::LSC) {
    switch (c) {
      case '3':
        // DECDHL (Double Height Line): カーソル行を倍高、倍幅、トップハーフへ変更
        doubleHeightLine_TopHalf();
        break;
      case '4':
        // DECDHL (Double Height Line): カーソル行を倍高、倍幅、ボトムハーフへ変更
        doubleHeightLine_BotomHalf();
        break;
      case '5':
        // DECSWL (Single-width Line): カーソル行を単高、単幅へ変更
        singleWidthLine();
        break;
      case '6':
        // DECDWL (Double-Width Line): カーソル行を単高、倍幅へ変更
        doubleWidthLine();
        break;
      case '8':
        // DECALN (Screen Alignment Display): 画面を文字‘E’で埋める
        screenAlignmentDisplay();
        break;
      default:
        // 未確認のシーケンス
        unknownSequence(escMode, c);
        break;
    }
    clearParams(em::NONE);
    return;
  }

  // "(" G0 セットシーケンス
  if (escMode == em::G0S) {
    // SCS (Select Character Set): G0 文字コードの設定
    setG0charset(c);
    clearParams(em::NONE);
    return;
  }

  // ")" G1 セットシーケンス
  if (escMode == em::G1S) {
    // SCS (Select Character Set): G1 文字コードの設定
    setG1charset(c);
    clearParams(em::NONE);
    return;
  }

  // 改行 (LF / VT / FF)
  if ((c == 0x0a) || (c == 0x0b) || (c == 0x0c)) {
    scroll();
    return;
  }

  // 復帰 (CR)
  if (c == 0x0d) {
    XP = 0;
    return;
  }

  // バックスペース (BS)
  if ((c == 0x08) || (c == 0x7f)) {
    cursorBackward(1);
    uint16_t idx = YP * SC_W + XP;
    screen[idx] = 0;
    attrib[idx] = 0;
    colors[idx] = cColor.value;
    sc_updateChar(XP, YP);
    return;
  }

  // タブ (TAB)
  if (c == 0x09) {
    int16_t idx = -1;
    for (int16_t i = XP + 1; i < SC_W; i++) {
      if (tabs[i]) {
        idx = i;
        break;
      }
    }
    XP = (idx == -1) ? MAX_SC_X : idx;
    return;
  }

  // normal characters
  if (XP < SC_W) {
    uint16_t idx = YP * SC_W + XP;
    screen[idx] = c;
    attrib[idx] = cAttr.value;
    colors[idx] = cColor.value;
    sc_updateChar(XP, YP);
  }

  // X 位置 + 1
  XP ++;

  // wrap line
  if (XP >= SC_W) {
    if (mode_ex.Flgs.WrapLine)
      scroll();
    else
      XP = MAX_SC_X;
  }
}
// 文字列描画
void printString(const char *str) {
  while (*str) printChar(*str++);
}

UART Serial3(p0, p1);

// セットアップ
void setup() {
  //keyboard.begin(KBD_DAT, KBD_CLK);
  Serial.begin(57600);
  Serial3.begin(DEFAULT_BAUDRATE);
  Wire.begin();
/*
  // LED initialization of
  pinMode(LED_01, OUTPUT);
  pinMode(LED_02, OUTPUT);
  pinMode(LED_03, OUTPUT);
  pinMode(LED_04, OUTPUT);
  digitalWrite(LED_01, LOW);
  digitalWrite(LED_02, LOW);
  digitalWrite(LED_03, LOW);
  digitalWrite(LED_04, LOW);
  */

  // TFT initialization of
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  /*
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  setTftWindow(30, 30, 40, 40);//define an area to receive a stream of pixels
  for (int i = 0; i < 50; i++) {
    tft.pushColor(TFT_GREEN);//push a single pixel
  }

  tft.print("capdeporc");
  delay(2000);*/
  tft.drawChar('G',20,10);
  //fontTop = (uint8_t*)font6x8tt + 3;
  fontTop = (uint8_t*)font;
  resetToInitialState();
  //printString("test 123123123412 GARFIUS!!!");
  printString("\e[0;44m *** Terminal Init *** \e[0m\n");
  setCursorToHome();
  
  // Setting the timer for the cursor
  timer.setInterval(700, handle_timer);
  
}

// ループ
void loop() {
  bool needCursorUpdate = false;
  // Keyboard input processing (output to communication partner)
  if (TTYS0_Keyboard.available()) {
    char c = TTYS0_Keyboard.read();
    Serial3.print(c);
    needCursorUpdate = c;
  }

  timer.run();
  // Cursor display processing --> needCursorUpdate <==> resultat doGuess
  if (canShowCursor || needCursorUpdate)
    dispCursor(needCursorUpdate);

  // Serial input processing (input from communication partner)
  while (Serial3.available()) {
    char c = Serial3.read();
    switch (c) {
      case 0x07:
        //tone(SPK_PIN, 4000, 583);to-do
        break;
      default:
        printChar(c);
        break;
    }
  }
}
