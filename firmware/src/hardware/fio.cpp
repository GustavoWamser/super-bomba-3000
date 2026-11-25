#include "fio.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

// Fio entre o pino 2 (PD2/INT0) e o GND; a entrada usa pull-up interno.
static volatile bool fioPuxado = false;

ISR(INT0_vect) {
  fioPuxado = true;
}

void configurarFio(void) {
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);
  EIMSK &= ~(1 << INT0);
  EICRA = (EICRA & ~((1 << ISC01) | (1 << ISC00))) |
           (1 << ISC01) | (1 << ISC00); // Borda de subida: fio removido.
}

bool armarFio(void) {
  bool conectado;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    EIMSK &= ~(1 << INT0);
    fioPuxado = false;
    EIFR = (1 << INTF0);
    conectado = !(PIND & (1 << PD2));
    if (conectado) EIMSK |= (1 << INT0);
  }
  return conectado;
}

bool fioFoiPuxado(void) {
  bool puxado;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    puxado = fioPuxado || (EIFR & (1 << INTF0)) || (PIND & (1 << PD2));
  }
  return puxado;
}

bool desarmarFio(void) {
  bool puxado;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    EIMSK &= ~(1 << INT0);
    puxado = fioPuxado || (EIFR & (1 << INTF0)) || (PIND & (1 << PD2));
    fioPuxado = false;
    EIFR = (1 << INTF0);
  }
  return puxado;
}
