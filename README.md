# Bitcoin Tracker — ESP32 + OLED

O ESP32 consulta o preço do bitcoin na CoinGecko a cada 60 s e mostra o resultado
de duas formas:

- **no display OLED**: preço, variação desde a leitura anterior, variação de 24 h
  e status (Wi-Fi, erro, "atualizado há Xs");
- **numa página web** servida pelo próprio ESP32: preço, gráfico de histórico,
  máxima, mínima, variação no período e tabela com as últimas leituras.

O botão força uma atualização. O LED fica aceso enquanto a busca acontece.

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
2. Abra o arquivo direto no navegador (duplo clique). Sem o ESP32, a página
   entra em **modo simulado**, com preços inventados, para ver o visual.
3. Gere o `pagina.h` de novo:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1
   ```
4. Compile e envie o firmware novamente.

A página não usa biblioteca externa. O gráfico é desenhado em `<canvas>` para
caber no ESP32 e funcionar mesmo sem internet no aparelho que a abre.

## Rotas do ESP32

### `GET /dados`

Devolve o último valor guardado, sem consultar a CoinGecko a cada acesso:

```json
{
  "sessao": 3141592653,
  "leitura": 17,
  "valida": true,
  "intervaloSegundos": 60,
  "preco": 97432.18,
  "variacao24h": -1.23,
  "idadeSegundos": 12,
  "precoAnterior": 97410.05,
  "erro": null
}
```

- `sessao` muda a cada reinício da placa e `leitura` cresce a cada preço novo.
  A página usa o par para não registrar o mesmo ponto duas vezes.
- `idadeSegundos` existe porque o ESP32 não tem relógio. A página calcula o
  horário com o relógio do próprio aparelho.
- `erro` traz o texto da última falha (ex.: `"Limite da API (429)"`). O preço
  anterior continua sendo enviado, e a página o marca como desatualizado.

## Histórico: opção A (atual) e opção B (proposta)

**Hoje (opção A)** o histórico fica no navegador (`localStorage`). Ele começa
quando a página é aberta e cada aparelho tem o seu.

**Opção B (a combinar com quem cuida do firmware):** o ESP32 guardaria as
últimas leituras em memória e as devolveria numa rota nova. **A página já está
preparada:** se `/historico` existir, ela carrega esses pontos ao abrir. Formato
esperado:

```json
{
  "sessao": 3141592653,
  "pontos": [
    { "leitura": 1, "preco": 97001.10, "idadeSegundos": 960 },
    { "leitura": 2, "preco": 97020.55, "idadeSegundos": 900 }
  ]
}
```

`sessao` e `leitura` devem ser os mesmos de `/dados`.

## Limites conhecidos

- O certificado HTTPS da CoinGecko não é validado (`setInsecure`). Isso é aceitável
  num protótipo; num produto, fixe o certificado raiz.
- Sem chave, a API gratuita da CoinGecko tem limite baixo de consultas. Por isso o
  intervalo é de 60 s e o botão espera 15 s entre buscas. Uma chave *Demo*
  gratuita pode ser colocada em `segredos.h`.
- Enquanto busca o preço (1–2 s), o ESP32 não responde à página. A página tolera
  isso e tenta de novo na consulta seguinte.
