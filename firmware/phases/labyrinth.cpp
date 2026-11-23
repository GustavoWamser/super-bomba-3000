#include <avr/io.h>
#include <util/delay.h>
#include "../hardware/adc.h"
#include "../hardware/bits.h"

const uint8_t pinosLinhas[4] = {PB4, PB2, PB1, PB0};
const uint8_t pinosColunas[4] = {PB5, PD7, PC3, PC2};

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

// Linhas serão saídas e colunas entradas

// Grupo B (13 até 8)
// Pino 13 -> PB5
// Pino 12 -> PB4
// Pino 11 -> PB3
// Pino 10 -> PB2
// Pino 09 -> PB1
// Pino 08 -> PB0

// Grupo C (A5 até A0)
// Pino A5 -> PC5
// Pino A4 -> PC4
// Pino A3 -> PC3
// ...

// Grupo D (7 até 0)
// Pino 07 -> PD7
// Pino 06 -> PD6
// ...

char teclas[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

void setup() {
  Serial.begin(9600);
  
  // Definindo as linhas como saídas (Reg. DDRx = 1)
  set_bit(DDRB, PB4); // Pino 12 (PB4) - Linha 1
  set_bit(DDRB, PB2); // Pino 10 (PB2) - Linha 2
  set_bit(DDRB, PB1); // Pino 09 (PB1) - Linha 3
  set_bit(DDRB, PB0); // Pino 08 (PB0) - Linha 4
  
  // Configurando saídas em nível lógico alto (Reg. PORTx = 1)
  set_bit(PORTB, PB4); // Pino 12 (PB4) - Linha 1
  set_bit(PORTB, PB2); // Pino 10 (PB2) - Linha 2
  set_bit(PORTB, PB1); // Pino 09 (PB1) - Linha 3
  set_bit(PORTB, PB0); // Pino 08 (PB0) - Linha 4
  
  // Definindo as colunas como entradas (Reg. DDRx = 0)
  clr_bit(DDRB, PB5); // Pino 13 (PB5) - Coluna 1
  clr_bit(DDRD, PD7); // Pino 07 (PD7) - Coluna 2
  clr_bit(DDRC, PC3); // Pino A3 (PC3) - Coluna 3
  clr_bit(DDRC, PC2); // Pino A2 (PC2) - Coluna 4
  
  // Configurando as colunas como input pull up (Reg. PORTx = 1)
  set_bit(PORTB, PB5); // Pino 13 (PB5) - Coluna 1
  set_bit(PORTD, PD7); // Pino 07 (PD7) - Coluna 2
  set_bit(PORTC, PC3); // Pino A3 (PC3) - Coluna 3
  set_bit(PORTC, PC2); // Pino A2 (PC2) - Coluna 4
  
  // Configurando o pino ADC para leitura aleatoria no labirinto
  set_bit(ADCSRA, ADEN);  // Habilita o ADC
  set_bit(ADCSRA, ADPS2); // Os ADPSn servem para configurar o clock do ADC 
  set_bit(ADCSRA, ADPS1);
  set_bit(ADCSRA, ADPS0);
  set_bit(DIDR0, ADC0D);
  ADMUX = (1 << REFS0);  // Deixa a referência como 5V
  
  iniciarLabirinto();
}

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

char lerTeclado(void) {
  static char ultimaTecla = '\0';
  char teclaPressionada = '\0';
  
  for (int l = 0; l < 4; l++) {
  	clr_bit(PORTB, pinosLinhas[l]);
    _delay_us(1);
    for (int c = 0; c < 4; c++) {
      if (c < 1) {
        if (!(tst_bit(PINB, pinosColunas[c]))) teclaPressionada = teclas[l][c]; 
      }
      
      else if (c < 2) {
      	if (!(tst_bit(PIND, pinosColunas[c]))) teclaPressionada = teclas[l][c]; 
      }
      
      else {
      	if (!(tst_bit(PINC, pinosColunas[c]))) teclaPressionada = teclas[l][c];
      }
    }
    set_bit(PORTB, pinosLinhas[l]);
  }
  
  if (ultimaTecla != teclaPressionada) {
  	ultimaTecla = teclaPressionada;
    return teclaPressionada;
  }
  
  else {
  	return '\0';
  }
}

void loop() {
  
  char tecla = lerTeclado();
  if (tecla != '\0') {
    
    if (!estadoJogo){
      atualizarLabirinto(tecla);
    };
  }
  _delay_ms(20);
}