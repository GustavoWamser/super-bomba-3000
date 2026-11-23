#include "adc.h"
#include <avr/io.h>
#include "bits.h"

uint16_t meu_analogRead(uint8_t canal) {
  ADMUX = (ADMUX & 0xF0) | canal;
  set_bit(ADCSRA, ADSC);
  while (tst_bit(ADCSRA, ADSC));
  return ADC;
}

void configurarADC()  {
  set_bit(ADCSRA, ADEN);  // Habilita o ADC
  set_bit(ADCSRA, ADPS2); // Os ADPSn servem para configurar o clock do ADC 
  set_bit(ADCSRA, ADPS1);
  set_bit(ADCSRA, ADPS0);
  set_bit(DIDR0, ADC0D);
  ADMUX = (1 << REFS0);
}