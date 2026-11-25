#include <Arduino.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hardware/lerTeclado.h"
#include "hardware/adc.h"
#include "hardware/botoes.h"
#include "hardware/buzzer.h"
#include "hardware/leds.h"
#include "hardware/timer.h"
#include "hardware/display.h"
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
static bool aguardandoReinicio = false;
static uint8_t vidas = 3;
static uint16_t tempo_jogo = 0;

static const __FlashStringHelper *nomeFase(Fase fase) {
  switch (fase) {
    case LABIRINTO: return F("LABIRINTO");
    case MATH:      return F("MATEMATICA");
    case MEMORY:    return F("MEMORIA");
    case QUIZ:      return F("QUIZ DE CAPITAIS");
    case REFLEX:    return F("REFLEXO");
    default:        return F("");
  }
}

// Contagem regressiva antes de Memory e Reflex
static const uint8_t SEGUNDOS_CONTAGEM = 5;
static bool contagemAtiva = false;
static uint8_t contagemRestante = 0;
static uint32_t contagemReferencia = 0;

static void reiniciarPartida(void) {
  desligarBuzzer();
  jogoIniciado = false;
  faseInicializada = false;
  aguardandoReinicio = false;
  contagemAtiva = false;

  vidas = 3;
  tempo_jogo = 0;
  indiceFaseAtual = 0;
  apagarTodosLeds();
  desligarDisplay();
}

static const uint8_t TAM_BUFFER = 8;

static void verificarSerial(void) {
  static char buffer[TAM_BUFFER];
  static uint8_t indice = 0;

  while (Serial.available() > 0) {
    char caractere = Serial.read();

    if (caractere == '\r' || caractere == '\n') {
      if (indice == 0) {
        return; 
      }

      buffer[indice] = '\0';

      if (indice == 1 && (buffer[0] == 'R' || buffer[0] == 'r')) {
        reiniciarPartida();
      } else {
        Serial.println(F("Comando invalido"));
      }

      indice = 0;  
    } else if (indice < TAM_BUFFER - 1) {
      buffer[indice] = caractere;
      indice++;
    }
    // se o buffer encheu, o caractere e descartado e o indice nao avanca
  }
}

static void finalizarJogo(const char *mensagem) {
  jogoIniciado = false;
  faseInicializada = false;
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
  desligarBuzzer();
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
  contagemAtiva = false;
  jogoIniciado = true;
  faseInicializada = false;
  aguardandoReinicio = false;
  ligarDisplayJogo();
  atualizarVidasDisplay(vidas);
  atualizarTempoDisplay(tempo_jogo);
  Serial.println(F("Jogo iniciado!"));
  
}

void setup() {
  Serial.begin(9600);
  configurarTeclado();
  configurarADC();
  configurarBotoes();
  configurarBuzzer();
  configurarLeds();
  configurarDisplay();
  configurarTimer();
  srand(meu_analogRead(0));
}

void loop() {
  verificarSerial();
  if (aguardandoReinicio) {
    if (lerTeclado() != '\0') {
      aguardandoReinicio = false;
      desligarBuzzer();
      desligarDisplay();
    }
    return;
  }

  if (!jogoIniciado) {
    char dificuldade = menu();
    if (dificuldade != '\0') {
      iniciarJogo(dificuldade);
    }
    return;
  }

  if (timerPassou1s() && tempo_jogo > 0) {
    tempo_jogo--;
    atualizarTempoDisplay(tempo_jogo);
  }

  if (tempo_jogo == 0) {
    vidas = 0;
    atualizarVidasDisplay(vidas);
    atualizarTempoDisplay(tempo_jogo);
    tocarAlarmeBuzzer();
    finalizarJogo("A bomba explodiu! Voce foi derrotado.");
    return;
  }

  if (indiceFaseAtual >= quantidadeFases) {
    desligarBuzzer();
    finalizarJogo("Parabens! Voce concluiu o jogo.");
    return;
  }

  avisarTempoBuzzer(tempo_jogo);

  Fase faseAtual = ordemFases[indiceFaseAtual];
  if (!faseInicializada && (faseAtual == MEMORY || faseAtual == REFLEX)) {
  uint32_t agora = tempoAtual();

  if (!contagemAtiva) {
    contagemAtiva = true;
    contagemRestante = SEGUNDOS_CONTAGEM;
    contagemReferencia = agora;
    Serial.print(F("Proxima fase: "));
    Serial.println(nomeFase(faseAtual));
    Serial.println(F("Prepare as maos para apertar os botoes!"));
    Serial.println(contagemRestante);
  }

  lerAperto();

  if (agora - contagemReferencia >= 1000) {
    contagemReferencia = agora;
    contagemRestante--;
    if (contagemRestante > 0) {
      Serial.println(contagemRestante);
    } else {
      contagemAtiva = false;  // acabou: segue para a inicializacao da fase
    }
  }

  if (contagemAtiva) return;
  }
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
    atualizarVidasDisplay(vidas);
    tempo_jogo = tempo_jogo > 10 ? tempo_jogo - 10 : 0;
    atualizarTempoDisplay(tempo_jogo);
    faseInicializada = false;

    Serial.print(F("Erro! Vidas restantes: "));
    Serial.println(vidas);
    Serial.println(F("Voce perdeu 10 segundos."));

    if (vidas == 0 || tempo_jogo == 0) {
      atualizarTempoDisplay(0);
      tocarAlarmeBuzzer();
      finalizarJogo("A bomba explodiu! Voce perdeu o jogo.");
      return;
    }
    avisarTempoBuzzer(tempo_jogo);
  }

  if (statusFase == FASE_VENCEU) {
    indiceFaseAtual++;
    faseInicializada = false;
  }
}
