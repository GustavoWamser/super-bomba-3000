#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../hardware/adc.h"
#include "../hardware/lerTeclado.h"
#include "../hardware/bits.h"
#include "quiz.h"

#define TOTAL_PERGUNTAS 10
#define PERGUNTAS_POR_RODADA 5

struct PerguntaQuiz {
  const char *pais;
  const char *alternativaA;
  const char *alternativaB;
  const char *alternativaC;
  const char *alternativaD;
  char alternativaCerta;
};

static uint8_t sorteadas[TOTAL_PERGUNTAS];
static uint8_t numero = 0;
static uint8_t indice;
static bool perguntaExibida = false;

const struct PerguntaQuiz perguntas[TOTAL_PERGUNTAS] = {
  {"Brasil", "A) Buenos Aires", "B) Brasilia", "C) Lima", "D) Bogota", 'B'},
  {"Argentina", "A) Santiago", "B) Montevideu", "C) Buenos Aires", "D) Assuncao", 'C'},
  {"Franca", "A) Paris", "B) Roma", "C) Madrid", "D) Lisboa", 'A'},
  {"Japao", "A) Seul", "B) Pequim", "C) Tóquio", "D) Bangkok", 'C'},
  {"Australia", "A) Sydney", "B) Canberra", "C) Melbourne", "D) Wellington", 'B'},
  {"Canada", "A) Toronto", "B) Vancouver", "C) Ottawa", "D) Montreal", 'C'},
  {"Egito", "A) Cairo", "B) Alexandria", "C) Giza", "D) Luxor", 'A'},
  {"Italia", "A) Milao", "B) Veneza", "C) Napoles", "D) Roma", 'D'},
  {"Portugal", "A) Porto", "B) Lisboa", "C) Coimbra", "D) Braga", 'B'},
  {"Chile", "A) Valparaiso", "B) Santiago", "C) Concepcion", "D) Antofagasta", 'B'}
};

static uint8_t sorteiaPergunta(uint8_t sorteadas[]) {
  uint8_t indice;
  do {
    indice = rand() % TOTAL_PERGUNTAS;
  } while (sorteadas[indice]);

  sorteadas[indice] = 1;
  return indice;
}

void reiniciarQuiz(void) {
  for (uint8_t i = 0; i < TOTAL_PERGUNTAS; i++) {
    sorteadas[i] = 0;
  }
  numero = 0;
  indice = 0;
  perguntaExibida = false;
}

StatusFase quizFase() {
  if (numero >= PERGUNTAS_POR_RODADA) {
    Serial.println(F("Fase de capitais concluida!"));
    return FASE_VENCEU;
  }

  if (!perguntaExibida) {
    indice = sorteiaPergunta(sorteadas);
    const PerguntaQuiz &pergunta = perguntas[indice];

    Serial.println();
    Serial.print(F("Pergunta "));
    Serial.print(numero + 1);
    Serial.print(F("/"));
    Serial.println(PERGUNTAS_POR_RODADA);
    Serial.print(F("Qual e a capital de "));
    Serial.print(pergunta.pais);
    Serial.println(F("?"));
    Serial.println(pergunta.alternativaA);
    Serial.println(pergunta.alternativaB);
    Serial.println(pergunta.alternativaC);
    Serial.println(pergunta.alternativaD);
    Serial.println(F("Digite A, B, C ou D:"));

    perguntaExibida = true;
  }

  char resposta = lerTeclado();

  if (resposta < 'A' || resposta > 'D') {
    return FASE_RODANDO;
  }

  if (resposta != perguntas[indice].alternativaCerta) {
    Serial.println(F("Resposta errada! Tente novamente."));
    return FASE_ERROU;
  }

  Serial.println(F("Acertou!"));
  numero++;
  perguntaExibida = false;

  if (numero >= PERGUNTAS_POR_RODADA) {
    Serial.println(F("Fase de capitais concluida!"));
    return FASE_VENCEU;
  }

  return FASE_RODANDO;
}
