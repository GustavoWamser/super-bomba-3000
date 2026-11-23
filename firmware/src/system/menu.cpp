#include <Arduino.h>
#include "menu.h"
#include "../hardware/lerTeclado.h"

char menu(void) {
  static bool telaExibida = false;

  if (!telaExibida) {
    Serial.println();
    Serial.println(F("=== SUPER BOMBA 3000 ==="));
    Serial.println(F("1 - Facil (3 fases)"));
    Serial.println(F("2 - Normal (4 fases)"));
    Serial.println(F("3 - Dificil (5 fases)"));
    Serial.println(F("Escolha a dificuldade:"));
    telaExibida = true;
  }

  char escolha = lerTeclado();
  if (escolha == '1' || escolha == '2' || escolha == '3') {
    telaExibida = false;
    return escolha;
  }

  return '\0';
}
