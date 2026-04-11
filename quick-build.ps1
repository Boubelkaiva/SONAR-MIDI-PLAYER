# quick-build.ps1
# Rychlý rebuild bez čištění build složky

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $projectDir

Write-Host "Spouštím CMake build (bez čištění)..." -ForegroundColor Cyan

# Zastavení běžící aplikace
Write-Host "Zastavuji běžící instanci aplikace..." -ForegroundColor Cyan
Get-Process SonarMidiPlayer -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1

# Build Release (bez čištění)
$buildDir = Join-Path $projectDir "build"
cmake --build $buildDir --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Chyba při buildování!" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Hotovo! Výstupy jsou v $buildDir\bin\Release" -ForegroundColor Green
Write-Host "Spusť .\run.ps1 pro spuštění aplikace" -ForegroundColor Cyan
