#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../hardware/adc.h"
#include "../hardware/lerTeclado.h"
#include "../hardware/bits.h"
#include "math.h"

#define TOTAL_OPERACOES 10
#define OPERACOES_POR_FASE 5

struct Operacao {
  const char *conta;
  uint16_t resultado;
};

const Operacao operacoes[TOTAL_OPERACOES] = {
  {"12 + 3 x 4 - 8 / 2", 20},
  {"25 + 6 x 5 - 12 / 3", 51},
  {"7 + 4 x 8 - 18 / 6", 36},
  {"40 + 9 x 3 - 20 / 4", 62},
  {"15 + 2 x 7 - 21 / 3", 22},
  {"9 + 5 x 6 - 16 / 4", 35},
  {"30 + 8 x 2 - 15 / 5", 43},
  {"18 + 3 x 9 - 14 / 2", 38},
  {"50 + 7 x 4 - 24 / 6", 74},
  {"6 + 9 x 5 - 27 / 3", 42}
};


static uint8_t sorteiaOperacao(uint8_t sorteadas[]) {
  uint8_t indice;

  do {
    indice = rand() % TOTAL_OPERACOES;
  } while (sorteadas[indice]);

  sorteadas[indice] = 1;
  return indice;
}

StatusFase mathFase() {
  static uint8_t sorteadas[TOTAL_OPERACOES] = {0};
  static uint8_t numero = 0;
  static uint8_t indice;
  static uint32_t resposta = 0;
  static uint8_t quantidade = 0;
  static bool operacaoExibida = false;

  if (numero >= OPERACOES_POR_FASE) {
    Serial.println("Passou de fase!");
    return FASE_VENCEU;
  }

  if (!operacaoExibida) {
    indice = sorteiaOperacao(sorteadas);

    Serial.println();
    Serial.print("Operacao ");
    Serial.print(numero + 1);
    Serial.print("/");
    Serial.println(OPERACOES_POR_FASE);
    Serial.println("Calcule:");
    Serial.println(operacoes[indice].conta);
    Serial.println("Digite a resposta e pressione #:");

    resposta = 0;
    quantidade = 0;
    operacaoExibida = true;
  }

  char tecla = lerTeclado();

  if (tecla >= '0' && tecla <= '9' && quantidade < 5) {
    resposta = resposta * 10 + (tecla - '0');
    quantidade++;
    Serial.print(tecla);
  } else if (tecla == '*' && quantidade > 0) {
    resposta /= 10;
    quantidade--;
    Serial.print("\b \b");
  } else if (tecla == '#' && quantidade > 0) {
    Serial.println();

    if (resposta == operacoes[indice].resultado) {
      Serial.println("Acertou!");
      numero++;
      operacaoExibida = false;

      if (numero >= OPERACOES_POR_FASE) {
        Serial.println("Passou de fase!");
        return FASE_VENCEU;
      }
    } else {
      Serial.println("Errou! Tente novamente.");
      resposta = 0;
      quantidade = 0;
      return FASE_ERROU;
    }
  }

  return FASE_RODANDO;
}
