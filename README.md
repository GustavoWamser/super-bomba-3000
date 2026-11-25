# Super Bomba 3000

## 1. Contexto do projeto

O Super Bomba 3000 é um jogo de Escape Room. O objetivo do jogo é concluir fases e desafios antes que o tempo acabe e evitar que a bomba exploda.

As fases são sorteadas e aparecem em ordem aleatória. Para vencer, é preciso completar todas as fases da dificuldade escolhida, garantindo que não perca suas 3 vidas.

## 2. Tecnologias utilizadas

- Placa: Arduino Uno;
- Linguagem: C/C++ para Arduino;
- Teclado matricial 4x4;
- LEDs e Botões;
- Monitor Serial, com comunicação em 9600 baud;
- Fio de segurança entre D2 (PD2/INT0) e GND;
- Arduino IDE ou Arduino CLI;

## 3. Como jogar

Ao iniciar, escolha uma dificuldade no teclado:

- `1` — Fácil: 3 fases e 180 segundos
- `2` — Normal: 4 fases e 240 segundos
- `3` — Difícil: 5 fases e 300 segundos

A ordem das fases é sorteada a cada partida. Antes de cada fase, pressione uma tecla ou um botão para começar.

### Labirinto

Use o teclado numérico para levar o jogador (`@`) até a saída (`F`), sem atravessar as paredes:

- `2` — cima
- `8` — baixo
- `4` — esquerda
- `6` — direita

### Matemática

Resolva 5 operações matemáticas. Digite a resposta no teclado e pressione `#` para confirmar.

- `*` apaga o último número digitado
- `#` confirma a resposta

### Memória

Observe a sequência de LEDs e repita-a pressionando os botões correspondentes. A sequência aumenta a cada rodada, até completar 7 rodadas.

### Capitais

Responda 5 perguntas sobre capitais de países. Digite a alternativa correta usando `A`, `B`, `C` ou `D`.

### Reflexo

Quando um LED acender, pressione rapidamente o botão correspondente. É necessário acertar 7 vezes para concluir a fase.

### Vidas e tempo

Cada erro faz o jogador perder uma vida e 10 segundos. Se as 3 vidas acabarem ou o tempo chegar a zero, a bomba explode e a partida termina.

Ao concluir todas as fases da dificuldade escolhida, o jogador vence. Depois do fim da partida, pressione uma tecla para voltar ao menu.

## 4. Como compilar e executar

### Fio da bomba

Conecte o fio entre o pino D2 e o GND. O firmware usa o pull-up interno:

- fio conectado: D2 lê nível baixo;
- fio desconectado: D2 lê nível alto e dispara a interrupção `INT0`.

O estado é conferido antes de iniciar cada partida. Durante o jogo, a ISR apenas registra o evento; o `loop()` principal encerra a partida e mostra a mensagem de explosão.

### Requisitos

- Arduino IDE ou Arduino CLI instalado
- Arduino Uno conectado ao computador por USB
- Circuito montado com o teclado, os LEDs e os botões
- Cabo USB para programação e alimentação

### Arduino IDE

1. Abra o arquivo `firmware/firmware.ino` no Arduino IDE.
2. Selecione **Arduino Uno** em **Ferramentas > Placa**.
3. Selecione a porta USB da placa em **Ferramentas > Porta**.
4. Clique em **Verificar** para compilar.
5. Clique em **Enviar** para gravar o programa na placa.
6. Abra o Monitor Serial em `9600 baud` para acompanhar o jogo.

### Arduino CLI

Na raiz do projeto, compile com:

```bash
arduino-cli compile --fqbn arduino:avr:uno firmware
```

Envie para a placa substituindo `/dev/ttyACM0` pela porta correta:

```bash
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno firmware
```

Para acompanhar as mensagens do jogo pelo terminal:

```bash
arduino-cli monitor -p /dev/ttyACM0 -c baudrate=9600
```
