#include "adc.h"
#include <avr/io.h>

uint16_t meu_analogRead(uint8_t canal) {
  ADMUX = (ADMUX & 0xF0) | canal;
  set_bit(ADCSRA, ADSC);
  while (tst_bit(ADCSRA, ADSC));
  return ADC;
}