// ARQUIVO GERADO por gerar-pagina.ps1 a partir de web/index.html.
// Nao edite aqui: edite web/index.html e rode o script de novo.
#pragma once

const char PAGINA_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Bitcoin Tracker</title>
<style>
  :root {
    --fundo: #0e1117;
    --cartao: #161b22;
    --borda: #2a313c;
    --texto: #e6edf3;
    --suave: #8b949e;
    --marca: #f7931a;
    --alta: #2ea043;
    --baixa: #f85149;
    --neutro: #8b949e;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0;
    background: var(--fundo);
    color: var(--texto);
    font-family: system-ui, -apple-system, "Segoe UI", Roboto, sans-serif;
    line-height: 1.4;
  }
  main { max-width: 980px; margin: 0 auto; padding: 20px 16px 40px; }
  header { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; margin-bottom: 16px; }
  h1 { margin: 0; font-size: 20px; letter-spacing: .02em; }
  h1 span { color: var(--marca); }

  .status {
    display: inline-flex; align-items: center; gap: 8px;
    font-size: 13px; color: var(--suave);
    background: var(--cartao); border: 1px solid var(--borda); border-radius: 999px;
    padding: 6px 12px;
  }
  .status i { width: 8px; height: 8px; border-radius: 50%; background: var(--neutro); flex: none; }
  .status[data-estado="ok"] i { background: var(--alta); }
  .status[data-estado="aviso"] i { background: var(--marca); }
  .status[data-estado="falha"] i { background: var(--baixa); }

  .cartao { background: var(--cartao); border: 1px solid var(--borda); border-radius: 12px; padding: 16px; }
  .rotulo { font-size: 12px; color: var(--suave); text-transform: uppercase; letter-spacing: .06em; }

  .destaque { display: flex; flex-wrap: wrap; align-items: flex-end; justify-content: space-between; gap: 16px; margin-bottom: 12px; }
  .preco { font-size: clamp(34px, 8vw, 52px); font-weight: 700; font-variant-numeric: tabular-nums; line-height: 1.1; margin-top: 4px; }
  .preco.velho { opacity: .45; }
  .idade { font-size: 13px; color: var(--suave); margin-top: 6px; }
  .variacoes { display: flex; gap: 24px; flex-wrap: wrap; }
  .var { font-size: 15px; font-weight: 600; font-variant-numeric: tabular-nums; margin-top: 2px; }
  .alta { color: var(--alta); }
  .baixa { color: var(--baixa); }
  .neutra { color: var(--neutro); }

  .barra-grafico { display: flex; justify-content: space-between; align-items: center; gap: 8px; flex-wrap: wrap; margin-bottom: 10px; }
  .periodos { display: flex; gap: 4px; flex-wrap: wrap; }
  .periodos button {
    background: none; border: 1px solid transparent; color: var(--suave);
    border-radius: 6px; padding: 4px 10px; font: inherit; font-size: 13px; cursor: pointer;
  }
  .periodos button:hover { color: var(--texto); }
  .periodos button[aria-pressed="true"] { background: var(--borda); color: var(--texto); }
  .grafico { position: relative; height: 300px; touch-action: pan-y; }
  canvas { display: block; width: 100%; height: 100%; }
  .vazio {
    position: absolute; inset: 0; display: flex; align-items: center; justify-content: center;
    text-align: center; color: var(--suave); font-size: 14px; padding: 16px;
  }
  .dica {
    position: absolute; pointer-events: none; display: none;
    background: #0b0e13; border: 1px solid var(--borda); border-radius: 8px;
    padding: 6px 10px; font-size: 12px; white-space: nowrap;
    transform: translate(-50%, calc(-100% - 12px));
    font-variant-numeric: tabular-nums;
  }
  .dica b { display: block; font-size: 14px; color: var(--texto); }
  .dica span { color: var(--suave); }

  .indicadores { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 12px; margin: 12px 0; }
  .indicadores .valor { font-size: 18px; font-weight: 600; font-variant-numeric: tabular-nums; margin-top: 4px; }
  .indicadores .sub { font-size: 12px; color: var(--suave); margin-top: 2px; }

  table { width: 100%; border-collapse: collapse; font-size: 14px; font-variant-numeric: tabular-nums; margin-top: 8px; }
  th, td { padding: 8px 4px; text-align: right; border-bottom: 1px solid var(--borda); }
  th:first-child, td:first-child { text-align: left; }
  tr:last-child td { border-bottom: none; }
  th { color: var(--suave); font-weight: 500; font-size: 12px; text-transform: uppercase; letter-spacing: .06em; }

  footer { display: flex; justify-content: space-between; align-items: center; gap: 12px; flex-wrap: wrap; margin-top: 16px; font-size: 12px; color: var(--suave); }
  footer button {
    background: none; border: 1px solid var(--borda); color: var(--suave);
    border-radius: 6px; padding: 6px 10px; font: inherit; cursor: pointer;
  }
  footer button:hover { color: var(--texto); }

  @media (max-width: 600px) { .grafico { height: 230px; } }
