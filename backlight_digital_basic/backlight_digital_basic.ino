/* Basic code for color lamp
 * on WS2812 leds.
 * This code contain basic functions  
 * to control brightness and color
 * using only 3 digital buttons. 
 * 
 * Made by Gomer1002
 */

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define pin 9
#define num 61
#define del 500

Adafruit_NeoPixel pix = Adafruit_NeoPixel(num, pin, NEO_GRB + NEO_KHZ800);

int red, green, blue, bright;
int color[4] = {0, 0, 0, 0};
int color_pos = 0;

#define sw 2
#define up 3
#define down 4


void setup() {

  pinMode(sw, INPUT); digitalWrite(sw, HIGH);
  pinMode(down, INPUT); digitalWrite(down, HIGH);
  pinMode(up, INPUT); digitalWrite(up, HIGH);

  color[0] = EEPROM.read(0);
  color[1] = EEPROM.read(1);
  color[2] = EEPROM.read(2);
  color[3] = EEPROM.read(3);

  pix.begin();
  pix.show();
  pix.setPixelColor(0, color[1], color[2], color[3]);
}

void fixColor() {
  color[0] = constrain(color[0], 0, 255);
  color[1] = constrain(color[1], 0, 255);
  color[2] = constrain(color[2], 0, 255);
  color[3] = constrain(color[3], 0, 255);
  EEPROM.write(0, color[0]);
  EEPROM.write(1, color[1]);
  EEPROM.write(2, color[2]);
  EEPROM.write(3, color[3]);
}

bool flagSW = 0;
bool rbSW() {
  if ((!digitalRead(sw) == 1) && (flagSW == 0)) {
    flagSW = 1;
    if (color_pos < 4) {
      color_pos++;
    }
    if (color_pos == 4) color_pos = 0;
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

void loop() {

  rbSW();
  if (!digitalRead(up) > 0) {
    color[color_pos] += 5;
  }
  if (!digitalRead(down) > 0) {
    color[color_pos] -= 5;
  }
  fixColor();

  if (color_pos == 0) {
    pix.setPixelColor(0, color[0], color[0], color[0]);
  } else if (color_pos == 1) {
    pix.setPixelColor(0, color[1], 0, 0);
  } else if (color_pos == 2) {
    pix.setPixelColor(0, 0, color[2], 0);
  } else if (color_pos == 3) {
    pix.setPixelColor(0, 0, 0, color[3]);
  }
  pix.show();

  pix.setBrightness(color[0]);
  for (int pixel = 1; pixel < num; pixel ++) {
    pix.setPixelColor(pixel, color[1], color[2], color[3]);
    delayMicroseconds(del);
  }
  pix.show();

}

