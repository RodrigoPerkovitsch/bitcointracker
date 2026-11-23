// segredos.h para o simulador Wokwi. Pode ir para o Git: não tem senha real.
// "Wokwi-GUEST" é a rede simulada do Wokwi, aberta e com acesso à internet.
#pragma once

const char* WIFI_NOME  = "Wokwi-GUEST";
const char* WIFI_SENHA = "";

// A rede do Wokwi fica no canal 6. Sem isso o ESP32 simulado varre todos os
// canais e não conecta a tempo.
#define WIFI_CANAL 6

// O Wokwi simula o HTTPS bem mais devagar que a placa real; com os tempos da
// placa (4–5 s) a consulta à CoinGecko não termina a tempo.
#define TEMPO_LIMITE_CONEXAO_MS 20000
#define TEMPO_LIMITE_RESPOSTA_MS 20000
#define TEMPO_LIMITE_TLS_S 20

const char* COINGECKO_CHAVE = "";
