# Bitcoin Tracker (ESP32 + OLED)

Protótipo que acompanha a cotação do bitcoin com um ESP32. A placa consulta a
CoinGecko a cada 60 s e apresenta o resultado em dois lugares:

- no display OLED: preço, variação em relação ao preço anterior, variação de
  24 h e estado da conexão;
- em uma página web servida pelo próprio ESP32: preço, gráfico do histórico,
  máxima, mínima, variação no período e as últimas leituras.

O ESP32 mantém em memória as últimas 24 h de preços, de modo que a página já
abre com o gráfico preenchido. O botão força uma nova consulta, e o LED fica
aceso enquanto ela acontece.

Para a apresentação, consulte o [CONEXAO.md](CONEXAO.md) e use o arquivo
`ABRIR-AQUI.html`.

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
│   ├── servidor-simulado.js         imita as rotas do ESP32 para testar a página
│   ├── gerar-diagrama-protoboard.js gera wokwi/diagram.json (montagem na protoboard)
│   └── teste-montagem/              sketch que testa a fiação, sem Wi-Fi
├── MONTAGEM.md              passo a passo da montagem na protoboard
├── CONEXAO.md               como abrir o site do ESP32 na apresentação (e o plano B)
├── ABRIR-AQUI.html          página de entrada: site do ESP32 ou site simulado
├── .githooks/pre-commit     barra commit com pagina.h desatualizado
└── gerar-pagina.ps1         converte web/index.html em pagina.h
```

## Montagem

O passo a passo na protoboard, furo por furo, está no [MONTAGEM.md](MONTAGEM.md).
Resumo das ligações:

| Componente | Pino do componente | Liga em |
|---|---|---|
| OLED SSD1306 | VCC | 3V3 do ESP32 |
| | GND | GND |
| | SDA | GPIO21 |
| | SCL | GPIO22 |
| Chave táctil | um lado | GPIO4 |
| | outro lado | GND |
| LED | perna longa (+) | GPIO16, passando pelo resistor de 220 Ω |
| | perna curta (−) | GND |

Observações:

- O botão dispensa resistor externo, porque o firmware ativa o pull-up interno.
- A versão original usava o GPIO15 para o LED. Como esse pino participa do boot
  do ESP32, o LED passou para o GPIO16. Para voltar ao 15, altere `PINO_LED` no
  `.ino`.
- Se o display ficar apagado, troque `ENDERECO_OLED` de `0x3C` para `0x3D`.
- Na montagem real o OLED ficou na posição normal, por isso `GIRAR_TELA_180` está
  como `false`. No desenho do Wokwi o OLED aparece invertido; para simular lá, use
  `true`.

## Instalação (Arduino IDE 2)

1. Em *Boards Manager*, instale o pacote esp32 by Espressif Systems e selecione a
   placa *ESP32 Dev Module* (ou o modelo em uso).
2. Em *Library Manager*, instale:
   - Adafruit SSD1306 (instala junto a Adafruit GFX);
   - ArduinoJson, versão 7.x.
3. Preencha `BitcoinTracker/segredos.h` com o nome e a senha do Wi-Fi.
4. Abra `BitcoinTracker/BitcoinTracker.ino`, compile e grave na placa.
5. Abra o *Serial Monitor* em 115200. O endereço da página (por exemplo,
   `http://192.168.0.42`) aparece ali e também no OLED, por 15 s.

### Wi-Fi

- O ESP32 só se conecta a redes de 2,4 GHz com senha simples (WPA2-Personal).
- Redes com login individual, como a eduroam, não funcionam. Muitas redes de
  campus também bloqueiam a comunicação entre aparelhos. Por isso a apresentação
  usa o roteador do celular: o ESP32 e o aparelho que abre a página precisam
  estar na mesma rede.
- O endereço `http://bitcointracker.local` costuma funcionar no Windows, no macOS
  e no iPhone, mas nem sempre no Android. O IP funciona em todos.

## Simulação no Wokwi

