#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>

static volatile uint16_t milissegundos = 0;
static volatile bool passou1s = false;

ISR(TIMER1_COMPA_vect) {
  milissegundos++;

  if (milissegundos >= 1000) {
    milissegundos = 0;
    passou1s = true;
  }
}

void configurarTimer(void) {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 249; // 16 MHz / 64 / 250 = 1 ms
  TIMSK1 = (1 << OCIE1A);
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
  sei();
}

bool timerPassou1s(void) {
  cli();
  bool resultado = passou1s;
  passou1s = false;
  sei();
  return resultado;
}
