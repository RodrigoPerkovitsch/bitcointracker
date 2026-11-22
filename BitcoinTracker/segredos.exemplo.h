// Modelo do arquivo de credenciais.
// Copie para "segredos.h" (mesma pasta) e preencha. O segredos.h está no
// .gitignore para a senha do Wi-Fi não ir parar no repositório.
#pragma once

// A ESP32 só conecta em redes 2.4 GHz com senha simples (WPA2-Personal).
// Rede da faculdade com login e senha de aluno (eduroam etc.) não funciona:
// use o roteador do celular.
const char* WIFI_NOME  = "NOME_DA_REDE";
const char* WIFI_SENHA = "SENHA_DA_REDE";

// Opcional. Sem chave a CoinGecko funciona, mas com limite menor de consultas.
// Chave "Demo" gratuita: https://www.coingecko.com/en/api
const char* COINGECKO_CHAVE = "";
