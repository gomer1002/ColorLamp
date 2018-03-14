/* Color lamp on WS2812 leds
 * with OLED display and
 * different glow modes.
 * 
 * !!! This version works on FastLED library !!!
 * !!! Not NeoPixel                          !!!
 * 
 * Made by Gomer1002
 */

#include <OLED_I2C.h>
#include "FastLED.h"
#include <EEPROM.h>

OLED oled(SDA, SCL);
extern uint8_t (SmallFont[]);
extern uint8_t (TinyFont[]);

#define pin 9
#define len_led 60
CRGB leds[len_led];

#define swu 5
#define up 2
#define down 6
#define swd 4
#define md 3

#define max_mode 5

int color[4] = {0, 0, 0, 0}, color_prev[4] = {0, 0, 0, 0};
byte color_pos = 0, color_pos_prev;
byte curs = 0;
byte mode, mode_prev, del, del_prev, color_step, color_step_prev;

extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
CRGBPalette16 paletes[] = {ForestColors_p, CloudColors_p, RainbowColors_p};

void set_pixel(int _num, byte red,  byte green, byte blue) {
  leds[_num].r = red;
  leds[_num].g = green;
  leds[_num].b = blue;
}

void setup() {

  Serial.begin(9600);

  oled.begin();
  oled.setFont(SmallFont);

  pinMode(swu, INPUT); digitalWrite(swu, HIGH);
  pinMode(down, INPUT); digitalWrite(down, HIGH);
  pinMode(up, INPUT); digitalWrite(up, HIGH);
  pinMode(swd, INPUT); digitalWrite(swd, HIGH);
  pinMode(md, INPUT); digitalWrite(md, HIGH);

  color[0] = EEPROM.read(10);
  color[1] = EEPROM.read(11);
  color[2] = EEPROM.read(12);
  color[3] = EEPROM.read(13);
  mode = EEPROM.read(14);
  del = EEPROM.read(15);
  color_step = EEPROM.read(16);

  FastLED.addLeds<WS2812, pin, GRB>(leds, len_led);
  FastLED.show();
}

void save_settings() {
  EEPROM.write(10, constrain(color[0], 0, 255));
  EEPROM.write(11, constrain(color[1], 0, 255));
  EEPROM.write(12, constrain(color[2], 0, 255));
  EEPROM.write(13, constrain(color[3], 0, 255));
  EEPROM.write(14, constrain(mode, 0, 255));
  EEPROM.write(15, constrain(del, 1, 100));
  EEPROM.write(16, constrain(color_step, 1, 15));
  color[0] = EEPROM.read(10);
  color[1] = EEPROM.read(11);
  color[2] = EEPROM.read(12);
  color[3] = EEPROM.read(13);
  mode = EEPROM.read(14);
  del = EEPROM.read(15);
  color_step = EEPROM.read(16);
}

bool compare() {
  if  ((color_prev[0] != color[0]) ||
       (color_prev[1] != color[1]) ||
       (color_prev[2] != color[2]) ||
       (color_prev[3] != color[3]) ||
       (mode_prev != mode) ||
       (color_step_prev != color_step) ||
       (del_prev != del) ||
       (color_pos_prev != color_pos)) {
    color_prev[0] = color[0];
    color_prev[1] = color[1];
    color_prev[2] = color[2];
    color_prev[3] = color[3];
    color_step_prev = color_step;
    mode_prev = mode;
    del_prev = del;
    color_pos_prev = color_pos;
    return 1;
  } else return 0;
}

void read_color(byte _color_pos) {
  color[_color_pos] += !digitalRead(up) > 0 ? (color[_color_pos] < 50 ? 1 : 5) : 0;
  color[_color_pos] -= !digitalRead(down) > 0 ? (color[_color_pos] < 50 ? 1 : 5) : 0;
  save_settings();
}

bool flag_swd = 0;
bool read_button_swd(int max_pos) {
  if (!digitalRead(swd) == 1) {
    if (!flag_swd) {
      flag_swd = 1;
      if (color_pos < max_pos) {
        color_pos++;
      }
      if (color_pos == max_pos) color_pos = 0;
      return 1;
    } else {
      return 0;
    }
  } else {
    flag_swd = 0;
    return 0;
  }
}

