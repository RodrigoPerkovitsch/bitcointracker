/*
 * Bitcoin Tracker — ESP32 + OLED SSD1306 (0.96", I2C)
 *
 * - Consulta preço e variação de 24 h na CoinGecko a cada 60 s
 * - Mostra no OLED: preço, variação desde o preço anterior, variação 24 h e status
 * - Botão força uma consulta (respeitando um intervalo mínimo, por causa do limite da API)
 * - LED aceso enquanto a consulta está em andamento
 * - Guarda as últimas 24 h de preços na memória (opção B do histórico)
 * - Servidor web:
 *     "/"          página com gráfico
 *     "/dados"     preço atual em JSON
 *     "/historico" preços guardados em JSON
 *
 * A página (pagina.h) é gerada a partir de web/index.html — veja o README.
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>   // versão 7
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if __has_include("segredos.h")
#include "segredos.h"
#else
#error "Falta o arquivo segredos.h. Copie segredos.exemplo.h para segredos.h e preencha o Wi-Fi."
#endif

#include "pagina.h"

// ---------------- Hardware ----------------
const int PINO_BOTAO = 4;    // chave táctil entre o GPIO4 e o GND (usa o pull-up interno)
const int PINO_LED   = 16;   // LED + resistor de 220 Ω até o GND
// O código original usava o GPIO15. Ele funciona, mas é um pino de boot do ESP32:
// com algo ligado nele a placa às vezes não inicia. O 16 não tem esse problema.

const int LARGURA_TELA = 128;
const int ALTURA_TELA  = 64;
const uint8_t ENDERECO_OLED = 0x3C;  // se a tela ficar preta, teste 0x3D

// Na montagem real o OLED ficou na posição normal, então a imagem não precisa
// girar. Use true só se o OLED ficar de cabeça para baixo — por exemplo no
// desenho do Wokwi (wokwi/diagram.json), onde a tela fica para fora da borda.
const bool GIRAR_TELA_180 = false;

// ---------------- Tempos ----------------
const unsigned long INTERVALO_BUSCA_MS   = 60UL * 1000;  // consulta automática
const unsigned long INTERVALO_MINIMO_MS  = 15UL * 1000;  // o botão não força antes disso (limite da API gratuita)
const unsigned long TEMPO_LIMITE_WIFI_MS = 20UL * 1000;  // espera no boot antes de seguir sem rede
const unsigned long REPETIR_WIFI_MS      = 30UL * 1000;  // nova tentativa de conexão se cair
const unsigned long DEBOUNCE_MS          = 50;
const unsigned long REDESENHO_MS         = 1000;          // para o "atualizado há Xs" andar

// Durante a consulta o servidor web fica parado. Tempos curtos aqui reduzem o
// período em que a página fica sem resposta. O segredos.h pode aumentar estes
// valores: o Wokwi simula o HTTPS bem mais devagar que a placa real.
#ifndef TEMPO_LIMITE_CONEXAO_MS
#define TEMPO_LIMITE_CONEXAO_MS 4000
#endif
#ifndef TEMPO_LIMITE_RESPOSTA_MS
#define TEMPO_LIMITE_RESPOSTA_MS 4000
#endif
#ifndef TEMPO_LIMITE_TLS_S
#define TEMPO_LIMITE_TLS_S 5
#endif

// 1440 pontos = 24 h com um preço novo por minuto. Cada ponto ocupa 8 bytes,
// então o histórico inteiro usa ~11 KB de RAM.
const int CAPACIDADE_HISTORICO = 1440;

const char* NOME_MDNS = "bitcointracker";  // http://bitcointracker.local

// include_last_updated_at: a CoinGecko gratuita só renova o preço mais ou menos
// a cada minuto. Pelo horário da atualização dá para saber se o preço é novo
// ou se é o mesmo da consulta anterior.
const char* URL_COINGECKO =
  "https://api.coingecko.com/api/v3/simple/price"
  "?ids=bitcoin&vs_currencies=usd&include_24hr_change=true&include_last_updated_at=true";

Adafruit_SSD1306 tela(LARGURA_TELA, ALTURA_TELA, &Wire, -1);
WebServer servidor(80);
bool telaOk = false;

// ---------------- Estado ----------------
struct Leitura {
  bool valida = false;
  double preco = 0;
  double precoAnterior = 0;
  bool temAnterior = false;
  double variacao24h = 0;
  bool temVariacao24h = false;
  uint32_t atualizadoEm = 0;  // horário Unix (s) em que a CoinGecko atualizou o preço
  unsigned long momento = 0;  // millis() em que este preço chegou ao ESP32
};

Leitura leitura;
String ultimoErro = "";                 // vazio = a última consulta deu certo
unsigned long momentoUltimaTentativa = 0;
unsigned long momentoUltimaConsultaOk = 0;
bool jaTentou = false;
bool teveConsultaOk = false;
bool buscando = false;

// Preço em centavos (uint32) em vez de float: float perde os centavos a partir de ~US$ 100 mil.
struct Ponto {
  uint32_t atualizadoEm;
  uint32_t centavos;
};
Ponto historico[CAPACIDADE_HISTORICO];
int inicioHistorico = 0;
int totalHistorico = 0;

// Guardar início + duração (e não "até quando") mantém a conta certa mesmo
// quando o millis() dá a volta, depois de ~49 dias ligado.
String avisoTemporario = "";
unsigned long avisoInicio = 0;
unsigned long avisoDuracao = 0;

bool wifiConectado = false;
bool mdnsIniciado = false;
unsigned long ultimaTentativaWifi = 0;
unsigned long ultimoRedesenho = 0;

// ---------------- Histórico ----------------
void guardarNoHistorico(uint32_t atualizadoEm, double preco) {
  int posicao = (inicioHistorico + totalHistorico) % CAPACIDADE_HISTORICO;
  historico[posicao] = { atualizadoEm, (uint32_t)llround(preco * 100.0) };
  if (totalHistorico < CAPACIDADE_HISTORICO) {
    totalHistorico++;
  } else {
    // cheio: o novo ponto ocupou o lugar do mais antigo
    inicioHistorico = (inicioHistorico + 1) % CAPACIDADE_HISTORICO;
  }
}

// ---------------- Formatação ----------------
String formatarMilhar(double valor) {
  String s = String((long)lround(valor));
  String r = "";
  int n = s.length();
  for (int i = 0; i < n; i++) {
    r += s[i];
    int restantes = n - 1 - i;
    if (restantes > 0 && restantes % 3 == 0) r += ',';
  }
  return r;
}

String formatarComSinal(double valor, int casas) {
  return (valor >= 0 ? "+" : "") + String(valor, casas);
}

String formatarIdade(unsigned long segundos) {
  if (segundos < 60) return String(segundos) + "s";
  return String(segundos / 60) + "min";
}

// ---------------- Tela OLED ----------------
// A fonte padrão da Adafruit não tem acentos nem setas; por isso o texto da
// tela vai sem acento e as setas são desenhadas como triângulos.
void desenharSeta(int x, int y, double delta) {
  if (delta > 0)      tela.fillTriangle(x, y + 6, x + 6, y + 6, x + 3, y, SSD1306_WHITE);
  else if (delta < 0) tela.fillTriangle(x, y, x + 6, y, x + 3, y + 6, SSD1306_WHITE);
  else                tela.fillRect(x, y + 2, 7, 3, SSD1306_WHITE);
}

bool avisoAtivo() {
  return avisoDuracao > 0 && millis() - avisoInicio < avisoDuracao;
}

String textoRodape() {
  if (buscando) return "Buscando...";
  if (avisoAtivo()) return avisoTemporario;
  if (ultimoErro.length() > 0) return "! " + ultimoErro;
  if (teveConsultaOk) return "Atualizado ha " + formatarIdade((millis() - momentoUltimaConsultaOk) / 1000);
  return "Aguardando...";
}

void desenharTela() {
  if (!telaOk) return;

  tela.clearDisplay();
  tela.setTextColor(SSD1306_WHITE);

  tela.setTextSize(1);
  tela.setCursor(0, 0);
  tela.print("BTC/USD");
  const char* rede = wifiConectado ? "WiFi ok" : "sem WiFi";
  tela.setCursor(LARGURA_TELA - strlen(rede) * 6, 0);
  tela.print(rede);
  tela.drawLine(0, 10, LARGURA_TELA - 1, 10, SSD1306_WHITE);

  if (!leitura.valida) {
    tela.setTextSize(2);
    tela.setCursor(0, 18);
    tela.print("--");
  } else {
    // Preço sem centavos e com separador de milhar: "$105,432" ocupa 96 px de 128.
    tela.setTextSize(2);
    tela.setCursor(0, 16);
    tela.print("$");
    tela.print(formatarMilhar(leitura.preco));

    tela.setTextSize(1);
    if (leitura.temAnterior) {
      double delta = leitura.preco - leitura.precoAnterior;
      desenharSeta(0, 36, delta);
      tela.setCursor(10, 36);
      tela.print(formatarComSinal(delta, 2));
      tela.print(" ult.");
    } else {
      tela.setCursor(0, 36);
      tela.print("1a leitura");
    }

    tela.setCursor(0, 46);
    tela.print("24h ");
    if (leitura.temVariacao24h) {
      tela.print(formatarComSinal(leitura.variacao24h, 2));
      tela.print("%");
    } else {
      tela.print("--");
    }
  }

  tela.setCursor(0, 56);
  tela.print(textoRodape());
  tela.display();
}

void mostrarMensagem(const char* linha1, const String& linha2) {
  if (!telaOk) return;
  tela.clearDisplay();
  tela.setTextSize(1);
  tela.setTextColor(SSD1306_WHITE);
  tela.setCursor(0, 20);
  tela.println(linha1);
  tela.setCursor(0, 36);
  tela.println(linha2);
  tela.display();
}

void mostrarAviso(const String& texto, unsigned long duracaoMs) {
  avisoTemporario = texto;
  avisoInicio = millis();
  avisoDuracao = duracaoMs;
  desenharTela();
}

// ---------------- CoinGecko ----------------
void registrarPreco(double preco, uint32_t atualizadoEm, JsonVariantConst variacao24h) {
  if (leitura.valida) {
    leitura.precoAnterior = leitura.preco;
    leitura.temAnterior = true;
  }
  leitura.preco = preco;
  leitura.atualizadoEm = atualizadoEm;
  leitura.momento = millis();
  // Sem o campo, mostrar "--" em vez de um 0% que pareceria um valor real.
  leitura.temVariacao24h = variacao24h.is<double>();
  leitura.variacao24h = variacao24h | 0.0;
  leitura.valida = true;
  guardarNoHistorico(atualizadoEm, preco);
}

void buscarPreco() {
  jaTentou = true;
  momentoUltimaTentativa = millis();

  if (!wifiConectado) {
    ultimoErro = "Sem Wi-Fi";
    return;
  }

  buscando = true;
  digitalWrite(PINO_LED, HIGH);
  desenharTela();  // mostra "Buscando..." durante a espera (a requisição leva 1–2 s)

  WiFiClientSecure cliente;
  // Sem validar o certificado: aceitável num protótipo. Em produção o certo
  // é fixar o certificado raiz com cliente.setCACert(...).
  cliente.setInsecure();
  cliente.setHandshakeTimeout(TEMPO_LIMITE_TLS_S);

  HTTPClient http;
  http.setConnectTimeout(TEMPO_LIMITE_CONEXAO_MS);
  http.setTimeout(TEMPO_LIMITE_RESPOSTA_MS);
  String erro = "";
  bool precoNovo = false;

  if (!http.begin(cliente, URL_COINGECKO)) {
    erro = "Falha ao conectar";
  } else {
    if (strlen(COINGECKO_CHAVE) > 0) {
      http.addHeader("x-cg-demo-api-key", COINGECKO_CHAVE);
    }
    int codigo = http.GET();

    // Só 200 conta como sucesso: 429/404 também são "> 0" e deixariam o preço
    // antigo na tela como se fosse atual.
    if (codigo == HTTP_CODE_OK) {
      JsonDocument doc;
      DeserializationError falha = deserializeJson(doc, http.getString());
      JsonVariantConst btc = doc["bitcoin"];
      if (falha || !btc["usd"].is<double>() || !btc["last_updated_at"].is<uint32_t>()) {
        erro = "Resposta invalida";
      } else {
        teveConsultaOk = true;
        momentoUltimaConsultaOk = millis();
        uint32_t atualizadoEm = btc["last_updated_at"];
        // Mesmo horário de atualização = mesmo preço da consulta anterior.
        // Não conta como leitura nova, senão a tela mostraria "sem alteração"
        // e o gráfico ficaria em degraus.
        if (!leitura.valida || atualizadoEm != leitura.atualizadoEm) {
          registrarPreco(btc["usd"].as<double>(), atualizadoEm, btc["usd_24h_change"]);
          precoNovo = true;
        }
      }
    } else if (codigo == 429) {
      erro = "Limite da API (429)";
    } else if (codigo > 0) {
      erro = "Erro HTTP " + String(codigo);
    } else {
      erro = "Sem resposta da API";
      // No OLED não cabe o detalhe; no serial ele diz se foi tempo esgotado,
      // conexão recusada, falha no HTTPS etc.
      Serial.printf("Detalhe da falha (%d): %s\n", codigo, HTTPClient::errorToString(codigo).c_str());
    }
    http.end();
  }

  ultimoErro = erro;
  buscando = false;
  digitalWrite(PINO_LED, LOW);

  if (erro.length() > 0) {
    Serial.println("Erro na consulta: " + erro);
  } else if (precoNovo) {
    Serial.printf("Preco novo: %.2f USD | 24h: %+.2f%% | %d pontos no historico\n",
                  leitura.preco, leitura.variacao24h, totalHistorico);
  } else {
    Serial.println("Consulta ok, preco ainda nao mudou na CoinGecko.");
  }
  desenharTela();
}

// ---------------- Servidor web ----------------
void cabecalhosJson() {
  servidor.sendHeader("Cache-Control", "no-store");
  // Permite abrir a página a partir do PC (fora da ESP32) durante o desenvolvimento.
  servidor.sendHeader("Access-Control-Allow-Origin", "*");
}

void rotaPagina() {
  servidor.send_P(200, "text/html; charset=utf-8", PAGINA_HTML);
}

void rotaDados() {
  // A rota só devolve o que já está guardado. Chamar a CoinGecko aqui, a cada
  // acesso, estourava o limite da API com várias abas abertas.
  JsonDocument doc;
  doc["valida"] = leitura.valida;
  doc["intervaloSegundos"] = INTERVALO_BUSCA_MS / 1000;
  if (leitura.valida) {
    doc["preco"] = leitura.preco;
    doc["atualizadoEm"] = leitura.atualizadoEm;
    doc["idadeSegundos"] = (millis() - leitura.momento) / 1000;
    if (leitura.temAnterior) doc["precoAnterior"] = leitura.precoAnterior;
    else doc["precoAnterior"] = nullptr;
    if (leitura.temVariacao24h) doc["variacao24h"] = leitura.variacao24h;
    else doc["variacao24h"] = nullptr;
  }
  if (teveConsultaOk) doc["idadeConsultaSegundos"] = (millis() - momentoUltimaConsultaOk) / 1000;
  else doc["idadeConsultaSegundos"] = nullptr;
  if (ultimoErro.length() > 0) doc["erro"] = ultimoErro;
  else doc["erro"] = nullptr;

  String corpo;
  serializeJson(doc, corpo);
  cabecalhosJson();
  servidor.send(200, "application/json", corpo);
}

void rotaHistorico() {
  // Com o histórico cheio o JSON passa de 60 KB. Montar tudo numa String só
  // poderia faltar memória contínua, então a resposta vai em pedaços.
  cabecalhosJson();
  servidor.setContentLength(CONTENT_LENGTH_UNKNOWN);
  servidor.send(200, "application/json", "");

  String bloco = "{\"pontos\":[";
  for (int i = 0; i < totalHistorico; i++) {
    const Ponto& p = historico[(inicioHistorico + i) % CAPACIDADE_HISTORICO];
    if (i > 0) bloco += ',';
    bloco += "{\"atualizadoEm\":";
    bloco += String(p.atualizadoEm);
    bloco += ",\"preco\":";
    bloco += String(p.centavos / 100);
    bloco += '.';
    uint32_t c = p.centavos % 100;
    if (c < 10) bloco += '0';
    bloco += String(c);
    bloco += '}';
    if (bloco.length() > 1024) {
      servidor.sendContent(bloco);
      bloco = "";
    }
  }
  bloco += "]}";
  servidor.sendContent(bloco);
  servidor.sendContent("");  // encerra a resposta em pedaços
}

// ---------------- Wi-Fi ----------------
void aoConectarWifi() {
  wifiConectado = true;
  String ip = WiFi.localIP().toString();
  Serial.println("\nWi-Fi conectado. Abra no navegador: http://" + ip);
  Serial.printf("Ou tente: http://%s.local\n", NOME_MDNS);
  if (!mdnsIniciado && MDNS.begin(NOME_MDNS)) {
    MDNS.addService("http", "tcp", 80);
    mdnsIniciado = true;
  }
  mostrarAviso("IP " + ip, 15000);
  jaTentou = false;  // consulta já, sem esperar o próximo ciclo de 60 s
}

// WIFI_CANAL é opcional no segredos.h. Informar o canal pula a varredura de
// todos os canais; no Wokwi isso é necessário, senão a conexão demora demais.
void iniciarConexaoWifi() {
#ifdef WIFI_CANAL
  WiFi.begin(WIFI_NOME, WIFI_SENHA, WIFI_CANAL);
#else
  WiFi.begin(WIFI_NOME, WIFI_SENHA);
#endif
}

void conectarWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  iniciarConexaoWifi();
  ultimaTentativaWifi = millis();

  Serial.print("Conectando ao Wi-Fi");
  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < TEMPO_LIMITE_WIFI_MS) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    aoConectarWifi();
  } else {
    // Não trava: a tela e o botão continuam respondendo e a conexão é tentada de novo no loop.
    Serial.println("\nNao conectou. Tentando de novo em segundo plano.");
    ultimoErro = "Sem Wi-Fi";
  }
}

void verificarWifi() {
  bool agora = WiFi.status() == WL_CONNECTED;
  if (agora && !wifiConectado) {
    aoConectarWifi();
  } else if (!agora && wifiConectado) {
    wifiConectado = false;
    Serial.println("Wi-Fi caiu.");
    ultimaTentativaWifi = millis();
  } else if (!agora && millis() - ultimaTentativaWifi >= REPETIR_WIFI_MS) {
    ultimaTentativaWifi = millis();
    WiFi.disconnect();
    iniciarConexaoWifi();
  }
}

// ---------------- Botão ----------------
// Debounce por tempo em vez de delay(): o delay travava o servidor web.
int estadoEstavelBotao = HIGH;
int ultimaLeituraBotao = HIGH;
unsigned long momentoMudancaBotao = 0;

bool botaoFoiPressionado() {
  int atual = digitalRead(PINO_BOTAO);
  if (atual != ultimaLeituraBotao) {
    ultimaLeituraBotao = atual;
    momentoMudancaBotao = millis();
  }
  if (millis() - momentoMudancaBotao > DEBOUNCE_MS && atual != estadoEstavelBotao) {
    estadoEstavelBotao = atual;
    return estadoEstavelBotao == LOW;  // LOW = pressionado (pull-up)
  }
  return false;
}

// ---------------- Programa principal ----------------
void setup() {
  Serial.begin(115200);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_LED, OUTPUT);
  digitalWrite(PINO_LED, LOW);

  // Se o OLED falhar o programa segue: a página web continua útil e o erro
  // aparece no monitor serial. O código original travava aqui.
  telaOk = tela.begin(SSD1306_SWITCHCAPVCC, ENDERECO_OLED);
  if (telaOk && GIRAR_TELA_180) tela.setRotation(2);
  if (!telaOk) {
    Serial.println("OLED nao encontrado. Confira SDA=21, SCL=22 e o endereco (0x3C/0x3D).");
  }
  mostrarMensagem("BITCOIN TRACKER", "Conectando Wi-Fi...");

  conectarWifi();

  servidor.on("/", rotaPagina);
  servidor.on("/dados", rotaDados);
  servidor.on("/historico", rotaHistorico);
  servidor.onNotFound([]() { servidor.send(404, "text/plain", "Nao encontrado"); });
  servidor.begin();

  desenharTela();
}

void loop() {
  servidor.handleClient();
  verificarWifi();

  if (botaoFoiPressionado()) {
    unsigned long desde = millis() - momentoUltimaTentativa;
    if (jaTentou && desde < INTERVALO_MINIMO_MS) {
      unsigned long falta = (INTERVALO_MINIMO_MS - desde + 999) / 1000;
      mostrarAviso("Aguarde " + String(falta) + "s", 2000);
    } else {
      buscarPreco();
    }
  }

  if (!jaTentou || millis() - momentoUltimaTentativa >= INTERVALO_BUSCA_MS) {
    buscarPreco();
  }

  if (millis() - ultimoRedesenho >= REDESENHO_MS) {
    ultimoRedesenho = millis();
    desenharTela();
  }
}
