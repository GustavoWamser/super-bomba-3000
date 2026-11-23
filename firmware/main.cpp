#include "hardware/lerTeclado.h"
#include "hardware/adc.h"
#include <stdlib.h>

enum FaseAtual  {
    LABIRINTO,
    MATH,
    MEMORY,
    CAPITAL,
    REFLEX
};

faseAtual = LABIRINTO

void setup() {
    Serial.begin(9600);
    configurarTeclado();
    configurarADC();
    srand(meu_analogRead(0));
    iniciarLabirinto();
}

void loop   {
    if (faseAtual == LABIRINTO) {
        if (labirintoFaseFase()) {
            fase_atual = MATH;
        }
    }


}