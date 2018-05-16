#include <Wire.h>
#include <GloveButton.h>
#include <GloveDisplay.h>
// 2, 4, 5, 6, 7, 8, 9, 10
//       ~  ~        ~   ~

const uint8_t B_S = 0x80;
const uint8_t B_X = 0x81;
const uint8_t B_T = 0x82;
const uint8_t B_C = 0x83;
const uint8_t B_S_L = 0x84;
const uint8_t B_X_L = 0x85;
const uint8_t B_T_L = 0x86;
const uint8_t B_C_L = 0x87;
const uint8_t B_S_D = 0x88;
const uint8_t B_X_D = 0x89;
const uint8_t B_T_D = 0x8a;
const uint8_t B_C_D = 0x8b;
const uint8_t B_S_O = 0x8c;
const uint8_t B_X_O = 0x8d;
const uint8_t B_T_O = 0x8e;
const uint8_t B_C_O = 0x8f;

const int PIN_F1 = 2;
const int PIN_F2 = 4;
const int PIN_F3 = 7;
const int PIN_F4 = 8;
const int PIN_L1 = 5;
const int PIN_L2 = 6;
const int PIN_L3 = 9;
const int PIN_L4 = 10;

//1 = 0, f2 = 0, f3 = 0, f4 = 0;
boolean f1, f2, f3, f4;

GloveButton b1(PIN_F1), b2(PIN_F2), b3(PIN_F3), b4(PIN_F4);

void setup()
{
  Wire.begin();
  Display.init();  //initialze SEEED OLED display

  Display.clearDisplay();          //clear the screen and set start position to top left corner
  Display.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  Display.setPageMode();           //Set addressing mode to Page Mode
  Display.setHorizontalMode();           //Set addressing mode to Page Mode
  // Display.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  // Display.putString("Hello World!"); //Print the String

  Display.putString("ABCDEFGHIJ");
  Display.putInvertedString("ABCDEFGHIJKlmnopqrstuvwxyz");

  //Display.putString("\x80\x80\x80\x81\x81\x81\x82\x82\x82\x83\x83\x83");
  //Display.putString("\x84\x84\x84\x85\x85\x85x\x86\x86\x86\x87\x87\x87oooxXOA");

  b1.setup();
  b2.setup();
  b3.setup();
  b4.setup();
  pinMode(PIN_L1, OUTPUT);
  pinMode(PIN_L2, OUTPUT);
  pinMode(PIN_L3, OUTPUT);
  pinMode(PIN_L4, OUTPUT);
}

bool lon = false;

void toggle_light() {
  if ( lon ) {
    analogWrite(PIN_L1, 0);
    analogWrite(PIN_L2, 0);
    analogWrite(PIN_L3, 0);
    analogWrite(PIN_L4, 0);
    lon = false;
  } else {
    analogWrite(PIN_L1, 255);
    analogWrite(PIN_L2, 255);
    analogWrite(PIN_L3, 255);
    analogWrite(PIN_L4, 255);
    lon = true;
  }
}

void loop() {
  static byte col = 0;
  b1.read();
  b2.read();
  b3.read();
  b4.read();
  if ( b1.get_event() & GloveButton::PRESS ) {
    Display.putChar(B_S);
    col += 16;
    Wire.beginTransmission(4);
    Wire.write(col);
    Wire.endTransmission();
  } else if ( b1.get_event() & GloveButton::HOLD ) {
    Display.putChar(B_S_L);
  } else if ( b1.get_event() & GloveButton::REPEAT ) {
    Display.putChar(B_S_D);
  } else if ( b1.get_event() & GloveButton::RELEASE ) {
    Display.putChar(B_S_O);
  }
  if ( b2.get_event() & GloveButton::PRESS ) {
    Display.putChar(B_X);
    col -= 16;
    Wire.beginTransmission(4);
    Wire.write(col);
    Wire.endTransmission();
  } else if ( b2.get_event() & GloveButton::HOLD ) {
    Display.putChar(B_X_L);
  } else if ( b2.get_event() & GloveButton::REPEAT ) {
    Display.putChar(B_X_D);
  } else if ( b2.get_event() & GloveButton::RELEASE ) {
    Display.putChar(B_X_O);
  }
  if ( b3.get_event() & GloveButton::PRESS ) {
    Display.putString("3");
    toggle_light();
  } else if ( b3.get_event() & GloveButton::HOLD ) {
    Display.putString("c");
  } else if ( b3.get_event() & GloveButton::REPEAT ) {
    Display.putString("C");
  }
  if ( b4.get_event() & GloveButton::PRESS ) {
    Display.putString("4");
  } else if ( b4.get_event() & GloveButton::HOLD ) {
    Display.clearDisplay();
  }
  return;
  
  col += 1;
  Wire.beginTransmission(4);
  Wire.write(col);
  Wire.endTransmission();

  return;

  
  
  f1 = b1.get_down();
  if ( f1 ) {
    analogWrite(PIN_L1, 255);
  } else {
    analogWrite(PIN_L1, 0);
  }
  if ( f2 ) {
    analogWrite(PIN_L2, 255);
  } else {
    analogWrite(PIN_L2, 0);
  }
  if ( f3 ) {
    analogWrite(PIN_L3, 255);
  } else {
    analogWrite(PIN_L3, 0);
  }
  if ( f4 ) {
    analogWrite(PIN_L4, 255);
  } else {
    analogWrite(PIN_L4, 0);
  }
  /*Display.setTextXY(0, 0);
  Display.putString("Finger 4: ");
  Display.putNumber(f4);
  Display.setTextXY(1, 0);
  Display.putString("Finger 3: ");
  Display.putNumber(f3);
  Display.setTextXY(2, 0);
  Display.putString("Finger 2: ");
  Display.putNumber(f2);*/
  Display.setTextXY(3, 0);
  Display.putString("Finger 1: ");
  Display.putNumber(b1.get_cumulate());
//  Display.setTextXY(7, 0);
//  Display.putNumber(millis());
  //delay(1);

}

