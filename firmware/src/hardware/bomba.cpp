#include "bomba.h"

#include <avr/interrupt.h>
#include <avr/io.h>

static volatile bool acionamento = false;

ISR(INT0_vect) {
  acionamento = true;
}

void configurarBomba(void) {

  cli();

  DDRD &= ~(1 << DDD2);
  PORTD |= (1 << PORTD2);

  EICRA = (1 << ISC01) | (1 << ISC00); // INT0 na borda de subida

  sei();
}

bool fioDesconectado(void) {
  return (PIND & (1 << PIND2)) != 0;
}

bool bombaAcionada(void) {
  cli();
  bool resultado = acionamento;
  acionamento = false;
  sei();
  return resultado;
}

void bombaReset(void) {
  cli();
  acionamento = false;
  sei();
}