bool flag_swu = 0;
bool read_button_swu(int max_pos) {
  if (!digitalRead(swu) == 1) {
    if (!flag_swu) {
      flag_swu = 1;
      if (color_pos > 0) {
        color_pos--;
      } else color_pos = max_pos - 1;
      return 1;
    } else {
      return 0;
    }
  } else {
    flag_swu = 0;
    return 0;
  }
}

bool flag_mode = 0;
bool read_button_mode() {
  if (!digitalRead(md) == 1) {
    if (!flag_mode) {
      flag_mode = 1;
      mode = mode + 1 >= max_mode ? 0 : mode + 1;
      return 1;
    } else {
      return 0;
    }
  } else {
    flag_mode = 0;
    return 0;
  }
}

void draw_stats_lamp() {
  read_button_swu(4);
  read_button_swd(4);
  read_color(color_pos);
  switch (color_pos) {
    case 0: curs = 3; break;
    case 1: curs = 0; break;
    case 2: curs = 1; break;
    case 3: curs = 2; break;
  }
  oled.clrScr();
  oled.print("Color Lamp", CENTER, 0);
  oled.print(">", 0, 8 + 8 * curs);
  oled.print("Red:", 8, 8);
  oled.printNumI(color[1], 78, 8);
  oled.print("Green:", 8, 16);
  oled.printNumI(color[2], 78, 16);
  oled.print("Blue:", 8, 24);
  oled.printNumI(color[3], 78, 24);
  oled.print("Brightness:", 8, 32);
  oled.printNumI(color[0], 78, 32);
  oled.update();
}

void lamp() {
  draw_stats_lamp();
  FastLED.setBrightness(color[0]);
  for (int pixel = 0; pixel < len_led; pixel++) {
    set_pixel(pixel, color[1], color[2], color[3]);
    delayMicroseconds(del * 5);
  }
  FastLED.show();
}

void draw_stats_rainbow() {
  read_button_swu(3);
  read_button_swd(3);
  read_color(color_pos);
  switch (color_pos) {
    case 0:
      curs = 0;
      break;
    case 1:
      curs = 1;
      del += !digitalRead(up) > 0 ? 1 : 0;
      del -= !digitalRead(down) > 0 ? 1 : 0;
      save_settings();
      break;
    case 2:
      curs = 2;
      color_step += !digitalRead(up) > 0 ? 1 : 0;
      color_step -= !digitalRead(down) > 0 ? 1 : 0;
      save_settings();
      break;
    default: color_pos = 0; break;
  }
  oled.clrScr();
  oled.print("Rainbow", CENTER, 0);
  oled.print(">", 0, 8 + 8 * curs);
  oled.print("Brightness:", 8, 8);
  oled.printNumI(color[0], 78, 8);
  oled.print("Delay:", 8, 16);
  oled.printNumI(del * 5, 78, 16);
  oled.print("Step:", 8, 24);
  oled.printNumI(color_step, 78, 24);
  oled.update();
}

bool smooth_change(int col1, int col2, int col3, int index_col, bool direction) {
  for (int i = direction ? 0 : 255; direction ? i < 256 : i >= 0; i += direction ? color_step : -color_step) {
    draw_stats_rainbow();
    if (read_button_mode() == 1) return 1;
    FastLED.setBrightness(color[0]);
    for (int pixel = 0; pixel < len_led; pixel++) {
      set_pixel(pixel, index_col == 1 ? i : col1, index_col == 2 ? i : col2, index_col == 3 ? i : col3);
      delayMicroseconds(del * 5);
    }
    FastLED.show();
  }
  return 0;
}

void fadeall() {
  for (int i = 0; i < len_led; i++) {
    leds[i].nscale8(250);
  }
}

void rainbow() {
  static uint8_t hue = 0;
  for (int i = 0; i < len_led; i ++) {
    draw_stats_rainbow();
    FastLED.setBrightness(color[0]);
    if (read_button_mode() == 1) return 1;
    leds[i] = CHSV(hue += color_step, 255, 255);
    FastLED.show();
    fadeall();
    delay(del * 5);
  }
  for (int i = (len_led) - 1; i >= 0; i --) {
    draw_stats_rainbow();
    FastLED.setBrightness(color[0]);
    if (read_button_mode() == 1) return 1;
    leds[i] = CHSV(hue += color_step, 255, 255);
    FastLED.show();
    fadeall();
    delay(del * 5);
  }
}

