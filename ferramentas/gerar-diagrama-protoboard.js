// Gera wokwi/diagram.json com o circuito montado na protoboard de 830 furos.
//
//   node ferramentas/gerar-diagrama-protoboard.js
//
// As posições são calculadas, não arrastadas à mão: assim cada perna cai
// exatamente num furo e nenhum fio cruza outro. Para mudar algo de lugar,
// altere as colunas/fileiras abaixo e rode de novo.
//
// Geometria medida no próprio Wokwi (protoboard em left/top = 0):
//   furo da coluna n  -> x = 26 + (n - 1) * 9.6
//   fileiras          -> a..e e f..j (y na tabela FILEIRA)
//   trilhos           -> tp/tn (em cima), bp/bn (embaixo); 50 furos em grupos de 5

const fs = require('fs');
const path = require('path');

const MM = 3.7795;     // px por mm no Wokwi
const PASSO = 9.6;     // 2,54 mm entre furos

const BB = { left: 160, top: 0 };
const FILEIRA = { a: 51, b: 60.6, c: 70.2, d: 79.8, e: 89.4, f: 118.2, g: 127.8, h: 137.4, i: 147, j: 156.6 };
const TRILHO_Y = { tp: 12.3, tn: 22.3, bp: 186.3, bn: 196.3 };

const xColuna = (n) => BB.left + 26 + (n - 1) * PASSO;
const yFileira = (f) => BB.top + FILEIRA[f];
const furo = (n, f) => `bb1:${n}${'abcde'.includes(f) ? 't' : 'b'}.${f}`;

// Trilho: grupos de 5 furos a cada 6 colunas, começando na coluna 3.
function furoTrilho(trilho, coluna) {
  const g = Math.floor((coluna - 3) / 6);
  const h = (coluna - 3) % 6;
  if (h > 4) throw new Error(`coluna ${coluna} cai no espaço entre grupos do trilho`);
  return { id: `bb1:${trilho}.${g * 5 + h + 1}`, x: BB.left + 46 + g * 57.6 + h * PASSO, y: BB.top + TRILHO_Y[trilho] };
}

// ESP32 DevKit V1 (30 pinos). Pinos do lado direito, em mm (wokwi-boards).
const ESP_PINOS = { D22: [26.8, 8.34], D21: [26.8, 15.96], RX2: [26.8, 28.66], D4: [26.8, 31.2], '3V3': [26.8, 41.36], 'GND.2': [1.27, 38.82] };
// Posição escolhida para o D22 cair na altura da fileira b: assim D21 fica na
// fileira e, RX2 na h, D4 na i e o 3V3 na altura do trilho + de baixo.
const ESP = { left: 0, top: yFileira('b') - 8.34 * MM };
const pinoEsp = (nome) => ({ x: ESP.left + ESP_PINOS[nome][0] * MM, y: ESP.top + ESP_PINOS[nome][1] * MM });

// ---- Componentes (colunas escolhidas para nenhum fio cruzar outro) ----
const OLED = { sda: 41 };          // SDA 41, SCL 42, VCC 43, GND 44 (display girado 180°)
const LED_RES = { sinal: 7 };      // resistor da coluna 7 à 13 na fileira j
const LED = { anodo: 13 };         // anodo 13, catodo 14, pernas na fileira h
const BOTAO = { col: 18 };         // pernas nas colunas 18 e 20, fileiras f e i

const partes = [];
const ligacoes = [];
const espetar = (pino, alvo) => ligacoes.push([pino, alvo, '', ['$bb']]);
const fio = (de, para, cor, rota = []) => ligacoes.push([de, para, cor, rota]);
const r2 = (v) => Math.round(v * 100) / 100;

partes.push({ type: 'wokwi-breadboard', id: 'bb1', top: BB.top, left: BB.left, attrs: {} });
partes.push({ type: 'board-esp32-devkit-v1', id: 'esp', top: r2(ESP.top), left: ESP.left, attrs: {} });

// OLED girado 180°: pinos embaixo, tela para fora da borda de cima.
// Pinos (mm) sem girar: GND 10.1, VCC 12.6, SCL 15.1, SDA 17.7; y 1.71. Tamanho 27.7 x 22.6.
{
  const W = 27.7 * MM, H = 22.6 * MM;
  const xSdaGirado = W - 17.7 * MM;
  const yGirado = H - 1.71 * MM;
  partes.push({
    type: 'board-ssd1306', id: 'oled', rotate: 180,
    top: r2(yFileira('a') - yGirado), left: r2(xColuna(OLED.sda) - xSdaGirado),
    attrs: { i2cAddress: '0x3c' },
  });
  espetar('oled:SDA', furo(OLED.sda, 'a'));
  espetar('oled:SCL', furo(OLED.sda + 1, 'a'));
  espetar('oled:VCC', furo(OLED.sda + 2, 'a'));
  espetar('oled:GND', furo(OLED.sda + 3, 'a'));
}

