#include <FastLED.h>

#define LED_STRIP_1_PIN      5
#define LED_STRIP_2_PIN      6
#define NUM_LEDS_PER_STRIP   60
#define NUM_STRIPS           2
#define NUM_LEDS             (NUM_LEDS_PER_STRIP * NUM_STRIPS)

#define USABLE_WIDTH         (NUM_LEDS - (BUMPER_WIDTH * 2))
#define USABLE_HALF          (USABLE_START + USABLE_WIDTH / 2)
#define USABLE_START         (BUMPER_WIDTH)

#define BRIGHTNESS           255
#define LED_TYPE             WS2812B
#define COLOR_ORDER          GRB

#define CYLON_EYE_HALF_WIDTH 10
#define BUMPER_WIDTH         5

#define CYLON_DELAY          25
#define POLICE_DELAY         150
#define ALTERNATING_DELAY    5
#define PLASMA_DELAY         50

#define RUN_TIME             20000 /* 20 seconds */

CRGB leds[NUM_LEDS];

int redLookup[CYLON_EYE_HALF_WIDTH];

void setup() {
  delay(3000); // power-up safety delay

  FastLED.addLeds<LED_TYPE, LED_STRIP_1_PIN, COLOR_ORDER>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, LED_STRIP_2_PIN, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);

  generate_cylon_lookup_values();
}

uint16_t fix_index(uint16_t index) {
  if (index < NUM_LEDS_PER_STRIP) {
    return NUM_LEDS_PER_STRIP - index - 1;
  }

  // else
  return index;
}

void loop()
{
  plasma(RUN_TIME);
  rainbow(RUN_TIME);
  police_lights(RUN_TIME);
  fire_random_alternating(RUN_TIME);
  cylon(RUN_TIME);
}

void bumpers() {
  for (int i = 0; i < BUMPER_WIDTH - 1; i++) {
    leds[fix_index(i)].setColorCode(CRGB::Yellow);
    leds[fix_index(NUM_LEDS - 1 - i)].setColorCode(CRGB::Aqua);
  }

  leds[fix_index(NUM_LEDS - BUMPER_WIDTH)].setRGB(0, 0, 0);
  leds[fix_index(BUMPER_WIDTH - 2)].setRGB(0, 0, 0);

  leds[fix_index(NUM_LEDS - BUMPER_WIDTH + 1)].setRGB(0, 0, 0);
  leds[fix_index(BUMPER_WIDTH - 1)].setRGB(0, 0, 0);
}

void plasma(uint32_t time) {
  unsigned long now = millis();
  uint8_t n = 0;

  while (millis() - now < time) {
    for (uint32_t i = 0; i < USABLE_WIDTH; ++i) {
      int8_t wave = cubicwave8(i + sin8(n + i) + cos8(n - i));
      leds[fix_index(i + USABLE_START)] = HeatColor(wave);
    }
    n++;

    bumpers();
    FastLED.show();
    FastLED.delay(PLASMA_DELAY);
  }
}

void rainbow(uint32_t time) {
  unsigned long now = millis();
  uint32_t i = 0;

  while (millis() - now < time) {
    fill_rainbow(&(leds[USABLE_START]), USABLE_WIDTH, i);
    bumpers();
    FastLED.show();
    ++i;
  }
}

void police_lights(uint32_t time) {
  const uint32_t red = 0xff0000;
  const uint32_t blue = 0x0000ff;

  unsigned long now = millis();

  while (millis() - now < time) {
    for (int n = USABLE_START; n < USABLE_WIDTH + USABLE_START; ++n )
    {
      if (n < USABLE_HALF) {
        leds[fix_index(n)].setColorCode(blue);
      }
      else if (n >= USABLE_HALF) {
        leds[fix_index(n)].setColorCode(red);
      }
    }
    bumpers();
    FastLED.show();
    FastLED.delay(POLICE_DELAY);

    for (int n = USABLE_START; n < USABLE_WIDTH + USABLE_START; ++n )
    {
      if (n < USABLE_HALF) {
        leds[fix_index(n)].setColorCode(red);
      }
      else
        leds[fix_index(n)].setColorCode(blue);
    }
    bumpers();
    FastLED.show();
    FastLED.delay(POLICE_DELAY);
  }
}

void fire_random_alternating(uint32_t time) {
  unsigned long now = millis();

  while (millis() - now < time) {
    int r = random8();
    int g = random8();
    int b = random8();
    int start = USABLE_HALF;
    for (int n = 0; n < start; ++n) {
      leds[fix_index(start + 1 * n)].setRGB(r, g, b);
      leds[fix_index(start - 1 * n)].setRGB(r, g, b);
      leds[fix_index(start + 1 * (n - 1))].setRGB(0, 0, 0);
      leds[fix_index(start - 1 * (n - 1))].setRGB(0, 0, 0);
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

void cylon_eye_piece(int i) {
  for (int n = 0; n < CYLON_EYE_HALF_WIDTH; ++n) {
    if ((i + n) < USABLE_WIDTH + USABLE_START) {
      leds[fix_index(i + n)].setRGB(redLookup[n], 0, 0);
    }
    if ((i - n) >= USABLE_START) {
      leds[fix_index(i - n)].setRGB(redLookup[n], 0, 0);
    }
  }
}

void cylon(uint32_t time) {
  unsigned long now = millis();

  while (millis() - now < time) {
    for (uint32_t i = USABLE_START; i < USABLE_WIDTH; i++) {
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