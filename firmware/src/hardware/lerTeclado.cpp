#include "lerTeclado.h"
#include <avr/io.h>
#include "bits.h"
#include "timer.h"
#include <util/delay.h>

const uint8_t pinosLinhas[4] = {PB4, PB2, PB1, PB0};
const uint8_t pinosColunas[4] = {PB5, PD7, PC3, PC2};

const char teclas[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char lerTeclado(void) {
  static char ultimaTecla = '\0';
  char teclaPressionada = '\0';
  static char ultimaLeitura = '\0';
  static uint32_t tempoAnterior = 0;

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
  if (teclaPressionada != ultimaLeitura) {
    ultimaLeitura = teclaPressionada;
    tempoAnterior = tempoAtual();
    return '\0';
  }

  if (tempoAtual() - tempoAnterior >= 20) {
    if (ultimaTecla != teclaPressionada) {
      ultimaTecla = teclaPressionada;
      return teclaPressionada;
  }
  
  }

  return '\0';
}

void configurarTeclado()  {
  // Definindo as linhas como saídas (Reg. DDRx = 1)
  set_bit(DDRB, PB4); // Pino 12 (PB4) - Linha 1
  set_bit(DDRB, PB2); // Pino 10 (PB2) - Linha 2
  set_bit(DDRB, PB1); // Pino 09 (PB1) - Linha 3
  set_bit(DDRB, PB0); // Pino 08 (PB0) - Linha 4
  
  // Configurando saídas em nível lógico alto (Reg. PORTx = 1)
  set_bit(PORTB, PB4); // Pino 12 (PB4) - Linha 1
  set_bit(PORTB, PB2); // Pino 10 (PB2) - Linha 2
  set_bit(PORTB, PB1); // Pino 09 (PB1) - Linha 3
  set_bit(PORTB, PB0); // Pino 08 (PB0) - Linha 4
  
  // Definindo as colunas como entradas (Reg. DDRx = 0)
  clr_bit(DDRB, PB5); // Pino 13 (PB5) - Coluna 1
  clr_bit(DDRD, PD7); // Pino 07 (PD7) - Coluna 2
  clr_bit(DDRC, PC3); // Pino A3 (PC3) - Coluna 3
  clr_bit(DDRC, PC2); // Pino A2 (PC2) - Coluna 4
  
  // Configurando as colunas como input pull up (Reg. PORTx = 1)
  set_bit(PORTB, PB5); // Pino 13 (PB5) - Coluna 1
  set_bit(PORTD, PD7); // Pino 07 (PD7) - Coluna 2
  set_bit(PORTC, PC3); // Pino A3 (PC3) - Coluna 3
  set_bit(PORTC, PC2); // Pino A2 (PC2) - Coluna 4
}

// Linhas serão saídas e colunas entradas

// Grupo B (13 até 8)
// Pino 13 -> PB5
// Pino 12 -> PB4
// Pino 11 -> PB3
// Pino 10 -> PB2
// Pino 09 -> PB1
// Pino 08 -> PB0

// Grupo C (A5 até A0)
// Pino A5 -> PC5
// Pino A4 -> PC4
// Pino A3 -> PC3
// ...

// Grupo D (7 até 0)
// Pino 07 -> PD7
// Pino 06 -> PD6
// ...
