# Packaging script for Spherical Triangle Complex Engineering Problem
# Compresses all required submission files into a single ZIP archive

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$zipOutput = Join-Path $scriptDir "SphericalTriangle_CEP_Submission.zip"
if (Test-Path $zipOutput) {
    Remove-Item $zipOutput -Force
}

$tempStaging = Join-Path $scriptDir "staging_submission"
if (Test-Path $tempStaging) {
    Remove-Item $tempStaging -Recurse -Force
}
New-Item -ItemType Directory -Path $tempStaging | Out-Null
New-Item -ItemType Directory -Path "$tempStaging/src" | Out-Null
New-Item -ItemType Directory -Path "$tempStaging/include" | Out-Null
New-Item -ItemType Directory -Path "$tempStaging/shaders" | Out-Null
New-Item -ItemType Directory -Path "$tempStaging/report" | Out-Null

# 1. Copy source files (.cpp, .c, .h)
Copy-Item "$scriptDir/src/*" "$tempStaging/src" -Recurse -Force
Copy-Item "$scriptDir/include/*" "$tempStaging/include" -Recurse -Force

# 2. Copy shader files (.vert, .frag)
Copy-Item "$scriptDir/shaders/*" "$tempStaging/shaders" -Recurse -Force

# 3. Copy reports
Copy-Item "$scriptDir/report/*" "$tempStaging/report" -Recurse -Force

# 4. Copy Makefile & Plan
Copy-Item "$scriptDir/Makefile" "$tempStaging/" -Force
Copy-Item "$scriptDir/Implementation_Plan.md" "$tempStaging/" -Force

# 5. Create ZIP Archive
Compress-Archive -Path "$tempStaging/*" -DestinationPath $zipOutput -CompressionLevel Optimal
Remove-Item $tempStaging -Recurse -Force

Write-Host "Submission ZIP successfully generated at: $zipOutput" -ForegroundColor Green
Get-Item $zipOutput | Select-Object Name, Length, LastWriteTime
