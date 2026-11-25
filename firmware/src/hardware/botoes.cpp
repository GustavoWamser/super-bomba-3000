#include "botoes.h"
#include <Arduino.h>
#include <avr/io.h>

#include "adc.h"
#include "bits.h"
#include "timer.h"

// Valores medidos no hardware (ADC 0..1023), na ordem dos indices 0..3
static const uint16_t CENTRO[4]   = {830, 639, 448, 280};
static const uint16_t MARGEM      = 40;   // janela = centro +/- 40
static const uint16_t REPOUSO_MIN = 950;  // a partir daqui, nenhum botao

static const int8_t FAIXA_REPOUSO    = -1;
static const int8_t FAIXA_INDEFINIDA = -2;

static const uint16_t TEMPO_DEBOUNCE = 25;

static int8_t ultimaLeitura = -1;
static int8_t botaoEstavel = -1;
static uint32_t mudouEm = 0;

static int8_t classificarFaixa(uint16_t leitura) {
  if (leitura >= REPOUSO_MIN) return FAIXA_REPOUSO;

  for (uint8_t i = 0; i < 4; i++) {
    if (leitura >= CENTRO[i] - MARGEM && leitura <= CENTRO[i] + MARGEM) {
      return i;
    }
  }
  return FAIXA_INDEFINIDA;
}

void configurarBotoes(void) {
  clr_bit(DDRC, PC1);
  set_bit(PORTC, PC1);
  set_bit(DIDR0, ADC1D);
}


int8_t lerAperto(void) {
  int8_t leitura = classificarFaixa(meu_analogRead(1));
  uint32_t agora = tempoAtual();

  if (leitura == FAIXA_INDEFINIDA) {
    ultimaLeitura = FAIXA_INDEFINIDA;
    mudouEm = agora;
    return -1;
  }

  if (leitura != ultimaLeitura) {
    ultimaLeitura = leitura;
    mudouEm = agora;
    return -1;
  }

  if (agora - mudouEm < TEMPO_DEBOUNCE) return -1;
  if (leitura == botaoEstavel) return -1;

  int8_t anterior = botaoEstavel;
  botaoEstavel = leitura;
  // So conta como aperto novo se saiu do repouso
  if (leitura >= 0 && anterior == -1) return leitura;
  return -1;
}

int8_t botaoAtual(void) {
  return botaoEstavel;
}