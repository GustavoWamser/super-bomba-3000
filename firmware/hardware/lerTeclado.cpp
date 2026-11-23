#include "lerTeclado.h"
#include <avr/io.h>
#include "bits.h"

char lerTeclado(void) {
  static char ultimaTecla = '\0';
  char teclaPressionada = '\0';

  for (int linha = 0; linha < 4; linha++) {
    clr_bit(PORTB, pinosLinhas[linha]);
    _delay_us(1);

    for (int coluna = 0; coluna < 4; coluna++) {
      if (coluna < 1) {
        if (!(tst_bit(PINB, pinosColunas[coluna]))) {
          teclaPressionada = teclas[linha][coluna];
        }
      } else if (coluna < 2) {
        if (!(tst_bit(PIND, pinosColunas[coluna]))) {
          teclaPressionada = teclas[linha][coluna];
        }
      } else {
        if (!(tst_bit(PINC, pinosColunas[coluna]))) {
          teclaPressionada = teclas[linha][coluna];
        }
      }
    }

    set_bit(PORTB, pinosLinhas[linha]);
  }

  if (ultimaTecla != teclaPressionada) {
    ultimaTecla = teclaPressionada;
    return teclaPressionada;
  }

  return '\0';
}