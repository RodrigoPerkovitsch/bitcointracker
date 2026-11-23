# Montagem na protoboard

Guia para montar o Bitcoin Tracker na protoboard de 830 furos. O mesmo circuito
está desenhado no Wokwi (`wokwi/diagram.json`): abra lá para ver onde fica cada
peça, e use as tabelas abaixo para saber exatamente em que furo vai cada perna e
cada fio.

> Monte tudo com o ESP32 **desligado do USB**. Só ligue no final.

## Material

| Peça | Quantidade |
|---|---|
| ESP32 DevKit V1 (30 pinos) | 1 |
| Display OLED 0,96" I2C (4 pinos) | 1 |
| LED 5 mm | 1 |
| Resistor 220 Ω (vermelho, vermelho, marrom) | 1 |
| Chave táctil 6x6 mm | 1 |
| Jumper macho-fêmea | 6 |
| Jumper macho-macho | 4 |

## Como ler a protoboard

- As **colunas** têm números (1, 2, 3…) e as **fileiras** têm letras (a até j).
  Um furo é dito assim: "coluna 13, fileira h".
- Os 5 furos de uma mesma coluna entre **a e e** são ligados entre si. Os 5 entre
  **f e j** também. O vão no meio separa as duas metades.
- Os **trilhos** são as fileiras compridas das bordas. Só usamos os **dois trilhos
  de baixo**: o da linha **vermelha (+)** leva 3,3 V e o da linha **azul (−)** é o
  GND. Os trilhos de cima ficam sem uso, embaixo da tela do OLED.

## Onde fica cada peça

- **ESP32:** fora da protoboard, à esquerda, com os pinos D21, D22, RX2, D4 e 3V3
  virados para a protoboard. Ele é largo demais para a EasyProto: espetado nela,
  cobriria quase todos os furos livres. Por isso a ligação é por jumpers
  macho-fêmea: a ponta fêmea no pino do ESP32, a ponta macho no furo.
- **OLED:** espetado na fileira de cima, com a tela para fora da borda da protoboard.
- **LED, resistor e botão:** na metade de baixo, perto da esquerda.

## Passo 1: espetar as peças

| Peça | Perna | Furo |
|---|---|---|
| **OLED** (tela para fora da borda de cima) | SDA | coluna **41**, fileira **a** |
| | SCL | coluna **42**, fileira **a** |
| | VCC | coluna **43**, fileira **a** |
| | GND | coluna **44**, fileira **a** |
| **Resistor 220 Ω** (deitado; não tem lado certo) | uma perna | coluna **7**, fileira **j** |
| | outra perna | coluna **13**, fileira **j** |
| **LED** | perna **longa** (+) | coluna **13**, fileira **h** |
| | perna **curta** (−, lado reto do LED) | coluna **14**, fileira **h** |
| **Botão** | 4 pernas | colunas **18** e **20**, fileiras **f** e **i** |

### Cuidado com o OLED: confira a ordem dos pinos

Leia o que está impresso no **seu** módulo, ao lado dos pinos:

- **`GND VCC SCL SDA`** (o mais comum): monte como na tabela acima.
- **`VCC GND SCL SDA`**: o VCC e o GND vêm trocados. Monte igual, mas **troque os
  fios 7 e 8** do passo 3 (o 7 vai para o trilho − e o 8 para o trilho +).
  **Ligar VCC no GND queima o display.**

Com a tela virada para fora, ela fica de cabeça para baixo. O firmware já
compensa isso (`GIRAR_TELA_180` no `BitcoinTracker.ino`). Se montarem o OLED
de outro jeito e o texto aparecer invertido, mude esse valor para `false`.

### Cuidado com o botão: descubra quais pernas são ligadas

A chave táctil liga as pernas em pares, por dentro. Se o par ficar no lugar
errado, o botão parece apertado o tempo todo. Antes de espetar:

1. Coloque o multímetro no modo **continuidade** (o que apita).
2. Teste pares de pernas **sem apertar** o botão. Os dois pares que apitam
   sozinhos são os pares ligados por dentro.
3. Espete um desses pares na **coluna 18** (fileiras f e i) e o outro par na
   **coluna 20** (fileiras f e i).