O [Wokwi](https://wokwi.com) simula no navegador o ESP32 com o OLED, o botão e o
LED. O arquivo `wokwi/diagram.json` reproduz a montagem do
[MONTAGEM.md](MONTAGEM.md) e é gerado por
`node ferramentas/gerar-diagrama-protoboard.js`. Para mudar a posição de alguma
peça, altere o script e gere o arquivo de novo em vez de editar o JSON.

1. Acesse <https://wokwi.com/projects/new/esp32>.
2. Na aba `sketch.ino`, substitua o conteúdo pelo de `BitcoinTracker/BitcoinTracker.ino`.
3. Na aba `diagram.json`, substitua o conteúdo pelo de `wokwi/diagram.json`.
4. No menu ao lado das abas, use *New file…* para criar dois arquivos:
   - `pagina.h`, com o conteúdo de `BitcoinTracker/pagina.h`;
   - `segredos.h`, com o conteúdo de `wokwi/segredos.h` (rede `Wokwi-GUEST`, sem senha).

   Confira o nome antes de confirmar, porque o campo às vezes vem preenchido com
   o nome do arquivo anterior.
5. Na aba *Library Manager*, clique em *+* e adicione as bibliotecas abaixo. A
   lista tem nomes parecidos; escolha exatamente estes:
   - Adafruit SSD1306
   - Adafruit GFX Library
   - ArduinoJson

   O *Library Manager* gera o `libraries.txt` automaticamente. Um `libraries.txt`
   criado à mão deixou o display apagado em um dos testes, por isso o
   `wokwi/libraries.txt` do repositório serve apenas como referência.
6. Inicie a simulação. A primeira compilação leva alguns minutos.

Na simulação, o OLED mostra "Conectando Wi-Fi...", depois o IP e em seguida o
preço. O botão força uma nova consulta e acende o LED enquanto ela acontece; um
segundo clique logo em seguida mostra "Aguarde Xs". O monitor serial registra os
preços e os erros.

O `wokwi/segredos.h` também fixa o canal 6 da rede e aumenta os tempos limite da
consulta. Sem o canal o Wi-Fi simulado não conecta a tempo, e o HTTPS simulado é
bem mais lento que o da placa real.

Limitações do simulador:

- Se o Wi-Fi nunca conectar, troque de navegador. No Opera GX, o bloqueador de
  anúncios e a VPN interrompem a conexão que o Wokwi usa para simular a rede; no
  Edge e no Chrome a simulação funciona.
- Todos os usuários do Wokwi acessam a internet pelo mesmo endereço, então a
  CoinGecko pode responder com o erro 429 (limite de consultas). O firmware
  mostra esse erro no OLED e tenta de novo no ciclo seguinte.
- A versão gratuita não permite abrir a página web do ESP32 simulado. Para ver a
  página, abra `web/index.html` diretamente no navegador (modo simulado).

## Alterações na página

1. Edite `web/index.html`.
2. Teste de uma das duas formas:
   - abrindo o arquivo diretamente no navegador. A página entra em modo simulado
     e gera os próprios preços;
   - rodando o servidor simulado, que exige o [Node.js](https://nodejs.org), e
     abrindo <http://localhost:8765>. O servidor imita as rotas `/dados` e
     `/historico` do ESP32, então a página faz requisições reais:
     ```powershell
     node ferramentas/servidor-simulado.js
     ```
     Com o servidor no ar, `http://localhost:8765/simular/reiniciar` simula a
     placa reiniciando, e `http://localhost:8765/simular/lento` liga e desliga
     respostas lentas, como as do ESP32 durante uma consulta à CoinGecko.
3. Gere o `pagina.h` novamente:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1
   ```
4. Compile e grave o firmware.

O gancho de commit impede que o passo 3 seja esquecido: ele recusa o commit
quando o `pagina.h` difere do `index.html`. Para ativá-lo, rode uma vez:

```powershell
git config core.hooksPath .githooks
```

A página não depende de bibliotecas externas. O gráfico é desenhado em
`<canvas>`, o que mantém o arquivo pequeno o bastante para o ESP32 e permite
abri-lo sem internet.

## Rotas do ESP32

### `GET /dados`

Devolve o último valor armazenado, sem consultar a CoinGecko a cada acesso:

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

- `atualizadoEm` é o horário Unix, em segundos, em que a CoinGecko atualizou o
  preço. No plano gratuito ela renova o valor mais ou menos a cada minuto, então
  o ESP32 só registra um preço novo quando esse horário muda. Assim, duas
  consultas com o mesmo valor não aparecem como "sem alteração". A página usa o
  mesmo campo para posicionar os pontos no gráfico e descartar repetidos.
- `idadeSegundos` é o tempo desde que o preço atual chegou ao ESP32.
  `idadeConsultaSegundos` é o tempo desde a última consulta bem-sucedida, mesmo
  que o preço não tenha mudado. A página usa este segundo campo para marcar o
  valor como desatualizado.
- `variacao24h` vem como `null` quando a CoinGecko não envia o campo. Nesse caso
  a tela mostra "—" em vez de um 0% que pareceria real.
- `erro` contém a descrição da última falha, por exemplo `"Limite da API (429)"`.
  O último preço continua sendo enviado, e a página o marca como desatualizado.
- Se `valida` for `false` (placa recém-ligada, ainda sem preço), os campos de
  preço não são enviados e a página mostra "—".

### `GET /historico`

Preços armazenados na memória do ESP32, do mais antigo para o mais recente:

```json
{
  "pontos": [
    { "atualizadoEm": 1795387200, "preco": 97001.10 },
    { "atualizadoEm": 1795387260, "preco": 97020.55 }
  ]
}
```

- O ESP32 guarda até 1440 pontos, o equivalente a 24 h com um preço novo por
  minuto, usando cerca de 11 KB de RAM. Quando a capacidade se esgota, o ponto
  mais antigo é descartado.
- O histórico fica apenas na RAM e é perdido quando a placa reinicia. Gravá-lo
  na memória flash evitaria a perda, mas desgastaria a flash e complicaria o
  código sem ganho relevante para uma demonstração.
- A página também guarda no navegador os pontos que já recebeu (`localStorage`).
  Ao reabri-la no mesmo aparelho, as duas fontes são combinadas sem repetição.

### Comportamento da página quando o ESP32 não responde

Durante a consulta à CoinGecko o ESP32 pode passar até cerca de 13 s sem
atender a página. Por isso uma falha isolada não gera alerta: a página tenta de
novo em 3 s e só exibe "Sem resposta do ESP32" após 3 falhas seguidas.

## Limitações conhecidas

- O certificado HTTPS da CoinGecko não é validado (`setInsecure`). Isso é
  aceitável em um protótipo; em um produto, o certificado raiz deve ser fixado.
- Sem chave, a API gratuita da CoinGecko tem um limite baixo de consultas. Por
  isso o intervalo é de 60 s e o botão exige 15 s entre consultas. Uma chave
  *Demo* gratuita pode ser configurada no `segredos.h`.
- Enquanto busca o preço (normalmente 1 a 2 s, no pior caso cerca de 13 s), o
  ESP32 não responde à página, que trata essa situação como descrito acima.
- Com a tela do celular bloqueada, o navegador pausa a página. O gráfico não é
  afetado, porque o histórico fica no ESP32.
