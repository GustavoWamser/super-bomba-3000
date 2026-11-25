#include <Arduino.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hardware/lerTeclado.h"
#include "hardware/adc.h"
#include "hardware/bomba.h"
#include "hardware/botoes.h"
#include "hardware/leds.h"
#include "hardware/timer.h"
#include "phases/labyrinth.h"
#include "phases/math.h"
#include "phases/memory.h"
#include "phases/quiz.h"
#include "phases/reflex.h"
#include "system/fase_status.h"
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
static bool aguardandoInicioFase = false;
static bool mensagemInicioFaseExibida = false;
static bool aguardandoReinicio = false;
static uint8_t vidas = 3;
static uint16_t tempo_jogo = 0;

static void finalizarJogo(const char *mensagem) {
  jogoIniciado = false;
  faseInicializada = false;
  aguardandoInicioFase = false;
  mensagemInicioFaseExibida = false;
  aguardandoReinicio = true;
  Serial.println(mensagem);
  Serial.println(F("Pressione uma tecla para voltar ao menu."));
}

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
    tempo_jogo = 180;
  } else if (dificuldade == '2') {
    quantidadeFases = 4;
    tempo_jogo = 240;
  } else {
    quantidadeFases = 5;
    tempo_jogo = 300;
  }

  sortearOrdemFases();
  indiceFaseAtual = 0;
  vidas = 3;
  jogoIniciado = true;
  faseInicializada = false;
  aguardandoInicioFase = true;
  mensagemInicioFaseExibida = false;
  aguardandoReinicio = false;

  reiniciarMath();
  reiniciarQuiz();

  Serial.println(F("Jogo iniciado!"));
}

void setup() {
  Serial.begin(9600);
  configurarTeclado();
  configurarADC();
  configurarBomba();
  configurarBotoes();
  configurarLeds();
  configurarTimer();
  srand(meu_analogRead(0));
}

void loop() {
  if (aguardandoReinicio) {
    if (lerTeclado() != '\0') {
      aguardandoReinicio = false;
    }
    return;
  }

  if (!jogoIniciado) {
    char dificuldade = menu();
    if (dificuldade != '\0') {
      if (fioDesconectado()) {
        Serial.println(F("Conecte o fio da bomba entre D2 e GND antes de iniciar."));
        return;
      }

      bombaReset();
      iniciarJogo(dificuldade);
    }
    return;
  }

  if (bombaAcionada()) {
    finalizarJogo("A bomba explodiu! O fio foi desconectado.");
    return;
  }

  if (timerPassou1s() && tempo_jogo > 0) {
    tempo_jogo--;
  }

  if (tempo_jogo == 0) {
    finalizarJogo("A bomba explodiu! Voce foi derrotado.");
    return;
  }

  if (indiceFaseAtual >= quantidadeFases) {
    finalizarJogo("Parabens! Voce concluiu o jogo.");
    return;
  }

  if (aguardandoInicioFase) {
    if (!mensagemInicioFaseExibida) {
      Serial.println(F("Pressione um botao para iniciar a proxima fase."));
      mensagemInicioFaseExibida = true;
    }

    if (lerTeclado() == '\0' && lerAperto() < 0) {
      return;
    }

    aguardandoInicioFase = false;
    mensagemInicioFaseExibida = false;
    return;
  }

  Fase faseAtual = ordemFases[indiceFaseAtual];
  if (!faseInicializada) {
    if (faseAtual == LABIRINTO) {
      iniciarLabirinto();
    } else if (faseAtual == MEMORY) {
      iniciarMemory();
    } else if (faseAtual == REFLEX) {
      iniciarReflex();
    }
    faseInicializada = true;
  }

  StatusFase statusFase = FASE_RODANDO;
  if (faseAtual == LABIRINTO) {
    statusFase = labirintoFase();
  } else if (faseAtual == MATH) {
    statusFase = mathFase();
  } else if (faseAtual == QUIZ) {
    statusFase = quizFase();
  } else if (faseAtual == MEMORY) {
    statusFase = memoryFase();
  } else if (faseAtual == REFLEX) {
    statusFase = reflexFase();
  }

  if (statusFase == FASE_ERROU) {
    vidas--;
    tempo_jogo = tempo_jogo > 10 ? tempo_jogo - 10 : 0;

    // O labirinto deve continuar no mesmo mapa e na mesma posicao.
    // Memoria e Reflex reiniciam seus desafios quando a vida e perdida.
    if (faseAtual == MEMORY || faseAtual == REFLEX) {
      faseInicializada = false;
    }

    Serial.print(F("Erro! Vidas restantes: "));
    Serial.println(vidas);
    Serial.println(F("Voce perdeu 10 segundos."));

    if (vidas == 0 || tempo_jogo == 0) {
      finalizarJogo("A bomba explodiu! Voce perdeu o jogo.");
      return;
    }
  }

  if (statusFase == FASE_VENCEU) {
    indiceFaseAtual++;
    faseInicializada = false;
    aguardandoInicioFase = true;
    mensagemInicioFaseExibida = false;
  }
}
