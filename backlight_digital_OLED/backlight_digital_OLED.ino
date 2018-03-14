/* Color lamp on WS2812 leds
 * with OLED display and some 
 * different glow modes.
 * 
 * Made by Gomer1002
 */

#include <EEPROM.h>
#include <OLED_I2C.h>

OLED oled(SDA, SCL);
extern uint8_t (SmallFont[]);

#include "FastLED.h"
#define pin 9
#define num 60
CRGB leds[num];

#define sw 2
#define up 3
#define down 4
#define mu 5
#define md 6

int color[4] = {0, 0, 0, 0};
byte color_prev[4] = {0, 0, 0, 0};
byte color_pos = 0;
byte color_pos_prev = 0;
byte curs = 0;
byte mode, del, color_step;
byte del_prev, color_step_prev;



void set_pixel(int _num, byte red,  byte green, byte blue) {
  leds[_num].r = red;
  leds[_num].g = green;
  leds[_num].b = blue;
}

void setup() {
  Serial.begin(9600);
  oled.begin();
  oled.setFont(SmallFont);

  pinMode(sw, INPUT); digitalWrite(sw, HIGH);
  pinMode(down, INPUT); digitalWrite(down, HIGH);
  pinMode(up, INPUT); digitalWrite(up, HIGH);
  pinMode(mu, INPUT); digitalWrite(mu, HIGH);
  pinMode(md, INPUT); digitalWrite(md, HIGH);

  color[0] = EEPROM.read(10);
  color[1] = EEPROM.read(11);
  color[2] = EEPROM.read(12);
  color[3] = EEPROM.read(13);
  mode = EEPROM.read(14);
  del = EEPROM.read(15);
  color_step = EEPROM.read(16);

  FastLED.addLeds<WS2812, pin, GRB>(leds, num);
  FastLED.setBrightness(15);
  FastLED.show();

  if (mode == 0) {
    for (int pixel = 1; pixel < num; pixel ++) {
      set_pixel(pixel, color[1], color[2], color[3]);
      delayMicroseconds(del);
    }
  FastLED.show();
    drawStatsLamp();
  }

}

void fixColor() {
  color[0] = constrain(color[0], 0, 255);
  color[1] = constrain(color[1], 0, 255);
  color[2] = constrain(color[2], 0, 255);
  color[3] = constrain(color[3], 0, 255);
  EEPROM.write(10, color[0]);
  EEPROM.write(11, color[1]);
  EEPROM.write(12, color[2]);
  EEPROM.write(13, color[3]);
  EEPROM.write(14, mode);
  EEPROM.write(15, del);
  EEPROM.write(16, color_step);
}

bool flagSW = 0;
bool rbSW(int max_pos) {
  if ((!digitalRead(sw) == 1) && (flagSW == 0)) {
    flagSW = 1;
    if (color_pos < max_pos) {
      color_pos++;
    }
    if (color_pos == max_pos) color_pos = 0;
    return (1);
  }
  if ((!digitalRead(sw) == 1) && (flagSW == 1)) {
    return (0);
  }
  if ((!digitalRead(sw) == 0) && (flagSW == 1)) {
    flagSW = 0;
  }
  if ((!digitalRead(sw) == 0) && (flagSW == 0)) {
    return (0);
  }
}

bool flagMD = 0;
bool rbMD() {
  if ((!digitalRead(md) == 1) && (flagMD == 0)) {
    flagMD = 1;
    mode++;
    if (mode == 2) mode = 0;
    return 1;
  }
  if ((!digitalRead(md) == 1) && (flagMD == 1)) {
    return (0);
  }
  if ((!digitalRead(md) == 0) && (flagMD == 1)) {
    flagMD = 0;
  }
  if ((!digitalRead(md) == 0) && (flagMD == 0)) {
    return (0);
  }
}

void drawStatsLamp() {
  oled.clrScr();
  oled.print("Color Lamp", CENTER, 0);
  oled.print(">", 0, 8 + 8 * curs);
  oled.print("Red:", 8, 8);
  oled.printNumI(color[2], 78, 16);
  oled.print("Green:", 8, 16);
  oled.printNumI(color[1], 78, 8);
  oled.print("Blue:", 8, 24);
  oled.printNumI(color[3], 78, 24);
  oled.print("Brightness:", 8, 32);
  oled.printNumI(color[0], 78, 32);
  oled.update();
}

