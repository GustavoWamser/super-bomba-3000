#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../hardware/adc.h"
#include "../hardware/lerTeclado.h"
#include "../hardware/bits.h"

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

void setup() {
  Serial.begin(9600);
  configurarTeclado();
  configurarADC();
  lerTeclado();
  srand(meu_analogRead(0));
}

void loop() {
  quizFase();
  _delay_ms(1000);
}

uint8_t sorteiaPergunta(uint8_t sorteadas[]) {
  uint8_t indice;
  do {
    indice = rand() % TOTAL_PERGUNTAS;
  } while (sorteadas[indice]);

  sorteadas[indice] = 1;
  return indice;
}

void mostraPergunta(uint8_t numero, const struct PerguntaQuiz &pergunta) {
  Serial.println();
  Serial.print("Pergunta ");
  Serial.print(numero);
  Serial.print("/");
  Serial.println(PERGUNTAS_POR_RODADA);
  Serial.print("Qual e a capital de ");
  Serial.print(pergunta.pais);
  Serial.println("?");
  Serial.println(pergunta.alternativaA);
  Serial.println(pergunta.alternativaB);
  Serial.println(pergunta.alternativaC);
  Serial.println(pergunta.alternativaD);
  Serial.println("Digite A, B, C ou D:");

  while (1) {
    char resposta = lerTeclado();

    if (resposta >= 'A' && resposta <= 'D') {
      if (resposta == pergunta.alternativaCerta) {
        Serial.println("Acertou!");
        return;
      }

      Serial.println("Resposta errada! Tente novamente.");
    }
  }
}

void quizFase() {
  uint8_t sorteadas[TOTAL_PERGUNTAS] = {0};

  for (uint8_t numero = 1; numero <= PERGUNTAS_POR_RODADA; numero++) {
    uint8_t indice = sorteiaPergunta(sorteadas);
    mostraPergunta(numero, perguntas[indice]);
  }

  Serial.println("Fase de capitais concluida!");
}