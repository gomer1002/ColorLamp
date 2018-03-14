#include <OLED_I2C.h>
#include "FastLED.h"
#include <EEPROM.h>

OLED oled(SDA, SCL);
extern uint8_t (SmallFont[]);

#define pin 9
#define len_led 60
CRGB leds[len_led];

#define swu 5
#define up 2
#define down 6
#define swd 4
#define md 3

#define max_mode 4

int color[4] = {0, 0, 0, 0}, color_prev[4] = {0, 0, 0, 0};
byte color_pos = 0, color_pos_prev;
byte curs = 0;
byte mode, mode_prev, del, del_prev, color_step, color_step_prev;

extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
CRGBPalette16 paletes[] = {ForestColors_p, CloudColors_p, RainbowColors_p};

void setup() {
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

void set_pixel(int _num, byte red,  byte green, byte blue) {
  leds[_num].r = red;
  leds[_num].g = green;
  leds[_num].b = blue;
}

void read_button_pos() {
  if (!digitalRead(up) > 0) {
    if (color[color_pos] < 50) change_all(1, 1); else change_all(5, 1);
  }
  if (!digitalRead(down) > 0) {
    if (color[color_pos] < 50) change_all(-1, -1); else change_all(-5, -1);
  }
}

void change_all(int _in, int _other) {
  switch (mode) {
    case 0: switch (color_pos) {
        case 0: color[0] += _in; curs = 3; break;
        case 1: color[1] += _in; curs = 0; break;
        case 2: color[2] += _in; curs = 1; break;
        case 3: color[3] += _in; curs = 2; break;
      } break;
    case 1: switch (color_pos) {
        case 0: color[0] += _in; curs = 0; break;
        case 1: del += _other; curs = 1; break;
        case 2: color_step += _other; curs = 2; break;
      } break;
    case 2: switch (color_pos) {
        case 0: color[0] += _in; curs = 5; break;
        case 1: color[1] += _in; curs = 0; break;
        case 2: color[2] += _in; curs = 1; break;
        case 3: color[3] += _in; curs = 2; break;
        case 4: del += _other; curs = 3; break;
        case 5: color_step += _other; curs = 4; break;
      } break;
    case 3: switch (color_pos) {
        case 0: curs = 0; color[0] += _in; break;
        case 1: curs = 1; color[0] += _in; break;
        case 2: curs = 2; color[0] += _in; break;
      }
  }
  save_settings();
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

void save_settings() {
  if (compare()) {
    EEPROM.write(10, constrain(color[0], 0, 255));
    EEPROM.write(11, constrain(color[1], 0, 255));
    EEPROM.write(12, constrain(color[2], 0, 255));
    EEPROM.write(13, constrain(color[3], 0, 255));
    EEPROM.write(14, constrain(mode, 0, 255));
    EEPROM.write(15, constrain(del, 1, 100));
    EEPROM.write(16, constrain(color_step, 1, 15));
  }
  color[0] = EEPROM.read(10);
  color[1] = EEPROM.read(11);
  color[2] = EEPROM.read(12);
  color[3] = EEPROM.read(13);
  mode = EEPROM.read(14);
  del = EEPROM.read(15);
  color_step = EEPROM.read(16);
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
      color_pos = 0;
      return 1;
    } else {
      return 0;
    }
  } else {
    flag_mode = 0;
    return 0;
  }
}

void lamp() {
  FastLED.setBrightness(color[0]);
  draw_stats_lamp();
  if (read_button_mode() == 1) return 1;
  for (int pixel = 0; pixel < len_led; pixel++) {
    set_pixel(pixel, color[1], color[2], color[3]);
    delayMicroseconds(del * 5);
  }
  FastLED.show();
}

void draw_stats_lamp() {
  read_button_swu(4);
  read_button_swd(4);
  read_button_pos();

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

void draw_stats_rainbow() {
  read_button_swu(3);
  read_button_swd(3);
  read_button_pos();

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

void draw_stats_breath() {
  read_button_swu(6);
  read_button_swd(6);
  read_button_pos();

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

void palete() {
  static uint8_t startIndex = 0;
  startIndex += 1;
  uint8_t colorIndex = startIndex;

  FastLED.setBrightness(color[0]);
  draw_stats_palete();
  for ( int i = 0; i < len_led; i++) {
    if (read_button_mode() == 1) return 1;
    leds[i] = ColorFromPalette(paletes[color_pos], colorIndex, color[0], LINEARBLEND);
    colorIndex += 3;
  }
  FastLED.show();
  FastLED.delay(10);
}

void draw_stats_palete() {
  read_button_swu(3);
  read_button_swd(3);
  read_button_pos();

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
}

void loop() {
  switch (mode) {
    case 0: lamp(); break;
    case 1: rainbow(); break;
    case 2: breath(); break;
    case 3: palete(); break;
  }
}
