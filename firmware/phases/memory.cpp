#include "memory.h"

#include <avr/io.h>
#include <stdlib.h>

#include "../hardware/botoes.h"
#include "../hardware/leds.h"
#include "../hardware/timer.h"

namespace {

constexpr uint8_t TAMANHO_SEQUENCIA = 7;
constexpr uint32_t TEMPO_RESPOSTA = 3000;
constexpr uint32_t TEMPO_LED_ACESO = 500;
constexpr uint32_t TEMPO_LED_APAGADO = 250;
constexpr uint32_t PAUSA_RODADA = 800;
enum EstadoMemory {
  MEMORY_PREPARANDO,
  MEMORY_ACENDE_LED,
  MEMORY_LED_ACESO,
  MEMORY_LED_APAGADO,
  MEMORY_ESPERA_BOTAO,
  MEMORY_SEGURANDO,
  MEMORY_PAUSA_RODADA
};

EstadoMemory estado = MEMORY_PREPARANDO;
uint8_t sequencia[TAMANHO_SEQUENCIA];
uint8_t rodada = 1;
uint8_t posicao = 0;
int8_t apertado = -1;
uint32_t referenciaTempo = 0;

void imprimirRodada(void) {
  Serial.print("Rodada ");
  Serial.print(rodada);
  Serial.print("/");
  Serial.println(TAMANHO_SEQUENCIA);
}

}

void iniciarMemory(void) {
  apagarTodosLeds();

  for (uint8_t i = 0; i < TAMANHO_SEQUENCIA; i++) {
    sequencia[i] = rand() % 4;
  }

  rodada = 1;
  posicao = 0;
  apertado = -1;
  referenciaTempo = tempoAtual();
  estado = MEMORY_PREPARANDO;

  Serial.println("Fase Memory iniciando...");
}

StatusFase memoryFase(void) {
  int8_t aperto = lerAperto();
  uint32_t agora = tempoAtual();

  switch (estado) {
    case MEMORY_PREPARANDO:
      if (agora - referenciaTempo >= 500) {
        imprimirRodada();
        estado = MEMORY_ACENDE_LED;
      }
      break;

    case MEMORY_ACENDE_LED:
      acenderLed(sequencia[posicao]);
      referenciaTempo = agora;
      estado = MEMORY_LED_ACESO;
      break;

    case MEMORY_LED_ACESO:
      if (agora - referenciaTempo >= TEMPO_LED_ACESO) {
        apagarLed(sequencia[posicao]);
        referenciaTempo = agora;
        estado = MEMORY_LED_APAGADO;
      }
      break;

    case MEMORY_LED_APAGADO:
      if (agora - referenciaTempo >= TEMPO_LED_APAGADO) {
        posicao++;
        if (posicao < rodada) {
          estado = MEMORY_ACENDE_LED;
        } else {
          posicao = 0;
          referenciaTempo = agora;
          estado = MEMORY_ESPERA_BOTAO;
        }
      }
      break;

    case MEMORY_ESPERA_BOTAO:
      if (aperto >= 0) {
        apertado = aperto;
        acenderLed(apertado);
        estado = MEMORY_SEGURANDO;
      } else if (agora - referenciaTempo >= TEMPO_RESPOSTA) {
        apagarTodosLeds();
        Serial.println("Demorou demais!");
        return FASE_ERROU;
      }
      break;

    case MEMORY_SEGURANDO:
      if (botaoAtual() == -1) {
        apagarLed(apertado);

        if (apertado != sequencia[posicao]) {
          Serial.println("Botao errado!");
          return FASE_ERROU;
        }

        posicao++;
        if (posicao < rodada) {
          referenciaTempo = agora;
          estado = MEMORY_ESPERA_BOTAO;
        } else if (rodada == TAMANHO_SEQUENCIA) {
          apagarTodosLeds();
          Serial.println("Fase Memory concluida!");
          return FASE_VENCEU;
        } else {
          rodada++;
          posicao = 0;
          referenciaTempo = agora;
          estado = MEMORY_PAUSA_RODADA;
        }
      }
      break;

    case MEMORY_PAUSA_RODADA:
      if (agora - referenciaTempo >= PAUSA_RODADA) {
        imprimirRodada();
        estado = MEMORY_ACENDE_LED;
      }
      break;
  }

  return FASE_RODANDO;
}