// Resistor de 220 Ω deitado na fileira j.
partes.push({
  type: 'wokwi-resistor', id: 'r220',
  top: r2(yFileira('j') - 5.65), left: r2(xColuna(LED_RES.sinal)),
  attrs: { value: '220' },
});
espetar('r220:1', furo(LED_RES.sinal, 'j'));
espetar('r220:2', furo(LED_RES.sinal + 6, 'j'));

// LED com flip: anodo (perna longa) à esquerda, catodo à direita.
partes.push({
  type: 'wokwi-led', id: 'led',
  top: r2(yFileira('h') - 42), left: r2(xColuna(LED.anodo) - 15),
  attrs: { color: 'orange', flip: '1' },
});
espetar('led:A', furo(LED.anodo, 'h'));
espetar('led:C', furo(LED.anodo + 1, 'h'));

// Chave táctil 6x6 girada 270°: pernas "1" na coluna 18, "2" na coluna 20.
// Apertar liga a coluna 18 com a 20. Medido no Wokwi: com o giro, as pernas
// "1" ficam em (left + 3.2, top - 1) e (left + 3.2, top + 27).
partes.push({
  type: 'wokwi-pushbutton-6mm', id: 'botao', rotate: 270,
  top: r2(yFileira('f') + 1), left: r2(xColuna(BOTAO.col) - 3.2),
  attrs: { color: 'green' },
});
espetar('botao:1.r', furo(BOTAO.col, 'f'));
espetar('botao:1.l', furo(BOTAO.col, 'i'));
espetar('botao:2.r', furo(BOTAO.col + 2, 'f'));
espetar('botao:2.l', furo(BOTAO.col + 2, 'i'));

// ---- Fios ----
fio('esp:TX0', '$serialMonitor:RX', '');
fio('esp:RX0', '$serialMonitor:TX', '');

// Sinais: cada um sai reto do ESP32 na altura da própria fileira.
fio('esp:D22', furo(OLED.sda + 1, 'b'), 'gold');     // SCL
fio('esp:D21', furo(OLED.sda, 'e'), 'blue');         // SDA
fio('esp:RX2', furo(LED_RES.sinal, 'h'), 'orange');  // LED (GPIO16)
{
  // Botão (GPIO4): vem na fileira i, desce meia coluna antes do botão e entra na fileira j.
  const p = pinoEsp('D4');
  const xDobra = xColuna(BOTAO.col) - PASSO / 2;
  fio('esp:D4', furo(BOTAO.col, 'j'), 'green', [`h${r2(xDobra - p.x)}`, `v${r2(yFileira('j') - yFileira('i'))}`]);
}

// Alimentação: 3V3 reto no + de baixo; GND pelo pino do lado esquerdo,
// contornando o ESP32 por baixo para não cruzar o fio do 3V3.
const mais = furoTrilho('bp', 3);
fio('esp:3V3', mais.id, 'red');
{
  const g = pinoEsp('GND.2');
  // Coluna 4 (e não 3): assim a ponta preta não fica alinhada com a ponta do 3V3.
  const menos = furoTrilho('bn', 4);
  const yBaixo = ESP.top + 53 * MM + 16;
  fio('esp:GND.2', menos.id, 'black', ['h-15', `v${r2(yBaixo - g.y)}`, `h${r2(menos.x - (g.x - 15))}`, `v${r2(menos.y - yBaixo)}`]);
}

// Jumpers na protoboard. Os de GND descem por uma coluna sem furo no trilho +
// (o vão entre grupos) e só então entram no -, para não parecerem ligados ao +.
const Y_VAO = (TRILHO_Y.bp + TRILHO_Y.bn) / 2;
function jumperGnd(coluna, fileira) {
  const destino = furoTrilho('bn', coluna + 1);
  const y0 = yFileira(fileira);
  fio(furo(coluna, fileira), destino.id, 'black', [`v${r2(BB.top + Y_VAO - y0)}`, `h${r2(destino.x - xColuna(coluna))}`]);
}
fio(furo(OLED.sda + 2, 'e'), furoTrilho('bp', OLED.sda + 2).id, 'red');  // VCC do OLED -> +
jumperGnd(OLED.sda + 3, 'e');                                             // GND do OLED -> -
jumperGnd(LED.anodo + 1, 'j');                                            // catodo do LED -> -
jumperGnd(BOTAO.col + 2, 'j');                                            // botão -> -

const diagrama = { version: 1, author: 'Bitcoin Tracker', editor: 'wokwi', parts: partes, connections: ligacoes, dependencies: {} };
const destino = path.join(__dirname, '..', 'wokwi', 'diagram.json');
fs.writeFileSync(destino, JSON.stringify(diagrama, null, 2) + '\n');
console.log('wokwi/diagram.json gerado:', partes.length, 'partes,', ligacoes.length, 'ligações');
