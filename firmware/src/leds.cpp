#include "leds.h"
#include "FastLED.h"
#include "GMservice.h"

CRGB leds[6];
GMservice* _gm;

void init_leds(GMservice* gm) {
  FastLED.addLeds<NEOPIXEL, 23>(leds, 6);
  _gm = gm;
}

void error_led_tick() {
  bool is_sd_ok = /*_gm->periph_state.service_sd && */_gm->periph_state.user_sd;
  if (!is_sd_ok) {
    leds[0] = (millis() / 500) % 2 == 0 ? CRGB::Red : CRGB::Black;
  } else {
    leds[0] = CRGB::Black;
  }
}

void batt_leds_tick() {
  float voltage_pct = min((int)map(analogRead(39), 400, 1024, 0, 100), 100);
  if (voltage_pct > 90) {
    leds[3] = CRGB::Green;
    leds[4] = CRGB::Green;
    leds[5] = CRGB::Green;
  } else if (voltage_pct > 60) {
    leds[3] = CRGB::Orange;
    leds[4] = CRGB::Orange;
    leds[5] = CRGB::Black;  
  } else if (voltage_pct > 30) {
    leds[3] = CRGB::Orange;
    leds[4] = CRGB::Red;
    leds[5] = CRGB::Black;
  } else {
    leds[3] = (millis() / 500) % 2 == 0 ? CRGB::Red : CRGB::Black;
    leds[4] = (millis() / 500) % 2 == 0 ? CRGB::Red : CRGB::Black;
    leds[5] = (millis() / 500) % 2 == 0 ? CRGB::Red : CRGB::Black;
  }
}

void gps_led_tick() {
  if (_gm->gps->satellites.value() > 0) {
    leds[2] = CRGB::Blue;
  } else {
    leds[2] = (millis() / 500) % 2 == 0 ? CRGB::Blue : CRGB::Red;
  }
}

void gm_led_tick() {
  if (_gm->state != _states::_work) {
    leds[1] = CRGB::Black;
  } else {
    leds[1] = (millis() / 500) % 2 == 0 ? CRGB::Green : CRGB::Black;
  }
}

void blink() {
  static uint32_t time_label = 0;
  if (millis() - time_label < 400) return;
  time_label = millis();
  error_led_tick();
  batt_leds_tick();
  gps_led_tick();
  gm_led_tick();
  FastLED.show();
}