</style>
</head>
<body>
<main>
  <header>
    <h1><span>₿</span> Bitcoin Tracker</h1>
    <div class="status" id="status" data-estado="espera"><i></i><span id="status-texto">Conectando ao ESP32…</span></div>
  </header>

  <section class="cartao destaque">
    <div>
      <div class="rotulo">BTC / USD</div>
      <div class="preco" id="preco">—</div>
      <div class="idade" id="idade">Aguardando a primeira leitura</div>
    </div>
    <div class="variacoes">
      <div>
        <div class="rotulo">Desde a leitura anterior</div>
        <div class="var neutra" id="var-anterior">—</div>
      </div>
      <div>
        <div class="rotulo">Últimas 24 h</div>
        <div class="var neutra" id="var-24h">—</div>
      </div>
    </div>
  </section>

  <section class="cartao">
    <div class="barra-grafico">
      <div class="rotulo" id="rotulo-historico">Histórico</div>
      <div class="periodos" id="periodos">
        <button type="button" data-min="15">15 min</button>
        <button type="button" data-min="60" aria-pressed="true">1 h</button>
        <button type="button" data-min="360">6 h</button>
        <button type="button" data-min="1440">24 h</button>
        <button type="button" data-min="0">Tudo</button>
      </div>
    </div>
    <div class="grafico" id="area-grafico">
      <canvas id="grafico" aria-label="Gráfico do preço do bitcoin ao longo do tempo" role="img"></canvas>
      <div class="vazio" id="vazio">Aguardando leituras…</div>
      <div class="dica" id="dica"></div>
    </div>
  </section>

  <section class="indicadores">
    <div class="cartao"><div class="rotulo">Abertura do período</div><div class="valor" id="ind-abertura">—</div><div class="sub" id="ind-abertura-sub">&nbsp;</div></div>
    <div class="cartao"><div class="rotulo">Máxima</div><div class="valor" id="ind-maxima">—</div><div class="sub" id="ind-maxima-sub">&nbsp;</div></div>
    <div class="cartao"><div class="rotulo">Mínima</div><div class="valor" id="ind-minima">—</div><div class="sub" id="ind-minima-sub">&nbsp;</div></div>
    <div class="cartao"><div class="rotulo">Variação no período</div><div class="valor" id="ind-variacao">—</div><div class="sub" id="ind-variacao-sub">&nbsp;</div></div>
  </section>

  <section class="cartao">
    <div class="rotulo">Últimas leituras</div>
    <table>
      <thead><tr><th>Hora</th><th>Preço</th><th>Variação</th></tr></thead>
      <tbody id="tabela"></tbody>
    </table>
  </section>

  <footer>
    <span id="rodape">O ESP32 consulta a CoinGecko a cada 60 s; esta página pergunta ao ESP32 a cada 10 s.</span>
    <button type="button" id="limpar">Limpar histórico</button>
  </footer>
