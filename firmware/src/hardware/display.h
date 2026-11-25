#pragma once

#include <stdint.h>

void configurarDisplay(void);
void ligarDisplayJogo(void);
void desligarDisplay(void);
void atualizarVidasDisplay(uint8_t vidas);
void atualizarTempoDisplay(uint16_t segundosTotais);