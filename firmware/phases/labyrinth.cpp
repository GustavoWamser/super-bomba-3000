#include <avr/io.h>
#include <util/delay.h>
#include "../hardware/adc.h"
#include "../hardware/bits.h"
#include "../hardware/lerTeclado.h"

const char mapas [3][8][8] = {
  {{'+', '-', '-', '-', '-', '-', '-', '+'}, 
   {'|', ' ', '#', ' ', ' ', ' ', '#', '|'}, 
   {'|', ' ', '#', ' ', '#', ' ', ' ', '|'}, 
   {'|', ' ', ' ', ' ', '#', '#', ' ', '|'}, 
   {'|', ' ', ' ', ' ', ' ', '#', ' ', '|'}, 
   {'|', ' ', '#', '#', ' ', '#', ' ', '|'}, 
   {'|', ' ', ' ', '#', ' ', '#', 'F', '|'}, 
   {'+', '-', '-', '-', '-', '-', '-', '+'}},
    
  {{'+', '-', '-', '-', '-', '-', '-', '+'}, 
   {'|', ' ', ' ', ' ', ' ', ' ', '#', '|'}, 
   {'|', '#', '#', ' ', '#', ' ', ' ', '|'}, 
   {'|', ' ', ' ', ' ', '#', '#', ' ', '|'}, 
   {'|', ' ', '#', ' ', '#', ' ', ' ', '|'}, 
   {'|', ' ', '#', ' ', ' ', '#', ' ', '|'}, 
   {'|', ' ', 'F', '#', ' ', '#', '#', '|'}, 
   {'+', '-', '-', '-', '-', '-', '-', '+'}},
    
  {{'+', '-', '-', '-', '-', '-', '-', '+'}, 
   {'|', ' ', ' ', '#', ' ', ' ', ' ', '|'}, 
   {'|', '#', ' ', ' ', ' ', '#', ' ', '|'}, 
   {'|', '#', '#', '#', '#', ' ', ' ', '|'}, 
   {'|', ' ', ' ', ' ', ' ', ' ', ' ', '|'}, 
   {'|', ' ', '#', '#', '#', ' ', ' ', '|'}, 
   {'|', ' ', ' ', ' ', 'F', '#', ' ', '|'}, 
   {'+', '-', '-', '-', '-', '-', '-', '+'}},
};

// Posicao inicial do jogador nos labirintos
uint8_t jogadorLinha = 1;
uint8_t jogadorColuna = 1;

// Variável que vai guardar o mapa sorteado
uint8_t mapaAtual;

// Variável que vai dizer se o labirinto já foi concluido
uint8_t estadoJogo = 0;

void iniciarLabirinto(void) {
  srand(meu_analogRead(0));
  mapaAtual = rand() % 3;
  jogadorLinha = 1;
  jogadorColuna = 1;
  desenharLabirinto();
}

void desenharLabirinto(){
  Serial.println();
  Serial.println();
  for (uint8_t l = 0; l < 8; l++) {
    for (uint8_t c = 0; c < 8; c++) {
      if ((jogadorLinha == l) && (jogadorColuna == c)) {
      	Serial.print('@');
      }
      
      else Serial.print(mapas[mapaAtual][l][c]);
      
      Serial.print(' ');
    }
    Serial.println();
  }
}

uint8_t atualizarLabirinto(char Tecla) {
  int8_t proxLinha = jogadorLinha;
  int8_t proxColuna = jogadorColuna;
  
  switch (Tecla) {
  	case '2':
    	proxLinha--;
    	break;
    
    case '8':
    	proxLinha++;
    	break;
          
    case '4':
    	proxColuna--;
    	break;
    
    case '6':
    	proxColuna++;
    	break;
    
    default:
    	return 0;
  }
  if (mapas[mapaAtual][proxLinha][proxColuna] == ' ') {
  	jogadorLinha = proxLinha;
    jogadorColuna = proxColuna;
    desenharLabirinto();
    return 0;
  }
  
  if (mapas[mapaAtual][proxLinha][proxColuna] == 'F') {
  	jogadorLinha = proxLinha;
    jogadorColuna = proxColuna;
    desenharLabirinto();
    estadoJogo = 1;
    return 1;
  }
  return 0;
}

bool labirintoFase() {
  if (estadoJogo) {
    return true;
  }

  char tecla = lerTeclado();

  if (tecla != '\0') {
    estadoJogo = atualizarLabirinto(tecla);
  }

  return estadoJogo;
}