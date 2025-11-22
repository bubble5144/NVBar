@echo off
setlocal

:: ==================================================
:: NVBar build script (clean, configurable, robust)
:: - Put environment overrides at the top (change or set env vars)
:: - Auto-clean intermediate files before/after build
:: - Use temporary output then replace
:: ==================================================

:: --- Default configuration (change here or export environment variables) ---
set "MSVC_ROOT=F:\dev\VS\VC\Tools\MSVC\14.50.35717"
set "SDK_ROOT=C:\Program Files (x86)\Windows Kits\10"
set "SDK_VER=10.0.26100.0"
set "CUDA_PATH=F:\cuda"
set "OUT_NAME=NVBar"
set "SRC_GLOB=src\*.cpp"

:: Allow environment variables to override defaults
if defined VCINSTALLDIR set "MSVC_ROOT=%VCINSTALLDIR%\Tools\MSVC"
if defined MSVC_ROOT set "MSVC_ROOT=%MSVC_ROOT%"
if defined WindowsSdkDir set "SDK_ROOT=%WindowsSdkDir%"
if defined WindowsSDKVersion set "SDK_VER=%WindowsSDKVersion:~0,-1%"
if defined CUDA_PATH_V11_0 set "CUDA_PATH=%CUDA_PATH_V11_0%"
if defined CUDA_PATH_V12_0 set "CUDA_PATH=%CUDA_PATH_V12_0%"
if defined SDK_ROOT set "SDK_ROOT=%SDK_ROOT%"
if defined SDK_VER set "SDK_VER=%SDK_VER%"
if defined CUDA_PATH set "CUDA_PATH=%CUDA_PATH%"

:: Enter project root
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
cd /d "%PROJECT_ROOT%"

echo ============================================
echo NVBar build helper
echo Project root: %PROJECT_ROOT%
echo MSVC_ROOT: %MSVC_ROOT%
echo SDK_ROOT:  %SDK_ROOT%\Include\%SDK_VER%
echo CUDA_PATH: %CUDA_PATH%
echo OUT_NAME: %OUT_NAME%
echo ============================================

:: Clean intermediate files recursively
echo Cleaning intermediate build files...
pushd "%PROJECT_ROOT%"
for /r %%F in (*.obj *.pdb *.ilk *.iobj *.ipdb) do (
    if exist "%%F" del /f /q "%%F" 2>nul
)
popd
echo Done cleaning.

if not exist "bin" mkdir bin

:: Basic checks
:: Check MSVC compiler: use goto to avoid parsing problems when paths contain parentheses
: Try to find cl.exe; if not present, attempt PowerShell helper to detect environment
if exist "%MSVC_ROOT%\bin\Hostx64\x64\cl.exe" goto :msvc_ok
echo MSVC compiler not found at: %MSVC_ROOT%
echo Attempting automatic detection via PowerShell helper (scripts\find_vs.ps1)...
for /f "usebackq delims=" %%E in (`powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0find_vs.ps1"`) do (
    rem Each line from helper is KEY=VALUE
    for /f "delims=" %%K in ("%%E") do set "%%K"
)

rem If helper found CLPATH or MSVC_ROOT, set PATH accordingly
if defined CLPATH (
    for %%D in ("%CLPATH%") do set "CLDIR=%%~dpD"
    set "PATH=%CLDIR%;%PATH%"
    goto :msvc_ok
)
if defined MSVC_ROOT if exist "%MSVC_ROOT%\bin\Hostx64\x64\cl.exe" (
    set "PATH=%MSVC_ROOT%\bin\Hostx64\x64;%PATH%"
    goto :msvc_ok
)

echo Error: MSVC compiler still not found after detection. Please run this script from "x64 Native Tools Command Prompt for VS" or set MSVC_ROOT correctly.
exit /b 1
:msvc_ok

:: SDK and CUDA checks (single-line IF to avoid parentheses parsing issues)
if not exist "%SDK_ROOT%\Include\%SDK_VER%" echo Warning: Windows SDK path not found at: %SDK_ROOT%\Include\%SDK_VER%
if not exist "%CUDA_PATH%\include\nvml.h" echo Warning: CUDA/NVML headers not found at: %CUDA_PATH%\include

:: Set environment for compiler
set "PATH=%MSVC_ROOT%\bin\Hostx64\x64;%PATH%"
set "INCLUDE=%MSVC_ROOT%\include;%SDK_ROOT%\Include\%SDK_VER%\ucrt;%SDK_ROOT%\Include\%SDK_VER%\shared;%SDK_ROOT%\Include\%SDK_VER%\um;%CUDA_PATH%\include"
set "LIB=%MSVC_ROOT%\lib\x64;%SDK_ROOT%\Lib\%SDK_VER%\ucrt\x64;%SDK_ROOT%\Lib\%SDK_VER%\um\x64;%CUDA_PATH%\lib\x64"

echo.
echo Building %OUT_NAME%...

:: Compile to temporary file then atomically replace
cl.exe /nologo /MT /O2 /EHsc /W3 %SRC_GLOB% /Fe"bin\%OUT_NAME%_new.exe" /link /INCREMENTAL:NO nvml.lib user32.lib gdi32.lib shell32.lib Advapi32.lib

if %errorlevel% neq 0 (
    echo.
    echo Error: Build failed. See above for compiler/linker messages.
    exit /b %errorlevel%
)

if exist "bin\%OUT_NAME%.exe" del /f /q "bin\%OUT_NAME%.exe" 2>nul
if exist "bin\%OUT_NAME%_new.exe" ren "bin\%OUT_NAME%_new.exe" "%OUT_NAME%.exe"

:: Final cleanup (optional)
pushd "%PROJECT_ROOT%"
for /r %%F in (*.obj *.pdb *.ilk *.iobj *.ipdb) do (
    if exist "%%F" del /f /q "%%F" 2>nul
)
popd

echo.
echo Build successful: bin\%OUT_NAME%.exe
echo ============================================

endlocal
