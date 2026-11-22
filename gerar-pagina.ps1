# Gera BitcoinTracker\pagina.h a partir de web\index.html.
# Rode sempre que alterar a pagina: a ESP32 so enxerga o que esta no .h.
#
#   powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1

$raiz = $PSScriptRoot
$origem = Join-Path $raiz 'web\index.html'
$destino = Join-Path $raiz 'BitcoinTracker\pagina.h'

$html = [IO.File]::ReadAllText($origem, [Text.Encoding]::UTF8)
if ($html.Contains(')rawliteral"')) {
    throw 'index.html contem a sequencia )rawliteral" e quebraria o pagina.h'
}

$conteudo = "// ARQUIVO GERADO por gerar-pagina.ps1 a partir de web/index.html.`n" +
            "// Nao edite aqui: edite web/index.html e rode o script de novo.`n" +
            "#pragma once`n`n" +
            "const char PAGINA_HTML[] PROGMEM = R`"rawliteral(" + $html + ")rawliteral`";`n"

[IO.File]::WriteAllText($destino, $conteudo, (New-Object Text.UTF8Encoding($false)))
$kb = [math]::Round([Text.Encoding]::UTF8.GetByteCount($html) / 1024, 1)
Write-Host "pagina.h gerado ($kb KB)."
