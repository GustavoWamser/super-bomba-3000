#include <Arduino.h>
#include "reflex.h"

#include <avr/io.h>
#include <stdlib.h>

#include "../hardware/botoes.h"
#include "../hardware/leds.h"
#include "../hardware/timer.h"

namespace {

constexpr uint32_t TEMPO_MAXIMO = 1000;
constexpr uint8_t ACERTOS_PARA_PASSAR = 7;
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
    sorteado = sortearLedDiferente(sorteado);
    acenderLed(sorteado);
    referenciaTempo = agora;
  }

  if (aperto == -1) {
    if (agora - referenciaTempo >= TEMPO_MAXIMO) {
      apagarTodosLeds();
      Serial.println("Demorou demais!");
      return FASE_ERROU;
    }
    return FASE_RODANDO;
  }

  apagarTodosLeds();
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
