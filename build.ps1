# build.ps1
# ----------------------------------------------------
# Kompletní clean build pro SonarMidiPlayer s vcpkg
# + kontrola FluidSynth a RtMidi
# + spuštění exe po buildu
# ----------------------------------------------------

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $projectDir

# --- Konfigurace knihoven ---
$tripletDir = "$projectDir/vcpkg/installed/x64-windows"

$fluidsynthInclude = Join-Path $tripletDir "include\fluidsynth.h"
$fluidsynthLib = Join-Path $tripletDir "lib\libfluidsynth-3.lib"

$rtmidiInclude = Join-Path $tripletDir "include\rtmidi\RtMidi.h"
$rtmidiLib = Join-Path $tripletDir "lib\rtmidi.lib"

# --- Kontrola závislostí ---
Write-Host "Kontrola závislostí..." -ForegroundColor Cyan
$missing = @()
if (-not (Test-Path $fluidsynthInclude)) { $missing += "fluidsynth.h (include)" }
if (-not (Test-Path $fluidsynthLib)) { $missing += "libfluidsynth-3.lib (lib)" }
if (-not (Test-Path $rtmidiInclude)) { $missing += "RtMidi.h (include)" }
if (-not (Test-Path $rtmidiLib)) { $missing += "rtmidi.lib (lib)" }

if ($missing.Count -gt 0) {
    Write-Host "Chyba: Následující soubory nejsou nalezeny:" -ForegroundColor Red
    $missing | ForEach-Object { Write-Host "  $_" -ForegroundColor Red }
    Write-Host "Zkontrolujte instalaci knihoven ve vcpkg a triplet x64-windows." -ForegroundColor Red
    exit 1
}

# --- Čištění build složky ---
$buildDir = Join-Path $projectDir "build"
if (Test-Path $buildDir) {
    Write-Host "Čištění starého buildu..." -ForegroundColor Cyan
    Remove-Item -Recurse -Force $buildDir
}

# --- CMake konfigurace ---
$cmakeToolchain = Join-Path $projectDir "vcpkg/scripts/buildsystems/vcpkg.cmake"
Write-Host "Spouštím CMake konfiguraci..." -ForegroundColor Cyan
cmake -S . -B $buildDir -A x64 -DCMAKE_TOOLCHAIN_FILE="$cmakeToolchain"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Chyba při konfiguraci CMake!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# --- Build Release i Debug ---
$configs = @("Release", "Debug")
foreach ($config in $configs) {
    Write-Host "Spouštím build ($config)..." -ForegroundColor Cyan
    cmake --build $buildDir --config $config
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Chyba při buildování $config!" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

# --- Kontrola výstupu ---
$binDir = Join-Path $buildDir "bin\Release"
$exePath = Join-Path $binDir "SonarMidiPlayer.exe"

if (-not (Test-Path $exePath)) {
    Write-Host "Chyba: Soubor $exePath nebyl vytvořen!" -ForegroundColor Red
    exit 1
}

Write-Host "Hotovo! Výstupy jsou v $binDir" -ForegroundColor Green

# --- Spuštění exe ---
Write-Host "Spouštím SonarMidiPlayer.exe..." -ForegroundColor Cyan
Start-Process $exePath