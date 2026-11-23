#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "../hardware/bits.h"

#define TEMPO_MAXIMO 2000 // tempo pra apertar cada botao (ms)
#define ACERTOS_PRA_PASSAR 7

uint8_t led[4] = {PD6, PD5, PD4, PD3}; // azul, amarelo, verde, laranja

volatile uint32_t milissegundos = 0;

ISR(TIMER1_COMPA_vect) { // chamada pelo timer1 a cada 1 ms
  milissegundos++;
}

uint32_t tempo_atual() {
  cli(); // desliga as interrupcoes pra ler sem ser interrompido
  uint32_t agora = milissegundos;
  sei();
  return agora;
}

uint16_t meu_analogRead(uint8_t canal) {
  ADMUX = (ADMUX & 0xF0) | canal; // escolhe A0..A5
  set_bit(ADCSRA, ADSC);          // começa a conversão
  while (tst_bit(ADCSRA, ADSC));  // espera terminar a leitura
  return ADC;
}

int8_t le_faixa_adc(void) {
  uint16_t leitura_botao = meu_analogRead(1);
  if (leitura_botao >= 921) return -1; // Solto (~5,0 V -> 1023)
  if (leitura_botao >= 716) return 0;  // 1º botão - Azul    (~4,0 V -> 819)
  if (leitura_botao >= 512) return 1;  // 2º botão - Amarelo (~3,0 V -> 614)
  if (leitura_botao >= 307) return 2;  // 3º botão - Verde   (~2,0 V -> 410)
  return 3;                            // 4º botão - Laranja (~1,0 V -> 205)
}

void espera(uint32_t ms) {
  uint32_t inicio = tempo_atual();
  while (tempo_atual() - inicio < ms);
}

void envia_letra(char letra) {
  while (!(UCSR0A & (1 << UDRE0))); // espera a uart ficar livre
  UDR0 = letra;
}

void envia_texto(const char *texto) {
  for (int i = 0; texto[i] != '\0'; i++) {
    envia_letra(texto[i]);
  }
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
  espera(20); // bounce
  while (le_botao() != -1);
  espera(20);
}

void pisca_todos(uint8_t vezes, uint16_t intervalo) {
  for (uint8_t i = 0; i < vezes; i++) {
    PORTD |= 0b01111000; // acende os 4 leds
    espera(intervalo);
    PORTD &= ~0b01111000; // apaga os 4 leds
    espera(intervalo);
  }
}

uint8_t fase_reflexo() { // retorna 1 se passou e 0 se falhou
  uint8_t acertos = 0;
  int8_t led_anterior = -1;

  while (acertos < ACERTOS_PRA_PASSAR) {
    uint8_t sorteado = rand() % 4;
    if (sorteado == led_anterior) {
      continue; // mesmo led da rodada anterior, sorteia de novo
    }
    led_anterior = sorteado;

    PORTD |= (1 << led[sorteado]);
    uint32_t inicio = tempo_atual();

    int8_t apertado = -1;
    while (apertado == -1 && tempo_atual() - inicio < TEMPO_MAXIMO) {
      apertado = le_botao();
    }
    PORTD &= ~(1 << led[sorteado]);

    if (apertado == -1) {
      envia_texto("Demorou demais!\r\n");
      return 0;
    }
    if (apertado != sorteado) {
      envia_texto("Botao errado!\r\n");
      return 0;
    }

    acertos++;
    envia_texto("Acertou! ");
    envia_letra('0' + acertos);
    envia_letra('/');
    envia_letra('0' + ACERTOS_PRA_PASSAR);
    envia_texto("\r\n");
    espera_soltar();
  }
  return 1;
}


int main() {
  DDRD |= 0b01111000; // leds (pinos 3 a 6) como saida
  PORTD &= ~0b01111000; // comecam apagados
  ADMUX = (1 << REFS0); // referência como 5V
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  DIDR0 |= (1 << ADC1D);
  PORTC |= (1 << PC1);

  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // modo CTC, prescaler 64
  OCR1A = 249; // 16 MHz / 64 / 250 = 1 ms
  TIMSK1 = (1 << OCIE1A); // liga a interrupcao do timer1

  UBRR0 = 103; // 9600 baud
  UCSR0B = (1 << TXEN0); // so envio
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 bits

  sei();

  while (1) {
    envia_texto("Aperte um botao pra comecar\r\n");
    while (le_botao() == -1);
    srand(tempo_atual());
    espera_soltar();
    espera(500);

    if (fase_reflexo() == 1) {
      envia_texto("Fase concluida!\r\n");
      pisca_todos(3, 300);
    } else {
      envia_texto("Falhou!\r\n");
      pisca_todos(10, 80);
    }
    espera(1000);
  }
}