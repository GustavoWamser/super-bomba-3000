#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hardware/lerTeclado.h"
#include "hardware/adc.h"
#include "system/menu.h"

enum Fase {
  LABIRINTO,
  MATH,
  MEMORY,
  QUIZ,
  REFLEX,
  TOTAL_FASES
};

static Fase ordemFases[TOTAL_FASES];
static uint8_t quantidadeFases = 0;
static uint8_t indiceFaseAtual = 0;
static bool jogoIniciado = false;
static bool faseInicializada = false;

bool labirintoFase(void);
void iniciarLabirinto(void);
bool mathFase(void);
bool quizFase(void);
uint8_t fase_genius(void);
uint8_t fase_reflexo(void);

static void sortearOrdemFases(void) {
  for (uint8_t i = 0; i < TOTAL_FASES; i++) {
    ordemFases[i] = (Fase)i;
  }
  for (uint8_t i = TOTAL_FASES - 1; i > 0; i--) {
    uint8_t j = rand() % (i + 1);
    Fase temporaria = ordemFases[i];
    ordemFases[i] = ordemFases[j];
    ordemFases[j] = temporaria;
  }
}

static void iniciarJogo(char dificuldade) {
  if (dificuldade == '1') {
    quantidadeFases = 3;
  } else if (dificuldade == '2') {
    quantidadeFases = 4;
  } else {
    quantidadeFases = 5;
  }

  sortearOrdemFases();
  indiceFaseAtual = 0;
  jogoIniciado = true;
  faseInicializada = false;

  Serial.println("Jogo iniciado!");
}

void setup() {
  Serial.begin(9600);
  configurarTeclado();
  configurarADC();
  srand(meu_analogRead(0));
}

void loop() {
  if (!jogoIniciado) {
    char dificuldade = menu();
    if (dificuldade != '\0') {
      iniciarJogo(dificuldade);
    }
    return;
  }

  if (indiceFaseAtual >= quantidadeFases) {
    Serial.println("Parabens! Voce concluiu o jogo.");
    jogoIniciado = false;
    faseInicializada = false;
    return;
  }

  Fase faseAtual = ordemFases[indiceFaseAtual];
  if (!faseInicializada) {
    if (faseAtual == LABIRINTO) {
      iniciarLabirinto();
    }
    faseInicializada = true;
  }

  bool faseConcluida = false;
  if (faseAtual == LABIRINTO) {
    faseConcluida = labirintoFase();
  } else if (faseAtual == MATH) {
    faseConcluida = mathFase();
  } else if (faseAtual == QUIZ) {
    faseConcluida = quizFase();
  } else if (faseAtual == MEMORY || faseAtual == REFLEX) {
    // memory/reflex ainda são programas independentes com main() próprio.
  }

  if (faseConcluida) {
    indiceFaseAtual++;
    faseInicializada = false;
  }
}
