# Como conectar o ESP32 ao site

Procedimento para o site exibir os dados coletados pelo ESP32.

1. O site é servido pelo próprio ESP32. Não há servidor nem programa para
   instalar no notebook.
2. O ESP32 e o notebook precisam estar na mesma rede Wi-Fi. Use o roteador do
   celular.
3. O display mostra o IP do ESP32 (por exemplo, `192.168.43.120`). No notebook,
   abra o arquivo `ABRIR-AQUI.html`, digite o IP e clique em *Abrir site do ESP32*.
4. Se a conexão não funcionar, use o site simulado (botão 2 do `ABRIR-AQUI.html`),
   que funciona sem o ESP32 e sem internet.

## Rede gravada no ESP32

O ESP32 só se conecta à rede cujo nome e senha foram gravados no firmware. Esses
dados ficam no arquivo `BitcoinTracker/segredos.h`, nestas linhas:

```
const char* WIFI_NOME  = "nome da rede";
const char* WIFI_SENHA = "senha";
```

## Configuração do roteador do celular

A forma mais simples de conectar o ESP32 sem regravá-lo é configurar o roteador
do celular (*Roteador Wi-Fi* ou *Ponto de acesso*) com a mesma rede gravada:

- Nome da rede: idêntico ao `WIFI_NOME`, inclusive maiúsculas, minúsculas e
  espaços.
- Senha: idêntica ao `WIFI_SENHA`.
- Banda de 2,4 GHz, porque o ESP32 não reconhece redes de 5 GHz:
  - no Android, selecione 2,4 GHz em *Banda do ponto de acesso*;
  - no iPhone, ative *Maximizar compatibilidade*.

Se não for possível usar o mesmo nome e a mesma senha, o ESP32 precisa ser
regravado (veja "Regravação do ESP32").

## Passo a passo

1. Ligue o roteador do celular com os dados móveis ativos. O ESP32 consulta o
   preço do bitcoin pela internet.
2. Conecte o notebook à mesma rede do celular.
3. Ligue o ESP32 na porta USB do notebook ou em um carregador.
4. Acompanhe o display:
   - primeiro aparece `BITCOIN TRACKER` / `Conectando Wi-Fi...`;
   - em seguida, na última linha, `IP 192.168.xx.xx`. Anote esse endereço, que
     fica visível por apenas 15 segundos;
   - depois aparece o preço do bitcoin.
5. No notebook, abra o `ABRIR-AQUI.html`, digite o IP no campo 1 e clique em
   *Abrir site do ESP32*.
6. O site deve exibir "Conectado ao ESP32", com um indicador verde, no canto
   superior direito.

### Funcionamento esperado

- O botão da protoboard força uma nova consulta, e o LED fica aceso enquanto ela
  acontece. Um novo acionamento em menos de 15 segundos mostra "Aguarde Xs" no
  display, proteção contra o excesso de consultas.
- O display e o site exibem o mesmo preço.
- O site mostra o gráfico, a máxima e a mínima do período e as setas ▲▼ que
  indicam alta ou queda.

## Como encontrar o IP

- Pressione o botão EN (ou RST) da placa. O ESP32 reinicia e exibe o IP novamente.
- Digite `bitcointracker.local` no campo do `ABRIR-AQUI.html`. Esse endereço
  funciona na maioria dos notebooks com Windows.
- No Android, a tela do ponto de acesso costuma listar os aparelhos conectados;
  o ESP32 aparece com um nome semelhante a `esp32-...`.

O IP pode mudar a cada reinício do ESP32. Se o site deixar de responder depois
de um reinício, confira o endereço no display.

## Solução de problemas

| Sintoma | Causa e solução |
|---|---|
| Display exibe "sem WiFi" ou `! Sem Wi-Fi` | O nome ou a senha do roteador não estão idênticos aos gravados, ou a rede está em 5 GHz. Confira maiúsculas e espaços e ative o modo de compatibilidade / 2,4 GHz. |
| Display mostra o preço, mas o site não abre no notebook | O notebook não está na rede do celular (ele pode trocar de rede sozinho), o IP foi digitado incorretamente ou há uma VPN ativa no notebook. |
| O site abre, mas exibe "Sem resposta do ESP32" | O ESP32 desligou ou reiniciou, e o IP pode ter mudado. Confira o display. |
| Display mostra `! Limite da API (429)` | A CoinGecko bloqueou temporariamente por excesso de consultas. Aguarde 1 a 2 minutos; o último preço continua na tela. |
| Display mostra `! Sem resposta da API` | O celular está sem dados móveis ou sem sinal. |
| Display não acende | Cabo USB apenas de carga ou mau contato nos fios do display. |

## Regravação do ESP32

Necessária apenas se o roteador do celular não puder usar o mesmo nome e a
mesma senha gravados. A instalação exige conexão com a internet.

1. Instale o Arduino IDE 2 (arduino.cc).
2. Em *Boards Manager*, instale o pacote esp32 by Espressif Systems.
3. Em *Library Manager*, instale Adafruit SSD1306, Adafruit GFX Library e
   ArduinoJson.
4. Na pasta `BitcoinTracker`, copie `segredos.exemplo.h` para `segredos.h` e
   preencha o nome e a senha do roteador do celular.
5. Abra `BitcoinTracker/BitcoinTracker.ino`. Em *Tools*, selecione a placa
   *ESP32 Dev Module* e a porta COM que aparece com o ESP32 conectado ao USB.
6. Clique em *Upload*. Se o processo parar em `Connecting....`, mantenha
   pressionado o botão BOOT da placa até a gravação começar.
