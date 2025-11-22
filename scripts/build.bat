@echo off
setlocal enabledelayedexpansion

:: Get the project root directory
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
cd /d "%PROJECT_ROOT%"

:: Set paths manually
set MSVC_ROOT=F:\dev\VS\VC\Tools\MSVC\14.50.35717
set SDK_ROOT=C:\Program Files (x86)\Windows Kits\10
set SDK_VER=10.0.26100.0
set CUDA_PATH=F:\cuda

set PATH=%MSVC_ROOT%\bin\Hostx64\x64;%PATH%

:: Include CUDA headers for NVML
set INCLUDE=%MSVC_ROOT%\include;%SDK_ROOT%\Include\%SDK_VER%\ucrt;%SDK_ROOT%\Include\%SDK_VER%\shared;%SDK_ROOT%\Include\%SDK_VER%\um;%CUDA_PATH%\include
set LIB=%MSVC_ROOT%\lib\x64;%SDK_ROOT%\Lib\%SDK_VER%\ucrt\x64;%SDK_ROOT%\Lib\%SDK_VER%\um\x64;%CUDA_PATH%\lib\x64

if not exist "bin" mkdir bin

set SRC_FILES=src\*.cpp
set OUT_NAME=NVBar

echo Building %OUT_NAME%...

:: Compile with CL.exe
:: /MT = Static Runtime
:: /O2 = Maximize Speed
:: /EHsc = Exception Handling
cl.exe /nologo /MT /O2 /EHsc %SRC_FILES% /Fe"bin\%OUT_NAME%.exe" /link /INCREMENTAL:NO nvml.lib user32.lib gdi32.lib shell32.lib Advapi32.lib

if %errorlevel% neq 0 (
    echo Error: Build failed.
    exit /b %errorlevel%
)

echo.
echo Build successful!
echo Running bin\%OUT_NAME%.exe...
