#include "leds.h"

#include <avr/io.h>
#include <stdlib.h>

namespace {

constexpr uint8_t LEDS = 0b01111000;
const uint8_t PINOS_LED[4] = {PD6, PD5, PD4, PD3};

}

void configurarLeds(void) {
  DDRD |= LEDS;
  apagarTodosLeds();
}

void acenderLed(uint8_t led) {
  if (led < 4) PORTD |= (1 << PINOS_LED[led]);
}

void apagarLed(uint8_t led) {
  if (led < 4) PORTD &= ~(1 << PINOS_LED[led]);
}

void apagarTodosLeds(void) {
  PORTD &= ~LEDS;
}

void alternarTodosLeds(void) {
  PORTD ^= LEDS;
}

uint8_t sortearLedDiferente(int8_t anterior) {
  uint8_t novo;

  do {
    novo = rand() % 4;
  } while (novo == anterior);

  return novo;
}
