# ==============================================================================
#
#   FILE: run.ps1
#   DESCRIPTION: Chytre spusteni bez nutnosti rebuildovat vsechno.
#
# ==============================================================================

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$exeName = "SonarMidiPlayer.exe"

# Zkusime najit vsechny vyskyty exe v buildu a vybereme ten nejnovejsi
$latestExe = Get-ChildItem -Path "$projectDir\build" -Filter $exeName -Recurse | 
Sort-Object LastWriteTime -Descending | 
Select-Object -First 1

if ($null -eq $latestExe) {
    Write-Host "Chyba: Soubor $exeName nebyl nalezen v adresari build!" -ForegroundColor Red
    Write-Host "Musis nejdriv spustit .\build.ps1" -ForegroundColor Yellow
    exit 1
}

Write-Host "Spoustim nejnovsi verzi: $($latestExe.FullName)" -ForegroundColor Green
Write-Host "Cas posledni zmeny: $($latestExe.LastWriteTime)" -ForegroundColor Cyan

# Spusteni z rootu projektu, aby fungovaly relativni cesty
Start-Process -FilePath $latestExe.FullName -WorkingDirectory $projectDir