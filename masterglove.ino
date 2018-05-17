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

uint8_t m_state = 0;
uint8_t m_state_max = 2;
uint8_t m_pos = 0;
uint8_t m_bright = 3;
uint8_t m_bright_max = 5;
uint8_t m_mode = 0;
uint8_t m_mode_max = 8;
boolean m_print = true;

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

  Display.setTextXY(1, 4);
  Display.putInvertedString("Master");
  Display.setTextXY(2, 4);
  Display.putInvertedString("Control");
  Display.setTextXY(3, 4);
  Display.putInvertedString("Glove");
  Display.setTextXY(5, 0);
  Display.putString("initializing...");
  delay(1500);

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

uint8_t blur_i = 0;
void write_display() {
  if ( m_state == 0 ) {
    // nothing
    blur_i++;
    if ( blur_i % 32 == 0 ) {
      Display.sendData(random(256));
    }
  } else {
    if ( !m_print ) {
      return;
    }
    m_print = false;
    Display.setTextXY(0, 0);
    if ( m_state == 1 ) {
      // mode
      Display.putInvertedString("Vest Modus");
      Display.setTextXY(1, 0);
      Display.putString("Mode ");
      Display.putNumber(m_mode);
      Display.putString("/");
      Display.putNumber(m_mode_max);
      Display.putString("  ");
    } else if ( m_state == 2 ) {
      // bright
      Display.putInvertedString("Brightness");
      Display.setTextXY(1, 0);
      Display.putString("Level ");
      Display.putNumber(m_bright);
      Display.putString("/");
      Display.putNumber(m_bright_max);
      Display.putString("  ");
    }
  }
}

void menu_leave() {
  m_state = 0;
  m_print = true;
}

void menu_menu() {
  m_state += 1;
  if ( m_state > m_state_max ) {
    m_state = 0;
  }
  m_pos = 0;
  Display.clearDisplay();
  m_print = true;
}

void menu_next() {
  // index finger down:
  if ( m_state == 0) {
    // none - send beat
    send_vest(20);
  }
  if ( m_state == 1 ) {
    // modus
    m_mode +=1;
    if ( m_mode > m_mode_max ) {
      m_mode = 0;
    }
    send_mode();
  }
  if ( m_state == 2 ) {
    // brightness
    m_bright +=1;
    if ( m_bright > m_bright_max ) {
      m_bright = 0;
    }
    send_brightness();
  }
  m_print = true;
}

uint8_t m_mode_save = 255;
// middle finger down
void menu_prev() {
  if ( m_state == 0 ) {
    m_mode_save = m_mode;
    m_mode = 7;
    send_mode();
  }
  if ( m_state == 1 ) {
    // modus
    if ( m_mode == 0 ) {
      m_mode = m_mode_max;
    } else {
    m_mode -=1;
    }
    send_mode();
  }
  if ( m_state == 2 ) {
    // brightness
    if ( m_bright == 0 ) {
      m_bright = m_bright_max;
    } else {
      m_bright -=1;
    }
    send_brightness();
  }
  m_print = true;
}

// middle finger up
void menu_prev_up() {
  if ( m_state == 0 ) {
    if ( m_mode_save != 255 ) {
      m_mode = m_mode_save;
      m_mode_save = 255;
      send_mode();
    }
  }
  m_print = true;
}

void send_vest(uint8_t cmd) {
  Wire.beginTransmission(4);
  Wire.write(cmd);
  Wire.endTransmission();
}

void send_brightness() {
  if ( m_bright > m_bright_max ) return;
  send_vest(m_bright+200);
}

void send_mode() {
  if ( m_mode > m_mode_max ) return;
  send_vest(m_mode+100);
}

void loop() {
  static byte col = 0;
  b1.read();
  b2.read();
  b3.read();
  b4.read();
  // Index Finger (next):
  if ( b1.get_event() & GloveButton::PRESS ) {
    // Display.putChar(B_S);
    // col += 16;
    menu_next();
  } else if ( b1.get_event() & GloveButton::HOLD ) {
    // Display.putChar(B_S_L);
  } else if ( b1.get_event() & GloveButton::REPEAT ) {
    // Display.putChar(B_S_D);
  } else if ( b1.get_event() & GloveButton::RELEASE ) {
    // Display.putChar(B_S_O);
  }
  if ( b2.get_event() & GloveButton::PRESS ) {
    // Display.putChar(B_X);
    // col -= 16;
    menu_prev();
  } else if ( b2.get_event() & GloveButton::HOLD ) {
    // Display.putChar(B_X_L);
  } else if ( b2.get_event() & GloveButton::REPEAT ) {
    // Display.putChar(B_X_D);
  } else if ( b2.get_event() & GloveButton::RELEASE ) {
    // Display.putChar(B_X_O);
    menu_prev_up();
  }
  if ( b3.get_event() & GloveButton::PRESS ) {
    // Display.putString("3");
    toggle_light();
  } else if ( b3.get_event() & GloveButton::HOLD ) {
    // Display.putString("c");
  } else if ( b3.get_event() & GloveButton::REPEAT ) {
    // Display.putString("C");
  }
  if ( b4.get_event() & GloveButton::PRESS ) {
    // Display.putString("4");
    menu_menu();
  } else if ( b4.get_event() & GloveButton::HOLD ) {
    Display.clearDisplay();
    menu_leave();
  }
  write_display();
}