</main>

<script>
(function () {
  'use strict';

  // Aberta direto do disco (file://) ou com ?simulado, a página gera dados
  // falsos. Serve para mostrar a tela sem a placa ligada.
  const SIMULADO = location.protocol === 'file:' || new URLSearchParams(location.search).has('simulado');
  const INTERVALO_CONSULTA_MS = SIMULADO ? 3000 : 10000;
  const TEMPO_LIMITE_MS = 5000;
  const LIMITE_PONTOS = 2000;          // ~33 h com uma leitura por minuto
  const LACUNA_MS = 10 * 60 * 1000;    // acima disso a linha do gráfico é interrompida
  const CHAVE = SIMULADO ? null : 'btc-tracker-historico-v1';

  const fmtPreco = new Intl.NumberFormat('pt-BR', { style: 'currency', currency: 'USD' });
  const fmtEixo0 = new Intl.NumberFormat('pt-BR', { maximumFractionDigits: 0 });
  const fmtEixo2 = new Intl.NumberFormat('pt-BR', { minimumFractionDigits: 2, maximumFractionDigits: 2 });
  const fmtPct = new Intl.NumberFormat('pt-BR', { minimumFractionDigits: 2, maximumFractionDigits: 2 });
  const fmtHora = new Intl.DateTimeFormat('pt-BR', { hour: '2-digit', minute: '2-digit' });
  const fmtHoraSeg = new Intl.DateTimeFormat('pt-BR', { hour: '2-digit', minute: '2-digit', second: '2-digit' });

  const $ = (id) => document.getElementById(id);

  // ---------- Estado ----------
  let historico = carregarHistorico();  // [{ t: ms, p: preço, k: "sessao-leitura" }] em ordem de tempo
  let ultimo = null;                    // última resposta de /dados
  let momentoLeitura = null;            // quando o ESP32 obteve o preço, no relógio do navegador
  let ultimoContato = null;
  let falhou = false;
  let periodoMin = 60;
  let origem = SIMULADO ? 'simulado' : 'navegador';
  let hover = null;
  let geometria = null;

  // ---------- Histórico ----------
  function carregarHistorico() {
    if (!CHAVE) return [];
    try {
      const v = JSON.parse(localStorage.getItem(CHAVE));
      return Array.isArray(v) ? v.filter((x) => x && isFinite(x.t) && isFinite(x.p) && x.k) : [];
    } catch (e) { return []; }
  }

  function salvarHistorico() {
    if (!CHAVE) return;
    try { localStorage.setItem(CHAVE, JSON.stringify(historico)); } catch (e) { /* navegador sem armazenamento: segue só em memória */ }
  }

  // A chave sessao-leitura evita duplicar: a página consulta a cada 10 s, mas o
  // ESP32 só tem preço novo a cada 60 s.
  function adicionarPontos(novos) {
    const chaves = new Set(historico.map((x) => x.k));
    let mudou = false;
    for (const n of novos) {
      if (!chaves.has(n.k)) { historico.push(n); chaves.add(n.k); mudou = true; }
    }
    if (!mudou) return;
    historico.sort((a, b) => a.t - b.t);
    if (historico.length > LIMITE_PONTOS) historico = historico.slice(-LIMITE_PONTOS);
    salvarHistorico();
  }

  function pontosDoPeriodo() {
    if (!periodoMin) return historico;
    const corte = Date.now() - periodoMin * 60000;
    return historico.filter((x) => x.t >= corte);
  }

  // ---------- Comunicação com o ESP32 ----------
  async function buscarJson(url) {
    const ctrl = new AbortController();
    const timer = setTimeout(() => ctrl.abort(), TEMPO_LIMITE_MS);
    try {
      const r = await fetch(url, { signal: ctrl.signal, cache: 'no-store' });
      if (!r.ok) throw new Error('HTTP ' + r.status);
      return await r.json();
    } finally {
      clearTimeout(timer);
    }
  }

  // Preparado para a opção B: se o ESP32 tiver a rota /historico, a página já
  // abre com o gráfico preenchido. Sem a rota (opção A), o 404 é ignorado.
  async function carregarHistoricoDoEsp32() {
    if (SIMULADO) return;
    try {
      const d = await buscarJson('/historico');
      if (!d || !Array.isArray(d.pontos)) return;
      const agora = Date.now();
      adicionarPontos(d.pontos
        .filter((x) => isFinite(x.preco) && isFinite(x.idadeSegundos))
        .map((x) => ({ t: agora - x.idadeSegundos * 1000, p: x.preco, k: d.sessao + '-' + x.leitura })));
      origem = 'esp32';
    } catch (e) { /* rota ainda não existe */ }
  }

  async function consultar() {
    try {
      const d = SIMULADO ? simulador.proximo() : await buscarJson('/dados');
      ultimo = d;
      ultimoContato = Date.now();
      falhou = false;
      if (d.valida) {
        momentoLeitura = Date.now() - (d.idadeSegundos || 0) * 1000;
        adicionarPontos([{ t: momentoLeitura, p: d.preco, k: d.sessao + '-' + d.leitura }]);
      }
    } catch (e) {
      falhou = true;
    }
    renderizarTudo();
    setTimeout(consultar, INTERVALO_CONSULTA_MS);
  }

  // ---------- Formatação ----------
  function tempoRelativo(ms) {
    const s = Math.max(0, Math.round(ms / 1000));
    if (s < 60) return s + ' s';
    if (s < 3600) return Math.floor(s / 60) + ' min';
    return Math.floor(s / 3600) + ' h ' + Math.floor((s % 3600) / 60) + ' min';
  }

  function direcao(delta) { return delta > 0 ? 'alta' : delta < 0 ? 'baixa' : 'neutra'; }
  function seta(delta) { return delta > 0 ? '▲' : delta < 0 ? '▼' : '='; }
  function sinal(delta) { return delta > 0 ? '+' : delta < 0 ? '−' : ''; }

  // Cor nunca sozinha: sempre seta e sinal junto, para quem não distingue verde de vermelho.
  function textoVariacao(delta, base) {
    if (delta === 0) return '= sem alteração';
    const pct = base ? (delta / base) * 100 : 0;
    return seta(delta) + ' ' + sinal(delta) + fmtPreco.format(Math.abs(delta)) +
      ' (' + sinal(delta) + fmtPct.format(Math.abs(pct)) + '%)';
  }

  function escrever(el, texto, classe) {
    el.textContent = texto;
    el.classList.remove('alta', 'baixa', 'neutra');
    if (classe) el.classList.add(classe);
  }

  // ---------- Renderização ----------
  function renderizarStatus() {
    let estado, texto;
    if (SIMULADO) { estado = 'aviso'; texto = 'Modo simulado (sem ESP32)'; }
    else if (falhou && !ultimoContato) { estado = 'falha'; texto = 'ESP32 não respondeu'; }
    else if (!ultimoContato) { estado = 'espera'; texto = 'Conectando ao ESP32…'; }
    else if (falhou) { estado = 'falha'; texto = 'Sem resposta do ESP32 há ' + tempoRelativo(Date.now() - ultimoContato); }
    else if (ultimo && ultimo.erro) { estado = 'aviso'; texto = 'ESP32: ' + ultimo.erro; }
    else { estado = 'ok'; texto = 'Conectado ao ESP32'; }
    $('status').dataset.estado = estado;
    $('status-texto').textContent = texto;
  }

  function renderizarDestaque() {
    if (!ultimo || !ultimo.valida) {
      if (ultimo && ultimo.erro) $('idade').textContent = 'O ESP32 ainda não obteve o preço: ' + ultimo.erro;
      return;
    }
    $('preco').textContent = fmtPreco.format(ultimo.preco);

    const intervalo = (ultimo.intervaloSegundos || 60) * 1000;
    const idade = Date.now() - momentoLeitura;
    const velho = falhou || idade > intervalo * 2.5;
    $('preco').classList.toggle('velho', velho);

    let texto = 'Lido pelo ESP32 há ' + tempoRelativo(idade);
    if (velho) texto += ' · valor desatualizado';
    else if (idade < intervalo) texto += ' · próxima em ~' + tempoRelativo(intervalo - idade);
    else texto += ' · atualizando…';
    $('idade').textContent = texto;

    if (ultimo.precoAnterior == null) {
      escrever($('var-anterior'), 'Primeira leitura', 'neutra');
    } else {
      const delta = ultimo.preco - ultimo.precoAnterior;
      escrever($('var-anterior'), textoVariacao(delta, ultimo.precoAnterior), direcao(delta));
    }

    const v24 = ultimo.variacao24h;
    escrever($('var-24h'), seta(v24) + ' ' + sinal(v24) + fmtPct.format(Math.abs(v24)) + '%', direcao(v24));
  }

  function renderizarIndicadores() {
    const pts = pontosDoPeriodo();
    const ids = ['abertura', 'maxima', 'minima', 'variacao'];
    if (!pts.length) {
      ids.forEach((id) => { escrever($('ind-' + id), '—', null); $('ind-' + id + '-sub').innerHTML = '&nbsp;'; });
      return;
    }
    let max = pts[0], min = pts[0];
    for (const x of pts) { if (x.p > max.p) max = x; if (x.p < min.p) min = x; }
    const abertura = pts[0], atual = pts[pts.length - 1];
    const delta = atual.p - abertura.p;

    escrever($('ind-abertura'), fmtPreco.format(abertura.p), null);
    $('ind-abertura-sub').textContent = 'às ' + fmtHora.format(abertura.t);
    escrever($('ind-maxima'), fmtPreco.format(max.p), null);
    $('ind-maxima-sub').textContent = 'às ' + fmtHora.format(max.t);
    escrever($('ind-minima'), fmtPreco.format(min.p), null);
    $('ind-minima-sub').textContent = 'às ' + fmtHora.format(min.t);
    escrever($('ind-variacao'), delta === 0 ? '= sem alteração'
      : seta(delta) + ' ' + sinal(delta) + fmtPct.format(Math.abs(delta / abertura.p * 100)) + '%', direcao(delta));
    $('ind-variacao-sub').textContent = sinal(delta) + fmtPreco.format(Math.abs(delta)) + ' · ' + pts.length + ' leituras';
  }

  function renderizarTabela() {
    const tb = $('tabela');
    tb.replaceChildren();
    const n = historico.length;
    if (!n) {
      const tr = tb.insertRow();
      const td = tr.insertCell();
      td.colSpan = 3;
      td.textContent = 'Nenhuma leitura ainda.';
      td.style.color = 'var(--suave)';
      return;
    }
    for (let i = n - 1; i >= Math.max(0, n - 8); i--) {
      const atual = historico[i], ant = historico[i - 1];
      const tr = tb.insertRow();
      tr.insertCell().textContent = fmtHoraSeg.format(atual.t);
      tr.insertCell().textContent = fmtPreco.format(atual.p);
      const td = tr.insertCell();
      if (ant) {
        const d = atual.p - ant.p;
        td.textContent = d === 0 ? '=' : seta(d) + ' ' + sinal(d) + fmtEixo2.format(Math.abs(d));
        td.className = direcao(d);
      } else {
        td.textContent = '—';
        td.className = 'neutra';
      }
    }
  }

  function renderizarRotulos() {
    $('rotulo-historico').textContent =
      origem === 'esp32' ? 'Histórico guardado pelo ESP32'
      : origem === 'simulado' ? 'Histórico (dados simulados)'
      : 'Histórico registrado neste navegador';
    if (ultimo && ultimo.intervaloSegundos && !SIMULADO) {
      $('rodape').textContent = 'O ESP32 consulta a CoinGecko a cada ' + ultimo.intervaloSegundos +
        ' s; esta página pergunta ao ESP32 a cada ' + INTERVALO_CONSULTA_MS / 1000 + ' s.';
    } else if (SIMULADO) {
      $('rodape').textContent = 'Modo simulado: os preços são inventados e mudam a cada 3 s.';
    }
  }

  function renderizarTudo() {
    renderizarStatus();
    renderizarDestaque();
    renderizarRotulos();
    desenharGrafico();
    renderizarIndicadores();
    renderizarTabela();
  }

  // ---------- Gráfico (canvas, sem biblioteca: a página precisa caber no ESP32) ----------
  const canvas = $('grafico');
  const ctx = canvas.getContext('2d');
  const areaGrafico = $('area-grafico');

  function cssVar(nome) { return getComputedStyle(document.documentElement).getPropertyValue(nome).trim(); }

  function comTransparencia(hex, a) {
    const n = parseInt(hex.slice(1), 16);
    return 'rgba(' + (n >> 16 & 255) + ',' + (n >> 8 & 255) + ',' + (n & 255) + ',' + a + ')';
  }

  function dividirEmSegmentos(pts) {
    const segs = [[pts[0]]];
    for (let i = 1; i < pts.length; i++) {
      if (pts[i].t - pts[i - 1].t > LACUNA_MS) segs.push([]);
      segs[segs.length - 1].push(pts[i]);
    }
    return segs;
  }

  function desenharGrafico() {
    const w = areaGrafico.clientWidth, h = areaGrafico.clientHeight;
    const dpr = window.devicePixelRatio || 1;
    if (canvas.width !== Math.round(w * dpr) || canvas.height !== Math.round(h * dpr)) {
      canvas.width = Math.round(w * dpr);
      canvas.height = Math.round(h * dpr);
    }
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    ctx.clearRect(0, 0, w, h);

    const pts = pontosDoPeriodo();
    const vazio = $('vazio'), dica = $('dica');
    if (pts.length < 2) {
      geometria = null;
      dica.style.display = 'none';
      vazio.style.display = 'flex';
      vazio.textContent = pts.length === 0
        ? 'Nenhuma leitura neste período ainda.'
        : 'Uma leitura registrada. O gráfico aparece a partir da segunda.';
      return;
    }
    vazio.style.display = 'none';

    const m = { esq: 4, dir: 70, cima: 12, baixo: 26 };
    const larg = w - m.esq - m.dir, alt = h - m.cima - m.baixo;

    let min = Infinity, max = -Infinity;
    for (const x of pts) { if (x.p < min) min = x.p; if (x.p > max) max = x.p; }
    let folga = (max - min) * 0.12;
    if (folga === 0) folga = Math.max(1, max * 0.0005);
    const yMin = min - folga, yMax = max + folga;
    const t0 = pts[0].t, t1 = pts[pts.length - 1].t;
    const X = (t) => m.esq + (t1 === t0 ? larg : (t - t0) / (t1 - t0) * larg);
    const Y = (p) => m.cima + (yMax - p) / (yMax - yMin) * alt;

    const cores = { borda: cssVar('--borda'), suave: cssVar('--suave') };
    const subiu = pts[pts.length - 1].p >= pts[0].p;
    const corLinha = cssVar(subiu ? '--alta' : '--baixa');
    const fmtEixo = (yMax - yMin) < 50 ? fmtEixo2 : fmtEixo0;

    // Grade horizontal e preços no eixo da direita, como em gráfico de corretora
    ctx.font = '11px system-ui, sans-serif';
    ctx.lineWidth = 1;
    ctx.textAlign = 'left';
    ctx.textBaseline = 'middle';
    for (let i = 0; i <= 4; i++) {
      const p = yMin + (yMax - yMin) * i / 4;
      const y = Math.round(Y(p)) + 0.5;
      ctx.strokeStyle = cores.borda;
      ctx.beginPath(); ctx.moveTo(m.esq, y); ctx.lineTo(m.esq + larg, y); ctx.stroke();
      ctx.fillStyle = cores.suave;
      ctx.fillText(fmtEixo.format(p), m.esq + larg + 8, y);
    }

    // Horários no eixo de baixo
    const fmtT = (t1 - t0) < 10 * 60000 ? fmtHoraSeg : fmtHora;
    const nRotulos = Math.max(2, Math.min(6, Math.floor(larg / 110)));
    ctx.textBaseline = 'top';
    for (let i = 0; i < nRotulos; i++) {
      const t = t0 + (t1 - t0) * i / (nRotulos - 1);
      ctx.textAlign = i === 0 ? 'left' : i === nRotulos - 1 ? 'right' : 'center';
      ctx.fillText(fmtT.format(t), X(t), m.cima + alt + 8);
    }

    // Linha tracejada no preço de abertura do período: referência de "subiu ou desceu"
    ctx.save();
    ctx.setLineDash([4, 4]);
    ctx.strokeStyle = comTransparencia(cores.suave, 0.6);
    const yAbertura = Math.round(Y(pts[0].p)) + 0.5;
    ctx.beginPath(); ctx.moveTo(m.esq, yAbertura); ctx.lineTo(m.esq + larg, yAbertura); ctx.stroke();
    ctx.restore();

    // Área e linha, interrompidas onde a página ficou fechada
    const gradiente = ctx.createLinearGradient(0, m.cima, 0, m.cima + alt);
    gradiente.addColorStop(0, comTransparencia(corLinha, 0.28));
    gradiente.addColorStop(1, comTransparencia(corLinha, 0));
    for (const seg of dividirEmSegmentos(pts)) {
      if (seg.length === 1) {
        ctx.fillStyle = corLinha;
        ctx.beginPath(); ctx.arc(X(seg[0].t), Y(seg[0].p), 2, 0, Math.PI * 2); ctx.fill();
        continue;
      }
      ctx.beginPath();
      seg.forEach((x, i) => (i ? ctx.lineTo(X(x.t), Y(x.p)) : ctx.moveTo(X(x.t), Y(x.p))));
      ctx.lineTo(X(seg[seg.length - 1].t), m.cima + alt);
      ctx.lineTo(X(seg[0].t), m.cima + alt);
      ctx.closePath();
      ctx.fillStyle = gradiente;
      ctx.fill();

      ctx.beginPath();
      seg.forEach((x, i) => (i ? ctx.lineTo(X(x.t), Y(x.p)) : ctx.moveTo(X(x.t), Y(x.p))));
      ctx.strokeStyle = corLinha;
      ctx.lineWidth = 2;
      ctx.lineJoin = 'round';
      ctx.stroke();
    }

    // Preço atual destacado no eixo
    const ult = pts[pts.length - 1];
    const yUlt = Y(ult.p);
    ctx.fillStyle = corLinha;
    ctx.beginPath(); ctx.arc(X(ult.t), yUlt, 3.5, 0, Math.PI * 2); ctx.fill();
    ctx.fillRect(m.esq + larg + 2, yUlt - 9, m.dir - 4, 18);
    ctx.fillStyle = '#fff';
    ctx.textAlign = 'left';
    ctx.textBaseline = 'middle';
    ctx.fillText(fmtEixo.format(ult.p), m.esq + larg + 8, yUlt);

    geometria = { pts, X, Y, m, alt };

    // Cursor: linha vertical + caixa com hora e preço
    if (hover != null && pts[hover]) {
      const p = pts[hover];
      const x = X(p.t), y = Y(p.p);
      ctx.save();
      ctx.setLineDash([3, 3]);
      ctx.strokeStyle = cores.suave;
      ctx.beginPath(); ctx.moveTo(Math.round(x) + 0.5, m.cima); ctx.lineTo(Math.round(x) + 0.5, m.cima + alt); ctx.stroke();
      ctx.restore();
      ctx.fillStyle = '#fff';
      ctx.beginPath(); ctx.arc(x, y, 4, 0, Math.PI * 2); ctx.fill();

      dica.replaceChildren();
      const b = document.createElement('b'); b.textContent = fmtPreco.format(p.p);
      const s = document.createElement('span'); s.textContent = fmtHoraSeg.format(p.t);
      dica.append(b, s);
      dica.style.display = 'block';
      dica.style.left = Math.min(Math.max(x, 70), w - 70) + 'px';
      dica.style.top = Math.max(y, 50) + 'px';
    } else {
      dica.style.display = 'none';
    }
  }

  function atualizarCursor(e) {
    if (!geometria) return;
    const r = canvas.getBoundingClientRect();
    const x = e.clientX - r.left;
    let melhor = 0, dist = Infinity;
    geometria.pts.forEach((p, i) => {
      const d = Math.abs(geometria.X(p.t) - x);
      if (d < dist) { dist = d; melhor = i; }
    });
    hover = melhor;
    desenharGrafico();
  }

  areaGrafico.addEventListener('pointermove', atualizarCursor);
  areaGrafico.addEventListener('pointerdown', atualizarCursor);
  areaGrafico.addEventListener('pointerleave', () => { hover = null; desenharGrafico(); });
  new ResizeObserver(() => desenharGrafico()).observe(areaGrafico);

  // ---------- Controles ----------
  $('periodos').addEventListener('click', (e) => {
    const botao = e.target.closest('button');
    if (!botao) return;
    periodoMin = Number(botao.dataset.min);
    document.querySelectorAll('#periodos button').forEach((b) => b.setAttribute('aria-pressed', String(b === botao)));
    hover = null;
    desenharGrafico();
    renderizarIndicadores();
  });

  $('limpar').addEventListener('click', () => {
    if (!confirm('Apagar o histórico guardado neste navegador?')) return;
    historico = [];
    salvarHistorico();
    hover = null;
    renderizarTudo();
  });

  // ---------- Simulador ----------
  function criarSimulador() {
    const sessao = 1;
    let leitura = 0;
    let preco = 96000 + Math.random() * 4000;
    let anterior = null;
    const referencia24h = preco * (1 + (Math.random() - 0.5) * 0.04);
    const passo = () => {
      anterior = preco;
      preco = Math.max(1000, preco + (Math.random() + Math.random() + Math.random() - 1.5) * 60);
      leitura++;
    };
    return {
      semear() {
        const agora = Date.now(), pts = [];
        for (let i = 180; i >= 1; i--) { passo(); pts.push({ t: agora - i * 60000, p: preco, k: sessao + '-' + leitura }); }
        adicionarPontos(pts);
      },
      proximo() {
        passo();
        return {
          sessao, leitura, valida: true, preco, precoAnterior: anterior,
          variacao24h: (preco / referencia24h - 1) * 100,
          idadeSegundos: 0, erro: null, intervaloSegundos: INTERVALO_CONSULTA_MS / 1000,
        };
      },
    };
  }

  // ---------- Início ----------
  const simulador = SIMULADO ? criarSimulador() : null;
  if (simulador) simulador.semear();

  setInterval(() => { renderizarStatus(); renderizarDestaque(); }, 1000);
  renderizarTudo();
  carregarHistoricoDoEsp32().finally(consultar);
})();
</script>
</body>
</html>
)rawliteral";
