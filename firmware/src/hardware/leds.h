#pragma once

#include <stdint.h>

void configurarLeds(void);
void acenderLed(uint8_t led);
void apagarLed(uint8_t led);
void apagarTodosLeds(void);
void alternarTodosLeds(void);
uint8_t sortearLedDiferente(int8_t anterior);
