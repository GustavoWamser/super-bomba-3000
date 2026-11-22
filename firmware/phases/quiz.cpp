#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define TOTAL_PERGUNTAS 10
#define PERGUNTAS_POR_RODADA 5

#define set_bit(reg, bit) ((reg) |= (1 << (bit)))
#define clr_bit(reg, bit) ((reg) &= ~(1 << (bit)))
#define tst_bit(reg, bit) ((reg) & (1 << (bit)))

const uint8_t pinosLinhas[4] = {PB4, PB2, PB1, PB0};
const uint8_t pinosColunas[4] = {PB5, PD7, PC3, PC2};

const char teclas[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

struct PerguntaQuiz {
  const char *pais;
  const char *alternativaA;
  const char *alternativaB;
  const char *alternativaC;
  const char *alternativaD;
  char alternativaCerta;
};

const struct PerguntaQuiz perguntas[TOTAL_PERGUNTAS] = {
  {"Brasil", "A) Buenos Aires", "B) Brasilia", "C) Lima", "D) Bogota", 'B'},
  {"Argentina", "A) Santiago", "B) Montevideu", "C) Buenos Aires", "D) Assuncao", 'C'},
  {"Franca", "A) Paris", "B) Roma", "C) Madrid", "D) Lisboa", 'A'},
  {"Japao", "A) Seul", "B) Pequim", "C) Tóquio", "D) Bangkok", 'C'},
  {"Australia", "A) Sydney", "B) Canberra", "C) Melbourne", "D) Wellington", 'B'},
  {"Canada", "A) Toronto", "B) Vancouver", "C) Ottawa", "D) Montreal", 'C'},
  {"Egito", "A) Cairo", "B) Alexandria", "C) Giza", "D) Luxor", 'A'},
  {"Italia", "A) Milao", "B) Veneza", "C) Napoles", "D) Roma", 'D'},
  {"Portugal", "A) Porto", "B) Lisboa", "C) Coimbra", "D) Braga", 'B'},
  {"Chile", "A) Valparaiso", "B) Santiago", "C) Concepcion", "D) Antofagasta", 'B'}
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
  quizFase();
  _delay_ms(1000);
}

uint16_t meu_analogRead(uint8_t canal) {
  ADMUX = (ADMUX & 0xF0) | canal;
  set_bit(ADCSRA, ADSC);
  while (tst_bit(ADCSRA, ADSC));
  return ADC;
}

char lerTeclado() {
  static char ultimaTecla = '\0';
  char teclaPressionada = '\0';

  for (uint8_t linha = 0; linha < 4; linha++) {
    clr_bit(PORTB, pinosLinhas[linha]);
    _delay_us(1);

    for (uint8_t coluna = 0; coluna < 4; coluna++) {
      bool pressionada;
      if (coluna == 0) pressionada = !tst_bit(PINB, pinosColunas[coluna]);
      else if (coluna == 1) pressionada = !tst_bit(PIND, pinosColunas[coluna]);
      else pressionada = !tst_bit(PINC, pinosColunas[coluna]);

      if (pressionada) teclaPressionada = teclas[linha][coluna];
    }

    set_bit(PORTB, pinosLinhas[linha]);
  }

  if (ultimaTecla != teclaPressionada) {
    ultimaTecla = teclaPressionada;
    return teclaPressionada;
  }

  return '\0';
}

uint8_t sorteiaPergunta(uint8_t sorteadas[]) {
  uint8_t indice;
  do {
    indice = rand() % TOTAL_PERGUNTAS;
  } while (sorteadas[indice]);

  sorteadas[indice] = 1;
  return indice;
}

void mostraPergunta(uint8_t numero, const struct PerguntaQuiz &pergunta) {
  Serial.println();
  Serial.print("Pergunta ");
  Serial.print(numero);
  Serial.print("/");
  Serial.println(PERGUNTAS_POR_RODADA);
  Serial.print("Qual e a capital de ");
  Serial.print(pergunta.pais);
  Serial.println("?");
  Serial.println(pergunta.alternativaA);
  Serial.println(pergunta.alternativaB);
  Serial.println(pergunta.alternativaC);
  Serial.println(pergunta.alternativaD);
  Serial.println("Digite A, B, C ou D:");

  while (1) {
    char resposta = lerTeclado();

    if (resposta >= 'A' && resposta <= 'D') {
      if (resposta == pergunta.alternativaCerta) {
        Serial.println("Acertou!");
        return;
      }

      Serial.println("Resposta errada! Tente novamente.");
    }
  }
}

void quizFase() {
  uint8_t sorteadas[TOTAL_PERGUNTAS] = {0};

  for (uint8_t numero = 1; numero <= PERGUNTAS_POR_RODADA; numero++) {
    uint8_t indice = sorteiaPergunta(sorteadas);
    mostraPergunta(numero, perguntas[indice]);
  }

  Serial.println("Fase de capitais concluida!");
}