# Montagem na protoboard

Instruções para montar o Bitcoin Tracker em uma protoboard de 830 furos. O mesmo
circuito está desenhado no Wokwi (`wokwi/diagram.json`), que mostra a posição de
cada peça. As tabelas abaixo indicam o furo exato de cada perna e de cada fio.

> Faça toda a montagem com o ESP32 desconectado do USB e ligue-o apenas no final.

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

## Leitura da protoboard

- As colunas são numeradas (1, 2, 3…) e as fileiras são identificadas por letras
  (a até j). Neste guia, um furo é indicado como "coluna 13, fileira h".
- Os cinco furos de uma coluna entre as fileiras a e e estão ligados entre si; o
  mesmo vale para os cinco entre f e j. O canal central separa as duas metades.
- Os trilhos são as fileiras longas das bordas. A montagem usa apenas os dois
  trilhos inferiores: o da linha vermelha (+) recebe 3,3 V e o da linha azul (−)
  é o GND. Os trilhos superiores ficam livres, sob a tela do OLED.

## Posição das peças

- ESP32: fora da protoboard, à esquerda, com os pinos D21, D22, RX2, D4 e 3V3
  voltados para ela. A placa é larga demais para a EasyProto; se fosse encaixada
  nela, cobriria quase todos os furos livres. Por isso a ligação é feita com
  jumpers macho-fêmea, com a ponta fêmea no pino do ESP32 e a ponta macho no furo.
- OLED: na fileira superior, com a tela para fora da borda da protoboard.
- LED, resistor e botão: na metade inferior, do lado esquerdo.

## Passo 1: encaixar as peças

| Peça | Perna | Furo |
|---|---|---|
| OLED (tela para fora da borda superior) | SDA | coluna 41, fileira a |
| | SCL | coluna 42, fileira a |
| | VCC | coluna 43, fileira a |
| | GND | coluna 44, fileira a |
| Resistor 220 Ω (deitado, sem polaridade) | uma perna | coluna 7, fileira j |
| | outra perna | coluna 13, fileira j |
| LED | perna longa (+) | coluna 13, fileira h |
| | perna curta (−, lado chanfrado) | coluna 14, fileira h |
| Botão | 4 pernas | colunas 18 e 20, fileiras f e i |

### Ordem dos pinos do OLED

Antes de encaixar, confira a ordem impressa ao lado dos pinos do módulo:

- `GND VCC SCL SDA` (a mais comum): siga a tabela acima.
- `VCC GND SCL SDA`: VCC e GND estão invertidos. Mantenha a posição do módulo e
  troque os fios 7 e 8 do passo 3, ligando o 7 ao trilho − e o 8 ao trilho +.
  **Ligar o VCC ao GND danifica o display.**

Se o texto aparecer de cabeça para baixo, ajuste `GIRAR_TELA_180` no
`BitcoinTracker.ino`. Na montagem real o OLED ficou na posição normal e o valor
está `false`. Com o OLED voltado para fora da borda, como no desenho do Wokwi,
use `true`.

### Pares de pernas do botão

Internamente, a chave táctil liga suas pernas em pares. Se os pares forem
encaixados na posição errada, o botão fica permanentemente acionado. Para
identificá-los:

1. Coloque o multímetro no modo continuidade.
2. Sem pressionar o botão, teste as pernas duas a duas. Os pares que acusam
   continuidade são os ligados internamente.
3. Encaixe um desses pares na coluna 18 (fileiras f e i) e o outro na coluna 20
   (fileiras f e i).

## Passo 2: fios do ESP32 (jumpers macho-fêmea)

| Fio | Pino do ESP32 (DevKit V1) | Destino | Cor sugerida | Função |
|---|---|---|---|---|
| 1 | D21 | coluna 41, fileira e | azul | SDA do OLED |
| 2 | D22 | coluna 42, fileira b | amarelo | SCL do OLED |
| 3 | RX2 (GPIO16) | coluna 7, fileira h | laranja | LED |
| 4 | D4 | coluna 18, fileira j | verde | botão |
| 5 | 3V3 | trilho + inferior, extremidade esquerda | vermelho | alimentação |
| 6 | GND | trilho − inferior, extremidade esquerda | preto | terra |

- No Wokwi, o fio 6 sai do GND do lado esquerdo do ESP32 e passa por baixo da
  placa apenas para não cruzar o fio 5 no desenho. Na montagem real, qualquer
  pino GND do ESP32 pode ser usado.
- No ESP32 de 38 pinos (DevKitC), os nomes impressos mudam, mas os números dos
  GPIO são os mesmos: 21, 22, 16, 4, 3V3 e GND.
- Use o pino 3V3, nunca o VIN ou o 5V: o OLED e as entradas do ESP32 operam em
  3,3 V.

## Passo 3: jumpers na protoboard (macho-macho)

| Fio | Origem | Destino | Cor sugerida | Função |
|---|---|---|---|---|
| 7 | coluna 43, fileira e | trilho + inferior, logo abaixo | vermelho | VCC do OLED |
| 8 | coluna 44, fileira e | trilho − inferior, logo abaixo | preto | GND do OLED |
| 9 | coluna 14, fileira j | trilho − inferior, logo abaixo | preto | − do LED |
| 10 | coluna 20, fileira j | trilho − inferior, logo abaixo | preto | botão |

No desenho do Wokwi, os fios pretos fazem um pequeno desvio antes de entrar no
trilho − para não passar sobre os furos do trilho +. Na montagem real, basta
usar o furo do trilho − mais próximo.

### Trilhos divididos

Em algumas protoboards de 830 furos os trilhos são interrompidos no meio, o que
aparece como uma falha na linha vermelha ou azul. Os fios 5 e 6 entram na
extremidade esquerda, e os fios 7 e 8 ficam perto da coluna 43, depois do meio.
Teste a continuidade entre um furo do trilho perto da coluna 3 e outro perto da
coluna 43. Se não houver continuidade, ligue as duas metades de cada trilho com
um jumper (+ com +, − com −).

## Passo 4: teste da fiação

Antes de gravar o firmware completo, grave o sketch
`ferramentas/teste-montagem/teste-montagem.ino`, que não precisa de Wi-Fi nem de
bibliotecas. Abra o monitor serial em 115200 e verifique:

| Resultado esperado | Se não ocorrer, verifique |
|---|---|
| `OK    OLED encontrado no endereco 0x3C` | fios 1, 2, 7 e 8 e a ordem dos pinos do OLED |
| o LED pisca 3 vezes logo no início | fio 3, o resistor, a polaridade do LED e o fio 9 |
| `OK    botao apertado -> LED aceso` ao pressionar o botão | fios 4 e 10 e os pares de pernas do botão |

Com os três resultados corretos, grave o `BitcoinTracker.ino` conforme o README.

## Problemas comuns

| Sintoma | Causa provável |
|---|---|
| Nada acende, nem o LED vermelho do ESP32 | cabo USB apenas de carga (sem dados) ou mal encaixado |
| OLED não responde | SDA e SCL trocados (fios 1 e 2) ou VCC e GND do OLED invertidos |
| LED nunca acende | LED invertido (a perna longa vai na coluna 13) ou resistor fora das colunas 7 e 13 |
| Botão permanentemente acionado | pares de pernas do botão na posição errada (veja o teste com multímetro) |
| Texto do OLED de cabeça para baixo | inverta o valor de `GIRAR_TELA_180` no `BitcoinTracker.ino` |
