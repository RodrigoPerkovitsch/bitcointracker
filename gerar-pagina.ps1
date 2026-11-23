# Gera BitcoinTracker\pagina.h a partir de web\index.html.
# Rode sempre que alterar a pagina: a ESP32 so enxerga o que esta no .h.
#
#   powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1
#
# Com -Verificar nao altera nada: so confere se o pagina.h esta em dia.
# E o que o gancho de commit (.githooks/pre-commit) usa.
#
#   powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1 -Verificar

param([switch]$Verificar)

$raiz = $PSScriptRoot
$origem = Join-Path $raiz 'web\index.html'
$destino = Join-Path $raiz 'BitcoinTracker\pagina.h'

# Normaliza quebras de linha: o Git no Windows pode trocar LF por CRLF, e isso
# nao deve contar como "pagina diferente".
$html = [IO.File]::ReadAllText($origem, [Text.Encoding]::UTF8).Replace("`r`n", "`n")
if ($html.Contains(')rawliteral"')) {
    throw 'index.html contem a sequencia )rawliteral" e quebraria o pagina.h'
}

$conteudo = "// ARQUIVO GERADO por gerar-pagina.ps1 a partir de web/index.html.`n" +
            "// Nao edite aqui: edite web/index.html e rode o script de novo.`n" +
            "#pragma once`n`n" +
            "const char PAGINA_HTML[] PROGMEM = R`"rawliteral(" + $html + ")rawliteral`";`n"

if ($Verificar) {
    $atual = ''
    if (Test-Path $destino) {
        $atual = [IO.File]::ReadAllText($destino, [Text.Encoding]::UTF8).Replace("`r`n", "`n")
    }
    if ($atual -ne $conteudo) {
        Write-Host 'pagina.h esta DESATUALIZADO em relacao a web/index.html.'
        Write-Host 'Rode: powershell -ExecutionPolicy Bypass -File .\gerar-pagina.ps1'
        exit 1
    }
    Write-Host 'pagina.h em dia.'
    exit 0
}

[IO.File]::WriteAllText($destino, $conteudo, (New-Object Text.UTF8Encoding($false)))
$kb = [math]::Round([Text.Encoding]::UTF8.GetByteCount($html) / 1024, 1)
Write-Host "pagina.h gerado ($kb KB)."
