#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_PIXELS 13

// For some reason the 144 led/m strip uses NEO_RGB in stead of NEO_GRB so be aware
#define NEO144 1
#ifdef NEO144
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_RGB + NEO_KHZ800);
#else
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
#endif
// ----------------------------------
// ---> Start: Support functions <---
// ----------------------------------
void clearStrip() {
  for( int i = 0; i<NUM_PIXELS; i++){
    strip.setPixelColor(i, 0x000000);
  }
  strip.show();
}

uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}
// --------------------------------
// ---> End: Support functions <---
// --------------------------------

// -------------------------------
// ---> Start: Main functions <---
// -------------------------------
// #FF1000 is the original color of the KITT front lights :-)
int KR_R = 0xFF;
int KR_G = 0x10;
int KR_B = 0x00;

void setup() {
  Serial.begin(9600);
  strip.begin();
  clearStrip(); // Initialize all pixels to 'off'
  KR_reset();
}

void loop() { 
  if (knightRider(3, 45, 2, strip.Color(KR_R, KR_G, KR_B))) { 
    int KR_T = KR_R;
    KR_R = KR_G;
    KR_G = KR_B;
    KR_B = KR_T;
  };
}
// -----------------------------
// ---> End: Main functions <---
// -----------------------------

// ----------------------------------
// ---> Start: Feature functions <---
// ----------------------------------

// #
// # Feature 1: Knight Rider light
// # Based on the code from https://github.com/technobly/NeoPixel-KnightRider
// # Changed to remove delay commands
uint32_t KR_old_val[NUM_PIXELS]; // up to 256 lights!
int KR_count;
int KR_dir;
unsigned long KR_next;
bool KR_overrun;
uint16_t KR_cycles;

void KR_reset() {
  for (int x=0; x<NUM_PIXELS; x++) {KR_old_val[x] = 0;}
  KR_count = 1;
  KR_dir = 1;
  KR_next = 0;
  KR_overrun = false;
  KR_cycles = 0;
}

bool knightRider(uint16_t cycles, uint16_t KR_speed, uint8_t width, uint32_t color) {
  if (KR_overrun) { KR_overrun = not(millis() < KR_next); }
  if (not(KR_overrun) && (millis() >= KR_next) && ((KR_cycles < cycles) || cycles == 0)) {
    if (KR_dir == 1) {
      strip.setPixelColor(KR_count, color);
      KR_old_val[KR_count] = color;
      for(int x = KR_count; x>0; x--) {
        KR_old_val[x-1] = dimColor(KR_old_val[x-1], width);
        strip.setPixelColor(x-1, KR_old_val[x-1]); 
      }
    } else {
      strip.setPixelColor(KR_count, color);
      KR_old_val[KR_count] = color;
      for(int x = KR_count; x<=NUM_PIXELS ;x++) {
        KR_old_val[x-1] = dimColor(KR_old_val[x-1], width);
        strip.setPixelColor(x+1, KR_old_val[x+1]);
      }
    }
    strip.show();
    if (KR_count == 0 || KR_count == NUM_PIXELS - 1) { 
      KR_dir = KR_dir * -1;
      if (KR_dir == 1) { KR_cycles++; }
      if (cycles > 0 && KR_cycles == cycles) { 
        clearStrip(); 
        KR_reset();
        return true;
      }
    }
    KR_count += KR_dir; 
    unsigned long KR_prev = KR_next;     
    KR_next = millis() + KR_speed;
    KR_overrun = (KR_next < KR_prev);
  }
  return false;
}
// --------------------------------
// ---> End: Feature functions <---
// --------------------------------
