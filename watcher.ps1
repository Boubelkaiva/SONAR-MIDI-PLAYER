# ==============================================================================
#
#   FILE: watcher.ps1
#   PROJECT: SONAR MIDI PLAYER
#   DESCRIPTION: Automated code aggregator with timestamp and project monitoring.
#   GENERATED ON: 09.04.2026
#
# ==============================================================================

$path = Get-Location

Write-Host "Sleduju zmeny v projektu (pouze .cpp a .h): $path" -ForegroundColor Green

$filter = "*.*"
$watcher = New-Object IO.FileSystemWatcher $path, $filter
$watcher.IncludeSubdirectories = $true
$watcher.EnableRaisingEvents = $true

$action = {
    $fileName = $Event.SourceEventArgs.Name
    
    # KRITICKA OCHRANA: Ignoruj zmeny v all_code.txt a watcher.ps1, aby nedoslo k cykleni
    if ($fileName -match "all_code.txt|watcher.ps1") { return }

    # Reagujeme jen na zdrojaky
    if ($fileName -notmatch "\.cpp$|\.h$") { return }

    Start-Sleep -Milliseconds 500

    $timestamp = Get-Date -Format "dd.MM.yyyy HH:mm:ss"
    Write-Host "[$timestamp] Zmena v $fileName detekovana, aktualizuju all_code.txt..." -ForegroundColor Yellow

    $header = "==============================================================================`n"
    $header += " GENERATED ON: $timestamp`n"
    $header += " ROOT PATH: $path`n"
    $header += "==============================================================================`n`n"
    $header | Out-File "all_code.txt" -Encoding utf8

    Get-ChildItem -Path $path -Recurse -Include *.cpp, *.h |
    Where-Object { 
        $_.FullName -notmatch "build|JUCE|vcpkg|\.git|\.vs|out|Debug|Release|x64" 
    } |
    ForEach-Object {
        $fileHeader = "===== FILE: $($_.FullName) ====="
        $content = Get-Content $_.FullName -Raw
        $footer = "`n-----------------------------`n"
        
        $fileHeader | Out-File "all_code.txt" -Append -Encoding utf8
        $content | Out-File "all_code.txt" -Append -Encoding utf8
        $footer | Out-File "all_code.txt" -Append -Encoding utf8
    }

    Write-Host "Hotovo!" -ForegroundColor Green
}

# Registrace udalosti
$handlers = @()
$handlers += Register-ObjectEvent $watcher "Changed" -Action $action
$handlers += Register-ObjectEvent $watcher "Created" -Action $action
$handlers += Register-ObjectEvent $watcher "Deleted" -Action $action
$handlers += Register-ObjectEvent $watcher "Renamed" -Action $action

try {
    while ($true) { Start-Sleep 1 }
}
finally {
    # Uklid pri zastaveni skriptu (Ctrl+C)
    $handlers | ForEach-Object { Unregister-Event -SourceIdentifier $_.Name }
    $watcher.Dispose()
}