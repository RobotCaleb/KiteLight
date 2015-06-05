#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    120
#define BRIGHTNESS  32
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define CYLON_EYE_HALF_WIDTH 10
#define BUMPER_WIDTH 3

#define USABLE_WIDTH (NUM_LEDS - (BUMPER_WIDTH * 2))
#define USABLE_HALF (USABLE_START + USABLE_WIDTH / 2)
#define USABLE_START (BUMPER_WIDTH)

#define CYLON_DELAY  25
#define POLICE_DELAY 150
#define ALTERNATING_DELAY 5

CRGB leds[NUM_LEDS];

int redLookup[CYLON_EYE_HALF_WIDTH];

void setup() {
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(0);

  generate_cylon_lookup_values();
}

void loop()
{
  police_lights(50);
  fire_random_alternating(50);
  cylon(50);
}

void police_lights(int iter) {
  uint32_t red = 0xff0000;
  uint32_t blue = 0x0000ff;
  for (int i = 0; i < iter; ++i) {
    for (int n = USABLE_START; n < USABLE_WIDTH + USABLE_START; ++n )
    {
      if (n < USABLE_HALF) {
        leds[n].setColorCode(blue);
      }
      else if (n >= USABLE_HALF) {
        leds[n].setColorCode(red);
      }
    }
    bumpers();
    FastLED.show();
    FastLED.delay(POLICE_DELAY);

    for (int n = USABLE_START; n < USABLE_WIDTH + USABLE_START; ++n )
    {
      if (n < USABLE_HALF) {
        leds[n].setColorCode(red);
      }
      else
        leds[n].setColorCode(blue);
    }
    bumpers();
    FastLED.show();
    FastLED.delay(POLICE_DELAY);
  }
}

void fire_random_alternating(int iter) {
  for (int i = 0; i < iter; ++i) {
    int r = random8();
    int g = random8();
    int b = random8();
    int start = USABLE_HALF;
    for (int n = 0; n < start; ++n) {
      leds[start + 1 * n].setRGB(r, g, b);
      leds[start - 1 * n].setRGB(r, g, b);
      leds[start + 1 * (n - 1)].setRGB(0, 0, 0);
      leds[start - 1 * (n - 1)].setRGB(0, 0, 0);
      bumpers();
      FastLED.show();
      FastLED.delay(ALTERNATING_DELAY);
    }
  }
}

void generate_cylon_lookup_values() {
  // calculate cylon lookup table
  // simple square curve
  float step = 1.f / (CYLON_EYE_HALF_WIDTH - 1);
  for (int i = 0; i < CYLON_EYE_HALF_WIDTH; ++i) {
    float exp = (float)i * step;
    exp *= exp;
    redLookup[CYLON_EYE_HALF_WIDTH - 1 - i] = (int)(exp * 255);
  }
}

void bumpers() {
  for (int i = 0; i < BUMPER_WIDTH; i++) {
    leds[NUM_LEDS - 1 - i].setRGB(255, 255, 0);

    leds[i].setRGB(0, 255, 255);
  }
}

void cylon_eye_piece(int i) {
  for (int n = 0; n < CYLON_EYE_HALF_WIDTH; ++n) {
    if ((i + n) < USABLE_WIDTH + USABLE_START) {
      leds[i + n].setRGB(redLookup[n], 0, 0);
    }
    if ((i - n) >= USABLE_START) {
      leds[i - n].setRGB(redLookup[n], 0, 0);
    }
  }
}

void cylon(int iter) {
  for (int n = 0; n < iter; ++n) {
    for (int i = USABLE_START; i < USABLE_WIDTH; i++) {
      cylon_eye_piece(i);

      bumpers();

      FastLED.show();
      FastLED.delay(CYLON_DELAY);
    }

    for (int i = USABLE_WIDTH - 1; i >= USABLE_START; i--) {
      cylon_eye_piece(i);

      bumpers();

      FastLED.show();
      FastLED.delay(CYLON_DELAY);
    }
  }
}