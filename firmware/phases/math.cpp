#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../hardware/adc.h"
#include "../hardware/lerTeclado.h"
#include "../hardware/bits.h"

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

void setup() {
  Serial.begin(9600);
  configurarTeclado();
  configurarADC();
  lerTeclado();
  srand(meu_analogRead(0));
}

void loop() {
  mathFase();
  _delay_ms(1000);
}

uint8_t sorteiaOperacao(uint8_t sorteadas[]) {
  uint8_t indice;

  do {
    indice = rand() % TOTAL_OPERACOES;
  } while (sorteadas[indice]);

  sorteadas[indice] = 1;
  return indice;
}

uint32_t lerResposta() {
  uint32_t resposta = 0;
  uint8_t quantidade = 0;

  Serial.println("Digite a resposta e pressione #:");

  while (1) {
    char tecla = lerTeclado();

    if (tecla >= '0' && tecla <= '9') {
      if (quantidade < 5) {
        resposta = resposta * 10 + (tecla - '0');
        quantidade++;
        Serial.print(tecla);
      }
    } else if (tecla == '*' && quantidade > 0) {
      resposta /= 10;
      quantidade--;
      Serial.print("\b \b");
    } else if (tecla == '#' && quantidade > 0) {
      Serial.println();
      return resposta;
    }
  }
}

uint8_t mostrarOperacao(uint8_t numero, uint8_t indice) {
  const Operacao &operacao = operacoes[indice];
  uint32_t resposta;

  Serial.println();
  Serial.print("Operacao ");
  Serial.print(numero);
  Serial.print("/");
  Serial.println(OPERACOES_POR_FASE);
  Serial.println("Calcule:");
  Serial.println(operacao.conta);

  while (1) {
    resposta = lerResposta();

    if (resposta == operacao.resultado) {
      Serial.println("Acertou!");
      return 1;
    }

    Serial.println("Errou! Tente novamente.");
  }
}

void mathFase() {
  uint8_t sorteadas[TOTAL_OPERACOES] = {0};
  uint8_t acertos = 0;

  for (uint8_t numero = 1; numero <= OPERACOES_POR_FASE; numero++) {
    uint8_t indice = sorteiaOperacao(sorteadas);
    acertos += mostrarOperacao(numero, indice);
  }

  Serial.println();
  Serial.print("Pontuacao: ");
  Serial.print(acertos);
  Serial.print("/");
  Serial.println(OPERACOES_POR_FASE);

  Serial.println("Passou de fase!");
}