void draw_stats_breath() {
  read_button_swu(6);
  read_button_swd(6);
  switch (color_pos) {
    case 0:
      curs = 5;
      read_color(color_pos);
      break;
    case 1:
      curs = 0;
      read_color(color_pos);
      break;
    case 2:
      curs = 1;
      read_color(color_pos);
      break;
    case 3:
      curs = 2;
      read_color(color_pos);
      break;
    case 4:
      curs = 3;
      del += !digitalRead(up) > 0 ? 1 : 0;
      del -= !digitalRead(down) > 0 ? 1 : 0;
      save_settings();
      break;
    case 5:
      curs = 4;
      color_step += !digitalRead(up) > 0 ? 1 : 0;
      color_step -= !digitalRead(down) > 0 ? 1 : 0;
      save_settings();
      break;
    default: color_pos = 0; break;
  }
  if (compare()) {
    oled.clrScr();
    oled.print("Breath", CENTER, 0);
    oled.print(">", 0, 8 + 8 * curs);
    oled.print("Red:", 8, 8);
    oled.printNumI(color[1], 78, 8);
    oled.print("Green:", 8, 16);
    oled.printNumI(color[2], 78, 16);
    oled.print("Blue:", 8, 24);
    oled.printNumI(color[3], 78, 24);
    oled.print("Delay:", 8, 32);
    oled.printNumI(del * 5, 78, 32);
    oled.print("Step:", 8, 40);
    oled.printNumI(color_step, 78, 40);
    oled.print("Brightness:", 8, 48);
    oled.printNumI(color[0], 78, 48);
    oled.update();
  }
}

bool smooth_breath(bool direction) {
  for (int i = direction ? 2 : color[0]; direction ? i <= color[0] : i >= 2; i += direction ? color_step : -color_step) {
    draw_stats_breath();
    if (read_button_mode() == 1) return 1;
    FastLED.setBrightness(i);
    for (int pixel = 0; pixel < len_led; pixel++) {
      set_pixel(pixel, color[1], color[2], color[3]);
      delayMicroseconds(del * 5);
    }
    FastLED.show();
  }
  return 0;
}

void breath() {
  if (smooth_breath(1)) return;
  if (smooth_breath(0)) return;
}

void color_palete(uint8_t colorIndex) {

  read_button_swu(3);
  read_button_swd(3);
  read_color(0);
  curs = color_pos;
  if (compare()) {
    oled.clrScr();
    oled.print("Paletes", CENTER, 0);
    oled.print(">", 0, 8 + 8 * curs);
    oled.print("Forest", 8, 8);
    oled.print("Cloud", 8, 16);
    oled.print("Rainbow", 8, 24);
    oled.print("Brightness:", 8, 32);
    oled.printNumI(color[0], 78, 32);
    oled.update();
  }


  FastLED.setBrightness(color[0]);
  for ( int i = 0; i < len_led; i++) {
    if (read_button_mode() == 1) return 1;
    leds[i] = ColorFromPalette( paletes[color_pos], colorIndex, color[0], LINEARBLEND);
    colorIndex += 3;
  }

  FastLED.show();
  FastLED.delay(10);
}

void power_off() {
  FastLED.setBrightness(0);
  FastLED.show();
  while (1) {
    oled.clrScr();
    oled.print("Power off", CENTER, 16);
    oled.setFont(TinyFont);
    oled.print("Press mode button to exit", CENTER, 48);
    oled.update();
    oled.setFont(SmallFont);
    if (read_button_mode() == 1) return 1;
  }
}

void loop() {
  read_button_mode();
  static uint8_t startIndex = 0;
  startIndex += 1;
  switch (mode) {
    case 0: lamp(); break;
    case 1: rainbow(); break;
    case 2: breath(); break;
    case 3: color_palete(startIndex); break;
    case 4: power_off(); break;
  }
}
