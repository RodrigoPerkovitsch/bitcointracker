# Bitcoin Tracker — ESP32 + OLED

O ESP32 consulta o preço do bitcoin na CoinGecko a cada 60 s e mostra o resultado
de duas formas:

- **no display OLED**: preço, variação desde o preço anterior, variação de 24 h
  e status (Wi-Fi, erro, "atualizado há Xs");
- **numa página web** servida pelo próprio ESP32: preço, gráfico de histórico,
  máxima, mínima, variação no período e tabela com as últimas leituras.

O ESP32 guarda na memória as últimas 24 h de preços, então a página já abre com
o gráfico preenchido. O botão força uma consulta. O LED fica aceso enquanto a
consulta acontece.

## Estrutura

```
bitcointracker/
├── BitcoinTracker/          pasta do sketch (abrir no Arduino IDE)
│   ├── BitcoinTracker.ino   firmware
│   ├── pagina.h             página web embutida (GERADO, não editar)
│   ├── segredos.h           Wi-Fi e chave da API (fora do Git)
│   └── segredos.exemplo.h   modelo do segredos.h
├── web/
│   └── index.html           fonte da página (editar aqui)
├── wokwi/                   arquivos para simular o circuito no Wokwi
├── ferramentas/
│   └── servidor-simulado.js imita as rotas do ESP32 para testar a página
├── .githooks/pre-commit     barra commit com pagina.h desatualizado
└── gerar-pagina.ps1         converte web/index.html em pagina.h
```

## Montagem

| Componente | Pino do componente | Liga em |
|---|---|---|
| OLED SSD1306 | VCC | 3V3 do ESP32 |
| | GND | GND |
| | SDA | GPIO21 |
| | SCL | GPIO22 |
| Chave táctil | um terminal | GPIO4 |
| | terminal do lado oposto | GND |
| LED | perna longa (+) | GPIO16, passando pelo resistor de 220 Ω |
| | perna curta (−) | GND |

- O botão não precisa de resistor: o código liga o pull-up interno.
- Na chave táctil de 4 terminais, os pares do mesmo lado já vêm ligados entre si.
  Use dois terminais em diagonal para não errar.
- O código original usava o **GPIO15** para o LED. Ele é um pino de boot do ESP32,
  por isso foi trocado pelo 16. Para voltar ao 15, mude `PINO_LED` no `.ino`.
- Se a tela ficar preta, troque `ENDERECO_OLED` de `0x3C` para `0x3D`.

## Instalação (Arduino IDE 2)

1. **Placa:** em *Boards Manager*, instale **esp32 by Espressif Systems**. Depois
   escolha a placa *ESP32 Dev Module* (ou o modelo de vocês).
2. **Bibliotecas:** em *Library Manager*, instale:
   - **Adafruit SSD1306** (ela instala junto a Adafruit GFX)
   - **ArduinoJson**, versão **7.x**
3. Preencha `BitcoinTracker/segredos.h` com o nome e a senha do Wi-Fi.
4. Abra `BitcoinTracker/BitcoinTracker.ino`, compile e envie para a placa.
5. Abra o *Serial Monitor* em **115200**. Ele mostra o endereço da página
   (ex.: `http://192.168.0.42`), que também aparece no OLED por 15 s.

### Sobre o Wi-Fi

- O ESP32 só conecta em rede **2.4 GHz** com senha simples (WPA2-Personal).
- Rede da faculdade com login de aluno (eduroam etc.) não funciona. Muitas redes
  de campus também impedem que um aparelho acesse outro. **Para a apresentação,
  use o roteador do celular**: o ESP32 e o computador ou celular que abre a página
  precisam estar na mesma rede.
- `http://bitcointracker.local` também costuma funcionar no Windows, macOS e
  iPhone; no Android, nem sempre. O IP sempre funciona.

## Testando sem a placa (Wokwi)

