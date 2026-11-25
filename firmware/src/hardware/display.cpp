#include "display.h"

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

static const uint8_t ENDERECO_LCD = 0x27;
static const uint8_t COLUNAS = 20;
static const uint8_t LINHAS = 4;

// Layout (colunas 0-19, linhas 0-3)
static const uint8_t LINHA_VIDAS = 0;
static const uint8_t COLUNA_PRIMEIRA_VIDA = 17;
static const uint8_t TOTAL_VIDAS = 3;

static const uint8_t LINHA_TEMPO = 2;
static const uint8_t COLUNA_INICIO_TEMPO = 7;    // Vai ser escrito no formato "0M:SS" a partir da coluna 6 (começa de 0)
static const uint8_t COLUNA_ZERO_FIXO = COLUNA_INICIO_TEMPO;
static const uint8_t COLUNA_MINUTO = COLUNA_INICIO_TEMPO + 1;
static const uint8_t COLUNA_DOIS_PONTOS = COLUNA_INICIO_TEMPO + 2;
static const uint8_t COLUNA_DEZENA_SEGUNDOS = COLUNA_INICIO_TEMPO + 3;
static const uint8_t COLUNA_UNIDADE_SEGUNDOS = COLUNA_INICIO_TEMPO + 4;

static LiquidCrystal_I2C lcd(ENDERECO_LCD, COLUNAS, LINHAS);

void configurarDisplay(void) {
    lcd.init();
    lcd.clear();
    lcd.noDisplay();
    lcd.noBacklight();
}

void ligarDisplayJogo(void) {
    lcd.clear();
    lcd.display();
    lcd.backlight();

    lcd.setCursor(COLUNA_ZERO_FIXO, LINHA_TEMPO);
    lcd.print('0');

    lcd.setCursor(COLUNA_DOIS_PONTOS, LINHA_TEMPO);
    lcd.print(':');
}

void desligarDisplay(void) {
    lcd.clear();
    lcd.noDisplay();
    lcd.noBacklight();
}

static void escreverTempo(uint16_t segundosTotais) {
    uint8_t minutos = segundosTotais / 60;
    uint8_t segundosRestantes = segundosTotais % 60;

    lcd.setCursor(COLUNA_MINUTO, LINHA_TEMPO);
    lcd.print(minutos);
    lcd.setCursor(COLUNA_DEZENA_SEGUNDOS, LINHA_TEMPO);
    lcd.print(segundosRestantes / 10);
    lcd.setCursor(COLUNA_UNIDADE_SEGUNDOS, LINHA_TEMPO);
    lcd.print(segundosRestantes % 10);
}

void atualizarVidasDisplay(uint8_t vidas) {
    for (uint8_t vida = 0; vida < TOTAL_VIDAS; vida++) {
        lcd.setCursor(COLUNA_PRIMEIRA_VIDA + vida, LINHA_VIDAS);
        if (vida < vidas) {
            lcd.print('.');
        }

        else {
            lcd.print(' ');
        }
    }
} 

void atualizarTempoDisplay(uint16_t segundosTotais) {
    escreverTempo(segundosTotais);
}