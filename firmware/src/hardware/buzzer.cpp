#include "buzzer.h"

#include <Arduino.h>

static const uint8_t PINO_BUZZER = 11;
static uint16_t ultimoSegundoAvisado = 0;

void configurarBuzzer(void) {
  pinMode(PINO_BUZZER, OUTPUT);
  desligarBuzzer();
}

void avisarTempoBuzzer(uint16_t segundos) {
  if (segundos > 0 && segundos <= 10 && segundos != ultimoSegundoAvisado) {
    ultimoSegundoAvisado = segundos;
    tone(PINO_BUZZER, 2200, 120);
  }
}

void tocarAlarmeBuzzer(void) {
  tone(PINO_BUZZER, 1000, 3000);
}

void desligarBuzzer(void) {
  noTone(PINO_BUZZER);
  ultimoSegundoAvisado = 0;
}
