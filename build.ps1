# ==============================================================================
#   FILE: build.ps1
#   DESCRIPTION: Čistý build bez nepoužitých proměnných.
# ==============================================================================

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $projectDir

# 1. Definice build složky
$buildDir = Join-Path $projectDir "build"

# 2. Cesta k vcpkg toolchainu (Tohle CMake potřebuje k nalezení knihoven)
$cmakeToolchain = Join-Path $projectDir "vcpkg/scripts/buildsystems/vcpkg.cmake"

# 3. Zastavení běžící aplikace
Write-Host "Zastavuji běžící instanci aplikace..." -ForegroundColor Cyan
Get-Process SonarMidiPlayer -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1

# 4. CMake konfigurace
Write-Host "Spouštím CMake konfiguraci..." -ForegroundColor Cyan
# AI: tripletDir už netřeba, CMake si cesty k vcpkg vytáhne z toolchainu sám
cmake -S . -B $buildDir -A x64 -DCMAKE_TOOLCHAIN_FILE="$cmakeToolchain"

if ($LASTEXITCODE -ne 0) {
    Write-Host "Chyba při konfiguraci CMake!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# 5. Samotný Build (Release)
Write-Host "Spouštím build (Release)..." -ForegroundColor Cyan
cmake --build $buildDir --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Chyba při buildování Release!" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Hotovo! Vše je čisté a připravené." -ForegroundColor Green