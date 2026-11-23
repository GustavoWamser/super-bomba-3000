#include "reflex.h"

#include <avr/io.h>
#include <stdlib.h>

#include "../hardware/botoes.h"
#include "../hardware/timer.h"

namespace {

constexpr uint32_t TEMPO_MAXIMO = 1000;
constexpr uint8_t ACERTOS_PARA_PASSAR = 7;
constexpr uint8_t LEDS = 0b01111000;
const uint8_t LED[4] = {PD6, PD5, PD4, PD3};

enum EstadoReflex {
  REFLEX_PREPARANDO,
  REFLEX_ESPERANDO
};

EstadoReflex estado = REFLEX_PREPARANDO;
uint8_t acertos = 0;
int8_t sorteado = -1;
uint32_t referenciaTempo = 0;

}

void iniciarReflex(void) {
  DDRD |= LEDS;
  PORTD &= ~LEDS;
  acertos = 0;
  sorteado = -1;
  referenciaTempo = tempoAtual();
  estado = REFLEX_PREPARANDO;
  Serial.println("Fase Reflex iniciando...");
}

StatusFase reflexFase(void) {
  int8_t aperto = lerAperto();
  uint32_t agora = tempoAtual();

  if (estado == REFLEX_PREPARANDO) {
    if (agora - referenciaTempo < 500) return FASE_RODANDO;
    estado = REFLEX_ESPERANDO;
  }

  if (sorteado == -1) {
    int8_t novo = rand() % 4;
    while (novo == sorteado) novo = rand() % 4;
    sorteado = novo;
    PORTD |= (1 << LED[sorteado]);
    referenciaTempo = agora;
  }

  if (aperto == -1) {
    if (agora - referenciaTempo >= TEMPO_MAXIMO) {
      PORTD &= ~LEDS;
      Serial.println("Demorou demais!");
      return FASE_ERROU;
    }
    return FASE_RODANDO;
  }

  PORTD &= ~LEDS;
  if (aperto != sorteado) {
    Serial.println("Botao errado!");
    return FASE_ERROU;
  }

  acertos++;
  Serial.print("Acertou! ");
  Serial.print(acertos);
  Serial.print("/");
  Serial.println(ACERTOS_PARA_PASSAR);

  if (acertos >= ACERTOS_PARA_PASSAR) {
    Serial.println("Fase Reflex concluida!");
    return FASE_VENCEU;
  }

  sorteado = -1;
  return FASE_RODANDO;
}
