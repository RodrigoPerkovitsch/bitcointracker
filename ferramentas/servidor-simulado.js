// Imita as rotas do ESP32 (/, /dados, /historico) com preços inventados, para
// testar a página sem a placa e sem depender da CoinGecko.
//
//   node ferramentas/servidor-simulado.js            -> http://localhost:8765
//
// Rotas extras só para teste:
//   /simular/reiniciar   apaga tudo, como se a placa tivesse reiniciado
//   /simular/lento       liga/desliga respostas lentas em parte das consultas
//                        (imita o ESP32 ocupado falando com a CoinGecko)
//
// Para os testes andarem rápido, o "preço novo" chega a cada 10 s, e não a cada 60 s.

const http = require('http');
const fs = require('fs');
const path = require('path');

const PORTA = Number(process.env.PORTA) || 8765;
const INTERVALO_S = 10;
const PAGINA = path.join(__dirname, '..', 'web', 'index.html');

let estado;
let lento = false;

function reiniciar() {
  estado = {
    preco: 96000 + Math.random() * 4000,
    precoAnterior: null,
    atualizadoEm: null,
    momento: null,
    referencia24h: null,
    historico: [],
    inicio: 0,
  };
  // Semeia 2 h de histórico, como se a placa estivesse ligada há um tempo.
  const agora = Math.floor(Date.now() / 1000);
  estado.referencia24h = estado.preco * 1.015;
  for (let i = 120; i >= 1; i--) novoPreco(agora - i * 60);
}

function novoPreco(atualizadoEm) {
  estado.precoAnterior = estado.atualizadoEm == null ? null : estado.preco;
  estado.preco += (Math.random() + Math.random() + Math.random() - 1.5) * 60;
  estado.atualizadoEm = atualizadoEm;
  estado.momento = Date.now();
  estado.historico.push({ atualizadoEm, preco: Math.round(estado.preco * 100) / 100 });
  if (estado.historico.length > 1440) estado.historico.shift();
}

reiniciar();
setInterval(() => {
  // Depois de "reiniciar", a placa leva ~30 s (Wi-Fi + primeira consulta) até ter preço.
  if (Date.now() - estado.inicio < 30000) return;
  novoPreco(Math.floor(Date.now() / 1000));
}, INTERVALO_S * 1000);

function json(res, corpo) {
  res.writeHead(200, { 'Content-Type': 'application/json', 'Cache-Control': 'no-store' });
  res.end(JSON.stringify(corpo));
}

http.createServer((req, res) => {
  const url = req.url.split('?')[0];

  if (url === '/') {
    res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
    return res.end(fs.readFileSync(PAGINA));
  }

  if (url === '/dados') {
    const responder = () => {
      const valida = estado.atualizadoEm != null;
      const idade = valida ? Math.floor((Date.now() - estado.momento) / 1000) : null;
      json(res, {
        valida,
        intervaloSegundos: INTERVALO_S,
        ...(valida && {
          preco: Math.round(estado.preco * 100) / 100,
          atualizadoEm: estado.atualizadoEm,
          idadeSegundos: idade,
          precoAnterior: estado.precoAnterior,
          variacao24h: (estado.preco / estado.referencia24h - 1) * 100,
        }),
        idadeConsultaSegundos: valida ? idade : null,
        erro: null,
      });
    };
    // ~40% das consultas demoram 7 s: o ESP32 real fica assim enquanto fala com a CoinGecko.
    if (lento && Math.random() < 0.4) return setTimeout(responder, 7000);
    return responder();
  }

  if (url === '/historico') return json(res, { pontos: estado.historico });

  if (url === '/simular/reiniciar') {
    estado = { preco: estado.preco, precoAnterior: null, atualizadoEm: null, momento: null,
      referencia24h: estado.preco, historico: [], inicio: Date.now() };
    return json(res, { ok: true, mensagem: 'Placa "reiniciada": sem preço e sem histórico.' });
  }

  if (url === '/simular/lento') {
    lento = !lento;
    return json(res, { ok: true, lento });
  }

  res.writeHead(404, { 'Content-Type': 'text/plain' });
  res.end('Nao encontrado');
}).listen(PORTA, () => console.log('Servidor simulado em http://localhost:' + PORTA));
