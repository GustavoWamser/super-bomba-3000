#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "../hardware/adc.h"
#include "../hardware/bits.h"

#define TAMANHO_FINAL 7
#define TEMPO_RESPOSTA 3000 // ms

uint8_t led[4] = {PD6, PD5, PD4, PD3}; // azul, amarelo, verde, laranja
uint8_t sequencia[TAMANHO_FINAL];

volatile uint32_t milissegundos = 0;

ISR(TIMER1_COMPA_vect) {
  milissegundos++;
}

uint32_t tempo_atual() {
  cli();
  uint32_t agora = milissegundos;
  sei();
  return agora;
}

void espera(uint32_t ms) {
  uint32_t inicio = tempo_atual();
  while (tempo_atual() - inicio < ms);
}

void envia_letra(char c) {
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
}

void envia_texto(const char *texto) {
  for (int i = 0; texto[i] != '\0'; i++) {
    envia_letra(texto[i]);
  }
}

int8_t le_faixa_adc(void) {
  uint16_t leitura_botao = meu_analogRead(1);
  if (leitura_botao >= 921) return -1; // Solto (~5,0 V -> 1023)
  if (leitura_botao >= 716) return 0;  // 1º botão - Azul    (~4,0 V -> 819)
  if (leitura_botao >= 512) return 1;  // 2º botão - Amarelo (~3,0 V -> 614)
  if (leitura_botao >= 307) return 2;  // 3º botão - Verde   (~2,0 V -> 410)
  return 3;                            // 4º botão - Laranja (~1,0 V -> 205)
}

int8_t le_botao() { // retorna o botao apertado (0 a 3) ou -1 se nenhum
  int8_t b1 = le_faixa_adc();

  if (b1 == -1) return -1;

  else {
    espera(15);
    int8_t b2 = le_faixa_adc();
    if (b1 == b2) return b1;

    return -1;
  }
}

void espera_soltar() {
  espera(20);
  while (le_botao() != -1);
  espera(20);
}

void pisca_todos(uint8_t vezes, uint16_t intervalo) {
  for (uint8_t i = 0; i < vezes; i++) {
    PORTD |= 0b01111000;
    espera(intervalo);
    PORTD &= ~0b01111000;
    espera(intervalo);
  }
}

void mostra_sequencia(uint8_t rodada) {
  for (uint8_t i = 0; i < rodada; i++) {
    PORTD |= (1 << led[sequencia[i]]);
    espera(500);
    PORTD &= ~(1 << led[sequencia[i]]);
    espera(250);
  }
}

uint8_t fase_genius() {
  for (uint8_t i = 0; i < TAMANHO_FINAL; i++) {
    sequencia[i] = rand() % 4;
  }

  for (uint8_t rodada = 1; rodada <= TAMANHO_FINAL; rodada++) {
    envia_texto("Rodada ");
    envia_letra('0' + rodada);
    envia_texto("\r\n");
    mostra_sequencia(rodada);

    for (uint8_t passo = 0; passo < rodada; passo++) {
      uint32_t inicio = tempo_atual();
      int8_t apertado = -1;
      while (apertado == -1 && tempo_atual() - inicio < TEMPO_RESPOSTA) {
        apertado = le_botao();
      }

      if (apertado == -1) {
        envia_texto("Demorou demais\r\n");
        return 0;
      }

      PORTD |= (1 << led[apertado]); // acende enquanto segura
      espera_soltar();
      PORTD &= ~(1 << led[apertado]);

      if (apertado != sequencia[passo]) {
        envia_texto("Errou\r\n");
        return 0;
      }
    }
    espera(800);
  }
  return 1;
}

int main() {
  DDRD |= 0b01111000; // leds nos pinos 3 a 6
  PORTD &= ~0b01111000;
  
 
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  DIDR0 |= (1 << ADC1D);
  PORTC |= (1 << PC1);
  ADMUX = (1 << REFS0);

  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC, prescaler 64
  OCR1A = 249; // 1 ms
  TIMSK1 = (1 << OCIE1A);

  UBRR0 = 103; // 9600 baud
  UCSR0B = (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  sei();

  while (1) {
    envia_texto("Aperte um botao pra comecar\r\n");
    while (le_botao() == -1);
    srand(tempo_atual());
    espera_soltar();
    espera(500);

    if (fase_genius()) {
      envia_texto("Passou!\r\n");
      pisca_todos(3, 300);
    } else {
      envia_texto("Falhou!\r\n");
      pisca_todos(10, 80);
    }
    espera(1000);
  }
}