#pragma once

#include <stdbool.h>

// Configura PD2/INT0 como entrada com pull-up e interrupcao na subida.
void configurarBomba(void);

// Le o nivel atual do fio: true significa fio desconectado (nivel alto).
bool fioDesconectado(void);

// Consome o evento sinalizado pela ISR. Retorna true uma unica vez por evento.
bool bombaAcionada(void);

// Descarta eventos anteriores ao inicio de uma nova partida.
void bombaReset(void);
