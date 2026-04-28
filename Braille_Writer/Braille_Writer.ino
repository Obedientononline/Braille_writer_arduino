#include <SPI.h>
#include <MD_MAX72xx.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES   1
#define CS_PIN        10
#define DIN_PIN       11
#define CLK_PIN       13

#define JOY_X_PIN     A0
#define JOY_Y_PIN     A1
#define JOY_BTN_PIN   6

#define JOY_THRESHOLD  300
#define MOVE_DELAY     220
#define LONG_PRESS_MS  1000
#define BLINK_INTERVAL 300

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
LiquidCrystal_I2C lcd(0x27, 16, 2);

struct DotPos { uint8_t row; uint8_t col; };

const DotPos DOT_POS[6] = {
  {1, 1},  // dot 1 : (top-left of Braille cell)
  {2, 1},  // dot 2 : (mid-left)
  {3, 1},  // dot 3 : (bot-left)
  {1, 2},  // dot 4 : (top-right)
  {2, 2},  // dot 5 : (mid-right)
  {3, 2},  // dot 6 : (bot-right)
};

const DotPos BORDER[] = {
  {0,0},{0,1},{0,2},{0,3},
  {4,0},{4,1},{4,2},{4,3},
  {1,0},{2,0},{3,0},
  {1,3},{2,3},{3,3},
};
const int BORDER_COUNT = sizeof(BORDER) / sizeof(BORDER[0]);

struct BrailleChar { byte dots; char letter; };
const BrailleChar BRAILLE_TABLE[] = {
  {0b000001,'A'},{0b000011,'B'},{0b001001,'C'},{0b011001,'D'},
  {0b010001,'E'},{0b001011,'F'},{0b011011,'G'},{0b010011,'H'},
  {0b001010,'I'},{0b011010,'J'},{0b000101,'K'},{0b000111,'L'},
  {0b001101,'M'},{0b011101,'N'},{0b010101,'O'},{0b001111,'P'},
  {0b011111,'Q'},{0b010111,'R'},{0b001110,'S'},{0b011110,'T'},
  {0b100101,'U'},{0b100111,'V'},{0b111010,'W'},{0b101101,'X'},
  {0b111101,'Y'},{0b110101,'Z'},
};
const int TABLE_SIZE = sizeof(BRAILLE_TABLE) / sizeof(BRAILLE_TABLE[0]);

bool    dotState[6]   = {false};
uint8_t cursorDot     = 0;      
bool    cursorVisible = true;

bool     btnPressed   = false;
uint32_t btnPressTime = 0;
bool     longPressDone = false;

uint32_t lastBlink = 0;
uint32_t lastMove  = 0;

int  lcdCol = 0;
int  lcdRow = 0;

void drawMatrix() {
  mx.clear();  

  for (int i = 0; i < BORDER_COUNT; i++) {
    mx.setPoint(BORDER[i].row, BORDER[i].col, true);
  }

  for (int i = 0; i < 6; i++) {
    if (dotState[i]) {
      mx.setPoint(DOT_POS[i].row, DOT_POS[i].col, true);
    }
  }

  bool showCursor = cursorVisible;
  mx.setPoint(
    DOT_POS[cursorDot].row,
    DOT_POS[cursorDot].col,
    dotState[cursorDot] || showCursor
  );
}

void moveCursor(int dx, int dy) {
  uint8_t col = cursorDot / 3;  
  uint8_t row = cursorDot % 3;   

  col = (col + dx + 2) % 2;
  row = (row + dy + 3) % 3;

  cursorDot = col * 3 + row;
}

char decodeBraille() {
  byte pattern = 0;
  for (int i = 0; i < 6; i++) {
    if (dotState[i]) pattern |= (1 << i);
  }
  if (pattern == 0) return ' ';
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (BRAILLE_TABLE[i].dots == pattern) return BRAILLE_TABLE[i].letter;
  }
  return '?';
}

void sendToLCD(char c) {
  lcd.setCursor(lcdCol, lcdRow);
  lcd.print(c);
  lcdCol++;
  if (lcdCol >= 16) {
    lcdCol = 0;
    lcdRow = (lcdRow + 1) % 2;
  }
  Serial.print(F("LCD: "));
  Serial.println(c);
}

void flashConfirm() {
  for (int i = 0; i < 3; i++) {
    // All border + dots full ON
    for (int b = 0; b < BORDER_COUNT; b++)
      mx.setPoint(BORDER[b].row, BORDER[b].col, true);
    for (int d = 0; d < 6; d++)
      mx.setPoint(DOT_POS[d].row, DOT_POS[d].col, true);
    delay(100);
    mx.clear();
    delay(80);
  }
}

void clearDots() {
  for (int i = 0; i < 6; i++) dotState[i] = false;
  cursorDot = 0; 
  drawMatrix();
}

void setup() {
  Serial.begin(9600);
  pinMode(JOY_BTN_PIN, INPUT_PULLUP);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 5);
  mx.control(MD_MAX72XX::SCANLIMIT, 7);  
  mx.control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::OFF);  
  delay(50);
  mx.clear();
  delay(50);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Braille Writer");
  lcd.setCursor(0, 1);
  lcd.print("Dot1=top-left");
  delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Short=toggle dot");
  lcd.setCursor(0, 1);
  lcd.print("Hold 1s=confirm");
  delay(2500);
  lcd.clear();
 drawMatrix();

  Serial.println(F("Ready. Cursor at dot1 (top-left of border box)."));
  Serial.println(F("Short press = toggle dot ON/OFF"));
  Serial.println(F("Hold 1s     = confirm letter to LCD"));
}

void loop() {
  uint32_t now = millis();
  if (now - lastBlink >= BLINK_INTERVAL) {
    lastBlink    = now;
    cursorVisible = !cursorVisible;
    drawMatrix();
  }
  if (now - lastMove >= MOVE_DELAY) {
    int jx = analogRead(JOY_X_PIN) - 512;
    int jy = analogRead(JOY_Y_PIN) - 512;

    int dx = 0, dy = 0;
    if      (jx >  JOY_THRESHOLD) dx = +1;
    else if (jx < -JOY_THRESHOLD) dx = -1;
    if      (jy >  JOY_THRESHOLD) dy = +1;
    else if (jy < -JOY_THRESHOLD) dy = -1;

    if (dx != 0 || dy != 0) {
      moveCursor(dx, dy);
      lastMove    = now;
      cursorVisible = true;
      lastBlink   = now;  
      drawMatrix();

      Serial.print(F("Cursor → dot"));
      Serial.println(cursorDot + 1);
    }
  }
  bool btnDown = (digitalRead(JOY_BTN_PIN) == LOW);

  if (btnDown && !btnPressed) {
    btnPressed    = true;
    btnPressTime  = now;
    longPressDone = false;
  }

  if (btnPressed && btnDown && !longPressDone) {
    if (now - btnPressTime >= LONG_PRESS_MS) {
      longPressDone = true;
      char letter = decodeBraille();
      flashConfirm();
      sendToLCD(letter);
      clearDots(); 
      Serial.print(F("Confirmed: "));
      Serial.println(letter);
    }
  }

  if (!btnDown && btnPressed) {
    if (!longPressDone) {
      dotState[cursorDot] = !dotState[cursorDot];
      cursorVisible = true;
      drawMatrix();

      Serial.print(F("Dot "));
      Serial.print(cursorDot + 1);
      Serial.println(dotState[cursorDot] ? F(" ON") : F(" OFF"));
    }
    btnPressed = false;
  }
}
