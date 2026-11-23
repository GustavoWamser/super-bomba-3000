#include "botoes.h"

#include <avr/io.h>

#include "adc.h"
#include "bits.h"
#include "timer.h"

static const uint16_t TEMPO_DEBOUNCE = 15;
static int8_t ultimaLeitura = -1;
static int8_t botaoEstavel = -1;
static uint32_t mudouEm = 0;

static int8_t lerFaixaAdc(void) {
  uint16_t leitura = meu_analogRead(1);

  if (leitura >= 921) return -1;
  if (leitura >= 716) return 0;
  if (leitura >= 512) return 1;
  if (leitura >= 307) return 2;
  return 3;
}

void configurarBotoes(void) {
  clr_bit(DDRC, PC1);
  set_bit(PORTC, PC1);
  set_bit(DIDR0, ADC1D);
}

int8_t lerAperto(void) {
  int8_t leitura = lerFaixaAdc();
  uint32_t agora = tempoAtual();

  if (leitura != ultimaLeitura) {
    ultimaLeitura = leitura;
    mudouEm = agora;
    return -1;
  }

  if (agora - mudouEm < TEMPO_DEBOUNCE) return -1;
  if (leitura == botaoEstavel) return -1;

  botaoEstavel = leitura;
  return botaoEstavel >= 0 ? botaoEstavel : -1;
}

int8_t botaoAtual(void) {
  return botaoEstavel;
}