O [Wokwi](https://wokwi.com) simula o ESP32 com o OLED, o botão e o LED no
navegador. A pasta `wokwi/` tem a mesma fiação da tabela acima.

1. Acesse <https://wokwi.com/projects/new/esp32>.
2. Na aba `sketch.ino`, apague o conteúdo e cole o de `BitcoinTracker/BitcoinTracker.ino`.
3. Na aba `diagram.json`, apague o conteúdo e cole o de `wokwi/diagram.json`.
4. Pela setinha ao lado das abas, escolha *New file…* e crie três arquivos:
   - `pagina.h`, com o conteúdo de `BitcoinTracker/pagina.h`;
   - `segredos.h`, com o conteúdo de `wokwi/segredos.h` (rede `Wokwi-GUEST`, sem senha);
   - `libraries.txt`, com o conteúdo de `wokwi/libraries.txt`.
5. Clique em ▶. A primeira compilação demora um pouco.

O que dá para testar:
- O OLED mostra "Conectando Wi-Fi...", depois o IP e em seguida o preço real.
- Clicar no botão força uma nova busca, e o LED acende enquanto ela acontece.
  Clicar de novo logo em seguida mostra "Aguarde Xs".
- O monitor serial, embaixo, mostra o preço e os erros.

Limites do simulador:
- Todos os usuários do Wokwi saem para a internet pelo mesmo endereço. Por isso
  a CoinGecko pode responder **429 (limite)** com frequência. Isso não é defeito
  do código, e é até uma boa forma de ver a mensagem de erro no OLED.
- Na versão gratuita do site não dá para abrir a página web do ESP32 simulado.
  Para ver a página, abra `web/index.html` direto no navegador (modo simulado).

## Alterando a página

1. Edite `web/index.html`.
2. Teste de um destes dois jeitos:
   - **rápido:** abra o arquivo direto no navegador (duplo clique). A página
     entra em **modo simulado**, com preços inventados gerados por ela mesma;
   - **completo:** rode o servidor simulado (precisa do [Node.js](https://nodejs.org))
     e abra <http://localhost:8765>. Ele imita as rotas `/dados` e `/historico`
     do ESP32, então testa a página fazendo requisições de verdade:
     ```powershell
     node ferramentas/servidor-simulado.js
     ```
     Com ele rodando, dá para abrir no navegador
     `http://localhost:8765/simular/reiniciar`, que imita a placa reiniciando,
     e `http://localhost:8765/simular/lento`, que liga e desliga respostas lentas
     (o ESP32 ocupado falando com a CoinGecko).
3. Gere o `pagina.h` de novo:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1
   ```
4. Compile e envie o firmware novamente.

Para não esquecer o passo 3, ative uma vez o gancho de commit. Ele recusa o
commit se o `pagina.h` estiver diferente do `index.html`:

```powershell
git config core.hooksPath .githooks
```

A página não usa biblioteca externa. O gráfico é desenhado em `<canvas>` para
caber no ESP32 e funcionar mesmo sem internet no aparelho que a abre.

## Rotas do ESP32

### `GET /dados`

Devolve o último valor guardado, sem consultar a CoinGecko a cada acesso:

```json
{
  "valida": true,
  "intervaloSegundos": 60,
  "preco": 97432.18,
  "atualizadoEm": 1795390800,
  "idadeSegundos": 42,
  "precoAnterior": 97410.05,
  "variacao24h": -1.23,
  "idadeConsultaSegundos": 12,
  "erro": null
}
```

- `atualizadoEm` é o horário Unix (em segundos) em que a **CoinGecko** atualizou
  o preço. A CoinGecko gratuita só renova o preço mais ou menos a cada minuto.
  Por isso o ESP32 só conta um preço como novo quando esse horário muda;
  consultar e receber o mesmo valor não vira "sem alteração" na tela. A página
  usa o mesmo campo para posicionar o ponto no gráfico e evitar pontos repetidos.
- `idadeSegundos` é o tempo desde que este preço chegou ao ESP32.
  `idadeConsultaSegundos` é o tempo desde a última consulta que deu certo, mesmo
  quando o preço não mudou. A página usa esse segundo campo para decidir se o
  valor está desatualizado.
- `variacao24h` vem `null` se a CoinGecko não mandar o campo, para a tela mostrar
  "—" em vez de um 0% falso.
- `erro` traz o texto da última falha (ex.: `"Limite da API (429)"`). O último
  preço continua sendo enviado, e a página o marca como desatualizado.
- Se `valida` for `false` (placa recém-ligada, sem nenhum preço ainda), os campos
  de preço não vêm, e a página mostra "—".

### `GET /historico`

Os preços guardados na memória do ESP32, do mais antigo para o mais novo:

```json
{
  "pontos": [
    { "atualizadoEm": 1795387200, "preco": 97001.10 },
    { "atualizadoEm": 1795387260, "preco": 97020.55 }
  ]
}
```

- São guardados até 1440 pontos, ou seja, 24 h com um preço novo por minuto.
  Isso ocupa cerca de 11 KB de RAM. Quando enche, o mais antigo sai.
- O histórico fica só na RAM: **se a placa reiniciar, ele começa do zero.**
  Gravar na memória flash evitaria isso, mas desgasta a flash e complica o código.
  Para uma demonstração não compensa.
- A página também guarda no navegador o que já viu (`localStorage`). Abrir a
  página de novo no mesmo aparelho junta as duas fontes, sem pontos repetidos.

### Comportamento da página quando o ESP32 não responde

Enquanto consulta a CoinGecko, o ESP32 fica até ~13 s sem atender a página.
Por isso uma falha isolada não gera alerta: a página tenta de novo em 3 s e só
mostra "Sem resposta do ESP32" depois de 3 falhas seguidas.

## Limites conhecidos

- O certificado HTTPS da CoinGecko não é validado (`setInsecure`). Isso é aceitável
  num protótipo; num produto, fixe o certificado raiz.
- Sem chave, a API gratuita da CoinGecko tem limite baixo de consultas. Por isso o
  intervalo é de 60 s e o botão espera 15 s entre buscas. Uma chave *Demo*
  gratuita pode ser colocada em `segredos.h`.
- Enquanto busca o preço (normalmente 1–2 s, no pior caso ~13 s), o ESP32 não
  responde à página. A página tolera isso; veja acima.
- Com a tela do celular bloqueada, o navegador pausa a página. Isso não afeta o
  gráfico, porque o histórico fica no ESP32.
