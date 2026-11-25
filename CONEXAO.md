# Como conectar o ESP32 ao site (guia para a apresentação)

Este guia não exige saber programar. Ele explica como fazer o site mostrar os
dados do ESP32 e o que fazer se algo der errado na hora.

## Resumo em 30 segundos

1. O **site fica dentro do ESP32**. Não há servidor nem nada para instalar no notebook.
2. O ESP32 e o notebook precisam estar no **mesmo Wi-Fi**. Use o **roteador do celular**.
3. O display mostra o **IP** do ESP32 (ex.: `192.168.43.120`). Abra o arquivo
   **`ABRIR-AQUI.html`** no notebook, digite o IP e clique em **Abrir site do ESP32**.
4. Se não funcionar, use o **site simulado** (botão 2 do `ABRIR-AQUI.html`). Ele
   funciona sem o ESP32 e sem internet.

---

## O que levar

- O ESP32 montado na protoboard.
- Um **cabo USB de dados** (não um cabo só de carga) ou um carregador de celular.
- Um **celular com dados móveis**, que vai funcionar como roteador.
- O **notebook** com a pasta do projeto (veja "Preparar o notebook").

## Hoje, antes da apresentação

### 1. Descobrir o Wi-Fi que está gravado no ESP32

O ESP32 só entra no Wi-Fi cujo **nome e senha foram gravados nele**. Provavelmente
foi o Wi-Fi da casa de quem gravou.

Quem gravou tem esses dados no arquivo `BitcoinTracker/segredos.h`, nestas linhas:

```
const char* WIFI_NOME  = "nome da rede";
const char* WIFI_SENHA = "senha";
```

### 2. Configurar o roteador do celular com o MESMO nome e senha

É o jeito mais fácil de fazer o ESP32 conectar na faculdade **sem regravar nada**.
Nas configurações do celular, em *Roteador Wi-Fi* / *Ponto de acesso*:

- **Nome da rede:** exatamente igual ao `WIFI_NOME`, com as mesmas letras
  maiúsculas e minúsculas e os mesmos espaços.
- **Senha:** exatamente igual ao `WIFI_SENHA`.
- **Banda 2,4 GHz.** O ESP32 não enxerga redes de 5 GHz.
  - Android: em *Banda do ponto de acesso*, escolha **2,4 GHz**.
  - iPhone: ligue **Maximizar compatibilidade**.

Se não der para usar o mesmo nome e senha, o ESP32 precisa ser regravado (veja o
"Plano C" no final).

### 3. Preparar o notebook

1. Copie a pasta do projeto para o notebook, por pendrive ou pelo arquivo `.zip`.
2. Abra o arquivo **`ABRIR-AQUI.html`** com dois cliques. Ele abre no navegador.
3. Clique em **Abrir site simulado** e confira se aparecem o gráfico e o texto
   **"Modo simulado (sem ESP32)"** no canto superior direito.

Se isso funcionou, o plano B já está garantido. O notebook não precisa de mais nada.

---

## No dia: passo a passo

1. **Ligue o roteador do celular**, com os **dados móveis ligados**: o ESP32 busca o
   preço do bitcoin na internet.
2. **Conecte o notebook nesse mesmo Wi-Fi do celular.** Não use o Wi-Fi da faculdade.
3. **Ligue o ESP32** no USB do notebook ou num carregador.
4. **Olhe o display:**
   - primeiro aparece `BITCOIN TRACKER` / `Conectando Wi-Fi...`;
   - depois, na linha de baixo, `IP 192.168.xx.xx`. **Anote esse número**: ele
     aparece só por 15 segundos;
   - em seguida aparece o preço do bitcoin.
5. No notebook, abra o **`ABRIR-AQUI.html`**, digite o IP no campo 1 e clique em
   **Abrir site do ESP32**.
6. O site deve mostrar **"Conectado ao ESP32"**, com uma bolinha verde, no canto
   superior direito.

**Dica:** ligue o ESP32 **uns 20 a 30 minutos antes** de apresentar. Ele guarda
o histórico de preços na memória, então o gráfico já abre com vários pontos.
Se ele for desligado, o histórico começa do zero.

### Durante a apresentação dá para mostrar

- **O botão da protoboard:** força uma consulta nova. O LED acende enquanto o
  ESP32 busca o preço. Apertando de novo em menos de 15 segundos, o display
  mostra "Aguarde Xs" (proteção contra excesso de consultas).
- **O display e o site mostrando o mesmo preço.**
- **No site:** o gráfico, a máxima e a mínima do período, e as setas ▲▼ de
  subiu/desceu.

---

## Perdi o IP. Como descubro?

- Aperte o botão **EN** (ou **RST**) da placa do ESP32. Ela reinicia e mostra o IP
  de novo.
- Tente abrir **`bitcointracker.local`** no campo do `ABRIR-AQUI.html`. Funciona na
  maioria dos notebooks com Windows.
- No **Android**, a tela do roteador do celular costuma listar os aparelhos
  conectados. O ESP32 aparece com um nome parecido com `esp32-...`.

O IP pode **mudar** quando o ESP32 reinicia. Se o site parar de responder depois
de um reinício, confira o IP no display de novo.

---

## Se der errado

| O que acontece | O que fazer |
|---|---|
| Display fica em **"sem WiFi"** / `! Sem Wi-Fi` | O nome ou a senha do roteador do celular não estão **idênticos** aos gravados, ou o roteador está em 5 GHz. Confira maiúsculas e espaços e ligue a compatibilidade / 2,4 GHz. |
| Display mostra o preço, mas o site **não abre** no notebook | O notebook não está no Wi-Fi do celular (às vezes ele volta sozinho para o Wi-Fi da faculdade), o IP foi digitado errado, ou há uma VPN ligada no notebook. |
| Site abre, mas aparece **"Sem resposta do ESP32"** | O ESP32 desligou ou reiniciou e o IP pode ter mudado. Confira o display. |
| Display mostra `! Limite da API (429)` | O site do preço (CoinGecko) bloqueou por excesso de consultas. Espere 1 ou 2 minutos; o último preço continua na tela. |
| Display mostra `! Sem resposta da API` | O celular está sem dados móveis ou sem sinal. |
| Display não acende | Cabo USB só de carga, ou mau contato nos fios do display. |
| **Nada funciona** | Use o **plano B**: no `ABRIR-AQUI.html`, clique em **Abrir site simulado**. Diga na apresentação que é uma simulação: o próprio site mostra "Modo simulado". |

---

## Plano C: regravar o ESP32 com o Wi-Fi do celular

Só é preciso se **não der** para deixar o roteador do celular com o mesmo nome e
senha gravados. **Prepare hoje**, porque precisa de internet e baixa cerca de 1 GB.

1. Instale o **Arduino IDE 2** (arduino.cc).
2. Em *Boards Manager*, instale **esp32 by Espressif Systems**.
3. Em *Library Manager*, instale **Adafruit SSD1306**, **Adafruit GFX Library** e
   **ArduinoJson**.
4. Na pasta `BitcoinTracker`, copie `segredos.exemplo.h` para **`segredos.h`** e
   preencha o nome e a senha do roteador do celular.
5. Abra `BitcoinTracker/BitcoinTracker.ino`. Em *Tools*, escolha a placa
   **ESP32 Dev Module** e a porta COM que aparece quando o ESP32 está no USB.
6. Clique em **Upload** (a seta →). Se travar em `Connecting....`, segure o botão
   **BOOT** da placa até começar a gravar.

O firmware do repositório já está com a tela na posição normal
(`GIRAR_TELA_180 = false`), igual à montagem atual.
