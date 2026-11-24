#include <Arduino.h>
#include "reflex.h"

#include <avr/io.h>
#include <stdlib.h>

#include "../hardware/botoes.h"
#include "../hardware/leds.h"
#include "../hardware/timer.h"

namespace {

constexpr uint32_t TEMPO_MAXIMO = 2000;
constexpr uint8_t ACERTOS_PARA_PASSAR = 7;
constexpr uint32_t PAUSA_ENTRE_LEDS = 400;
int8_t ultimoLed = -1;   // guarda o LED da rodada anterior
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
  apagarTodosLeds();
  acertos = 0;
  sorteado = -1;
  ultimoLed = -1;
  referenciaTempo = tempoAtual();
  estado = REFLEX_PREPARANDO;
  Serial.println(F("Fase Reflex iniciando..."));
}

StatusFase reflexFase(void) {
  int8_t aperto = lerAperto();
  uint32_t agora = tempoAtual();

  if (estado == REFLEX_PREPARANDO) {
    if (agora - referenciaTempo < PAUSA_ENTRE_LEDS) return FASE_RODANDO;
    estado = REFLEX_ESPERANDO;
  }

  if (sorteado == -1) {
    sorteado = sortearLedDiferente(ultimoLed);
    ultimoLed = sorteado;
    acenderLed(sorteado);
    referenciaTempo = agora;
  }

  if (aperto == -1) {
    if (agora - referenciaTempo >= TEMPO_MAXIMO) {
      apagarTodosLeds();
      Serial.println(F("Demorou demais!"));
      return FASE_ERROU;
    }
    return FASE_RODANDO;
  }

  apagarTodosLeds();
  if (aperto != sorteado) {
    Serial.println(F("Botao errado!"));
    return FASE_ERROU;
  }

  acertos++;
  Serial.print(F("Acertou! "));
  Serial.print(acertos);
  Serial.print(F("/"));
  Serial.println(ACERTOS_PARA_PASSAR);

  if (acertos >= ACERTOS_PARA_PASSAR) {
    Serial.println(F("Fase Reflex concluida!"));
    return FASE_VENCEU;
  }

  sorteado = -1;
  estado = REFLEX_PREPARANDO;
  referenciaTempo = agora;
  return FASE_RODANDO;
}
