@echo off
setlocal enabledelayedexpansion

:: Get the project root directory
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
cd /d "%PROJECT_ROOT%"

:: ============================================
:: Configuration - 可以手动设置或使用环境变量
:: ============================================

:: MSVC路径 - 优先使用环境变量，否则尝试自动检测
if defined VCINSTALLDIR (
    set "MSVC_ROOT=%VCINSTALLDIR%Tools\MSVC"
    :: 尝试找到最新的MSVC版本
    for /f "delims=" %%i in ('dir /b /ad /o-n "%MSVC_ROOT%" 2^>nul') do (
        set "MSVC_ROOT=%MSVC_ROOT%\%%i"
        goto :found_msvc
    )
) else if defined MSVC_ROOT (
    :: 使用手动设置的路径
) else (
    :: 尝试默认路径（需要手动修改）
    set "MSVC_ROOT=F:\dev\VS\VC\Tools\MSVC\14.50.35717"
    echo Warning: Using hardcoded MSVC path. Set VCINSTALLDIR or MSVC_ROOT environment variable.
)

:found_msvc

:: Windows SDK路径 - 优先使用环境变量
if defined WindowsSdkDir (
    set "SDK_ROOT=%WindowsSdkDir%"
) else if defined SDK_ROOT (
    :: 使用手动设置的路径
) else (
    set "SDK_ROOT=C:\Program Files (x86)\Windows Kits\10"
)

:: Windows SDK版本 - 优先使用环境变量
if defined WindowsSDKVersion (
    :: 移除末尾的反斜杠
    set "SDK_VER=%WindowsSDKVersion:~0,-1%"
) else if defined SDK_VER (
    :: 使用手动设置的版本
) else (
    :: 尝试找到最新的SDK版本
    for /f "delims=" %%i in ('dir /b /ad /o-n "%SDK_ROOT%\Include" 2^>nul') do (
        set "SDK_VER=%%i"
        goto :found_sdk
    )
    set "SDK_VER=10.0.26100.0"
    echo Warning: Using hardcoded SDK version. Set WindowsSDKVersion or SDK_VER environment variable.
)

:found_sdk

:: CUDA路径 - 优先使用环境变量
if defined CUDA_PATH (
    :: 使用环境变量
) else if defined CUDA_PATH_V11_0 (
    set "CUDA_PATH=%CUDA_PATH_V11_0%"
) else if defined CUDA_PATH_V12_0 (
    set "CUDA_PATH=%CUDA_PATH_V12_0%"
) else (
    :: 尝试默认路径（需要手动修改）
    set "CUDA_PATH=F:\cuda"
    echo Warning: Using hardcoded CUDA path. Set CUDA_PATH environment variable.
)

:: ============================================
:: 验证路径是否存在
:: ============================================
if not exist "%MSVC_ROOT%\bin\Hostx64\x64\cl.exe" (
    echo Error: MSVC compiler not found at: %MSVC_ROOT%
    echo Please set VCINSTALLDIR or MSVC_ROOT environment variable, or edit this script.
    exit /b 1
)

if not exist "%SDK_ROOT%\Include\%SDK_VER%" (
    echo Error: Windows SDK not found at: %SDK_ROOT%\Include\%SDK_VER%
    echo Please set WindowsSDKVersion or SDK_VER environment variable, or edit this script.
    exit /b 1
)

if not exist "%CUDA_PATH%\include\nvml.h" (
    echo Error: CUDA/NVML headers not found at: %CUDA_PATH%\include
    echo Please set CUDA_PATH environment variable, or edit this script.
    exit /b 1
)

:: ============================================
:: 设置编译环境
:: ============================================
set PATH=%MSVC_ROOT%\bin\Hostx64\x64;%PATH%

:: Include CUDA headers for NVML
set INCLUDE=%MSVC_ROOT%\include;%SDK_ROOT%\Include\%SDK_VER%\ucrt;%SDK_ROOT%\Include\%SDK_VER%\shared;%SDK_ROOT%\Include\%SDK_VER%\um;%CUDA_PATH%\include
set LIB=%MSVC_ROOT%\lib\x64;%SDK_ROOT%\Lib\%SDK_VER%\ucrt\x64;%SDK_ROOT%\Lib\%SDK_VER%\um\x64;%CUDA_PATH%\lib\x64

if not exist "bin" mkdir bin

set SRC_FILES=src\*.cpp
set OUT_NAME=NVBar

echo ============================================
echo Building %OUT_NAME%...
echo ============================================
echo MSVC: %MSVC_ROOT%
echo SDK:  %SDK_ROOT%\Include\%SDK_VER%
echo CUDA: %CUDA_PATH%
echo ============================================
echo.

:: Compile with CL.exe
:: /MT = Static Runtime
:: /O2 = Maximize Speed
:: /EHsc = Exception Handling
:: /W3 = Warning Level 3
cl.exe /nologo /MT /O2 /EHsc /W3 %SRC_FILES% /Fe"bin\%OUT_NAME%.exe" /link /INCREMENTAL:NO nvml.lib user32.lib gdi32.lib shell32.lib Advapi32.lib

if %errorlevel% neq 0 (
    echo.
    echo Error: Build failed.
    exit /b %errorlevel%
)

echo.
echo ============================================
echo Build successful!
echo Output: bin\%OUT_NAME%.exe
echo ============================================