## Passo 2: fios do ESP32 (jumpers macho-fêmea)

| Fio | Pino do ESP32 (DevKit V1) | Vai para | Cor sugerida | Para quê |
|---|---|---|---|---|
| 1 | **D21** | coluna **41**, fileira **e** | azul | SDA do OLED |
| 2 | **D22** | coluna **42**, fileira **b** | amarelo | SCL do OLED |
| 3 | **RX2** (é o GPIO16) | coluna **7**, fileira **h** | laranja | LED |
| 4 | **D4** | coluna **18**, fileira **j** | verde | botão |
| 5 | **3V3** | trilho **+** de baixo, ponta esquerda | vermelho | alimentação |
| 6 | **GND** | trilho **−** de baixo, ponta esquerda | preto | terra |

- No Wokwi o fio 6 sai do GND do **lado esquerdo** do ESP32 e passa por baixo
  da placa, só para o desenho não cruzar o fio 5. Na montagem real **qualquer
  GND do ESP32 serve**.
- Se o ESP32 de vocês for o de **38 pinos** (DevKitC), os nomes impressos são
  outros, mas os números dos GPIO são os mesmos: **21**, **22**, **16**, **4**,
  **3V3** e **GND**.
- Use **3V3**, nunca **VIN/5V**: o OLED e as entradas do ESP32 trabalham em 3,3 V.

## Passo 3: jumpers na própria protoboard (macho-macho)

| Fio | De | Para | Cor sugerida | Para quê |
|---|---|---|---|---|
| 7 | coluna **43**, fileira **e** | trilho **+** de baixo, logo abaixo | vermelho | VCC do OLED |
| 8 | coluna **44**, fileira **e** | trilho **−** de baixo, logo abaixo | preto | GND do OLED |
| 9 | coluna **14**, fileira **j** | trilho **−** de baixo, logo abaixo | preto | − do LED |
| 10 | coluna **20**, fileira **j** | trilho **−** de baixo, logo abaixo | preto | botão |

No desenho do Wokwi os fios pretos fazem uma curvinha antes de entrar no
trilho −, para não passarem por cima dos furos do trilho +. Na montagem real é
só espetar no furo do trilho − mais próximo.

### Trilho dividido no meio?

Em algumas protoboards de 830 furos os trilhos são **cortados no meio** (dá para
ver uma interrupção na linha vermelha/azul). Os fios 5 e 6 entram na ponta
esquerda e os fios 7 e 8 perto da coluna 43, depois do meio. Confira com o
multímetro (continuidade) entre um furo do trilho perto da coluna 3 e outro
perto da coluna 43. Se não apitar, ligue as duas metades de cada trilho com um
jumper (+ com +, − com −).

## Passo 4: testar a fiação

Antes de gravar o firmware completo, grave o sketch de teste
`ferramentas/teste-montagem/teste-montagem.ino`. Ele não precisa de Wi-Fi nem
de bibliotecas. Abra o monitor serial em **115200** e confira:

| Mensagem esperada | Se não aparecer, confira |
|---|---|
| `OK    OLED encontrado no endereco 0x3C` | fios 1, 2, 7 e 8 e a ordem dos pinos do OLED |
| o LED pisca 3 vezes logo no início | fio 3, o resistor, o lado do LED e o fio 9 |
| `OK    botao apertado -> LED aceso` ao apertar | fios 4 e 10 e o par de pernas do botão |

Se as três passarem, grave o `BitcoinTracker.ino` (veja o README).

## Problemas comuns

| Sintoma | Causa provável |
|---|---|
| Nada liga, nem o LED vermelho do ESP32 | cabo USB só de carga (sem dados) ou mal encaixado |
| OLED não responde | SDA e SCL trocados (fios 1 e 2), ou VCC/GND do OLED invertidos |
| LED nunca acende | LED ao contrário (perna longa na coluna 13) ou resistor fora das colunas 7 e 13 |
| Botão "apertado" o tempo todo | par de pernas do botão na posição errada (veja o teste com multímetro) |
| Texto do OLED de cabeça para baixo | troque `GIRAR_TELA_180` no `BitcoinTracker.ino` |
