#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../hardware/adc.h"

#define TOTAL_OPERACOES 10
#define OPERACOES_POR_FASE 5

#define set_bit(reg, bit) ((reg) |= (1 << (bit)))
#define clr_bit(reg, bit) ((reg) &= ~(1 << (bit)))
#define tst_bit(reg, bit) ((reg) & (1 << (bit)))

const uint8_t pinosLinhas[4] = {PB4, PB2, PB1, PB0};
const uint8_t pinosColunas[4] = {PB5, PD7, PC3, PC2};

char teclas[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

struct Operacao {
  const char *conta;
  uint16_t resultado;
};

const Operacao operacoes[TOTAL_OPERACOES] = {
  {"12 + 3 x 4 - 8 / 2", 20},
  {"25 + 6 x 5 - 12 / 3", 51},
  {"7 + 4 x 8 - 18 / 6", 36},
  {"40 + 9 x 3 - 20 / 4", 62},
  {"15 + 2 x 7 - 21 / 3", 22},
  {"9 + 5 x 6 - 16 / 4", 35},
  {"30 + 8 x 2 - 15 / 5", 43},
  {"18 + 3 x 9 - 14 / 2", 38},
  {"50 + 7 x 4 - 24 / 6", 74},
  {"6 + 9 x 5 - 27 / 3", 42}
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
  
  lerTeclado();
  srand(meu_analogRead(0));
}

void loop() {
  mathFase();
  _delay_ms(1000);
}

char lerTeclado(void) {
  static char ultimaTecla = '\0';
  char teclaPressionada = '\0';

  for (int linha = 0; linha < 4; linha++) {
    clr_bit(PORTB, pinosLinhas[linha]);
    _delay_us(1);

    for (int coluna = 0; coluna < 4; coluna++) {
      if (coluna < 1) {
        if (!(tst_bit(PINB, pinosColunas[coluna]))) {
          teclaPressionada = teclas[linha][coluna];
        }
      } else if (coluna < 2) {
        if (!(tst_bit(PIND, pinosColunas[coluna]))) {
          teclaPressionada = teclas[linha][coluna];
        }
      } else {
        if (!(tst_bit(PINC, pinosColunas[coluna]))) {
          teclaPressionada = teclas[linha][coluna];
        }
      }
    }

    set_bit(PORTB, pinosLinhas[linha]);
  }

  if (ultimaTecla != teclaPressionada) {
    ultimaTecla = teclaPressionada;
    return teclaPressionada;
  }

  return '\0';
}

uint8_t sorteiaOperacao(uint8_t sorteadas[]) {
  uint8_t indice;

  do {
    indice = rand() % TOTAL_OPERACOES;
  } while (sorteadas[indice]);

  sorteadas[indice] = 1;
  return indice;
}

uint32_t lerResposta() {
  uint32_t resposta = 0;
  uint8_t quantidade = 0;

  Serial.println("Digite a resposta e pressione #:");

  while (1) {
    char tecla = lerTeclado();

    if (tecla >= '0' && tecla <= '9') {
      if (quantidade < 5) {
        resposta = resposta * 10 + (tecla - '0');
        quantidade++;
        Serial.print(tecla);
      }
    } else if (tecla == '*' && quantidade > 0) {
      resposta /= 10;
      quantidade--;
      Serial.print("\b \b");
    } else if (tecla == '#' && quantidade > 0) {
      Serial.println();
      return resposta;
    }
  }
}

uint8_t mostrarOperacao(uint8_t numero, uint8_t indice) {
  const Operacao &operacao = operacoes[indice];
  uint32_t resposta;

  Serial.println();
  Serial.print("Operacao ");
  Serial.print(numero);
  Serial.print("/");
  Serial.println(OPERACOES_POR_FASE);
  Serial.println("Calcule:");
  Serial.println(operacao.conta);

  while (1) {
    resposta = lerResposta();

    if (resposta == operacao.resultado) {
      Serial.println("Acertou!");
      return 1;
    }

    Serial.println("Errou! Tente novamente.");
  }
}

void mathFase() {
  uint8_t sorteadas[TOTAL_OPERACOES] = {0};
  uint8_t acertos = 0;

  for (uint8_t numero = 1; numero <= OPERACOES_POR_FASE; numero++) {
    uint8_t indice = sorteiaOperacao(sorteadas);
    acertos += mostrarOperacao(numero, indice);
  }

  Serial.println();
  Serial.print("Pontuacao: ");
  Serial.print(acertos);
  Serial.print("/");
  Serial.println(OPERACOES_POR_FASE);

  Serial.println("Passou de fase!");
}