void drawStatsRainbow() {
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

void getStatsRainbow() {
  rbSW(3);
  if (color_pos == 0) {
    curs = 0;
  } else if (color_pos == 1) {
    curs = 1;
    if ((!digitalRead(up) > 0) && (del < 100)) {
      del += 1;
    }
    if ((!digitalRead(down) > 0) && (del > 1)) {
      del -= 1;
    }
  } else if (color_pos == 2) {
    curs = 2;
    if ((!digitalRead(up) > 0) && (color_step < 15)) {
      color_step += 1;
    }
    if ((!digitalRead(down) > 0) && (color_step > 1)) {
      color_step -= 1;
    }
  } else if (color_pos > 2) {
    color_pos = 0;
  }
}

bool compare() {
  if
  ((color_prev[0] != color[0]) ||
      (color_prev[1] != color[1]) ||
      (color_prev[2] != color[2]) ||
      (color_prev[3] != color[3]) ||
      (color_pos_prev != color_pos) ||
      (del_prev != del) ||
      (color_step_prev != color_step)) {
    color_prev[0] = color[0];
    color_prev[1] = color[1];
    color_prev[2] = color[2];
    color_prev[3] = color[3];
    color_pos_prev = color_pos;
    del_prev = del;
    color_step_prev = color_step;
    return 1;
  } else {
    return 0;
  }
}

void readColor() {
  if (!digitalRead(up) > 0) {
    if (color[color_pos] < 50) {
      color[color_pos] += 1;
    } else {
      color[color_pos] += 5;
    }
  }
  if (!digitalRead(down) > 0) {
    if (color[color_pos] < 50) {
      color[color_pos] -= 1;
    } else {
      color[color_pos] -= 5;
    }
  }
  fixColor();
}

void lamp() {
  rbSW(4);
  readColor();

  if (color_pos == 0) {
    curs = 3;
  } else if (color_pos == 1) {
    curs = 0;
  } else if (color_pos == 2) {
    curs = 1;
  } else if (color_pos == 3) {
    curs = 2;
  }

  if (compare() == 1) {
    FastLED.setBrightness(color[0]);
    for (int pixel = 0; pixel < num; pixel ++) {
      set_pixel(pixel, color[1], color[2], color[3]);
      delayMicroseconds(del);
    }
    FastLED.show();
    drawStatsLamp();
  }
}

bool smooth_change(int col1, int col2, int col3, int index_col, bool direction) {
  for (int i = direction ? 0 : 255; direction ? i < 256 : i >= 0; i += direction ? color_step : -color_step) {
    readColor();
    getStatsRainbow();
    drawStatsRainbow();
    FastLED.setBrightness(color[0]);
    if (rbMD() == 1) return 1;
    for (int pixel = 0; pixel < num; pixel ++) {
      set_pixel(pixel, index_col == 1 ? i : col1, index_col == 2 ? i : col2, index_col == 3 ? i : col3);
      delayMicroseconds(del * 5);
    }
    FastLED.show();
  }
  return 0;
}

bool rainbow() {
  if (smooth_change(0, 0, 0, 1, 1) == 1) return 1;
  if (smooth_change(255, 0, 0, 2, 1) == 1) return 1;
  if (smooth_change(255, 255, 0, 3, 1) == 1) return 1;
  if (smooth_change(255, 255, 0, 3, 0) == 1) return 1;
  if (smooth_change(255, 0, 0, 2, 0) == 1) return 1;
  if (smooth_change(0, 0, 0, 1, 0) == 1) return 1;
  return 0;
}

void loop() {
  rbMD();
  if (mode == 0) {
    lamp();
  } else if (mode == 1) {
    if (rainbow() == 1) {
      FastLED.setBrightness(color[0]);
      for (int pixel = 1; pixel < num; pixel ++) {
        set_pixel(pixel, color[1], color[2], color[3]);
        delayMicroseconds(del);
      }
      FastLED.show();
      drawStatsLamp();
    }
  }
}

