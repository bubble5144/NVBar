# find_vs.ps1 - detect Visual Studio/MSVC, Windows SDK, and CUDA paths
# Outputs key=value lines for batch parsing

function Write-KV($k,$v){ if($null -ne $v -and $v -ne ''){ Write-Output "$k=$v" } }

# Try vswhere
$vsPath = $null
$vswhere = "$env:ProgramFiles(x86)\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $inst = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
    if ($inst) { $vsPath = $inst.Trim() }
}

# Fallback common path
if (-not $vsPath) {
    $candidate = 'F:\dev\VS'
    if (Test-Path $candidate) { $vsPath = $candidate }
}

if ($vsPath) {
    # Try to find MSVC folder under installation
    $msvcRoot = Get-ChildItem -Path (Join-Path $vsPath 'VC\Tools\MSVC') -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
    if ($msvcRoot) { Write-KV 'MSVC_ROOT' ($msvcRoot.FullName) }
}

# Windows SDK
$wks = 'C:\Program Files (x86)\Windows Kits\10\Include'
if (Test-Path $wks) {
    $v = Get-ChildItem -Path $wks -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
    if ($v) { Write-KV 'SDK_ROOT' 'C:\Program Files (x86)\Windows Kits\10' ; Write-KV 'SDK_VER' $v.Name }
}

# CUDA/NVML
$cuda = $null
$stdCuda = 'C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA'
if (Test-Path $stdCuda) {
    $cuda = Get-ChildItem -Path $stdCuda -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1 | ForEach-Object { $_.FullName }
}
if (-not $cuda -and Test-Path 'F:\cuda') { $cuda = 'F:\cuda' }
if ($cuda) { Write-KV 'CUDA_PATH' $cuda }

# Also try to detect cl.exe in MSVC_ROOT
if ($msvcRoot) {
    $cl = Join-Path $msvcRoot.FullName 'bin\Hostx64\x64\cl.exe'
    if (Test-Path $cl) { Write-KV 'CLPATH' $cl }
}
