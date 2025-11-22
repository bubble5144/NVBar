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
@echo off
setlocal enabledelayedexpansion

:: ==================================================
:: Configuration (可在此处直接修改，或通过环境变量覆盖)
:: ==================================================
:: 示例：在命令行中设置： set MSVC_ROOT=C:\Path\To\MSVC
:: 如果你使用 Visual Studio 的 "x64 Native Tools Command Prompt"，通常无需修改。

:: --- 可修改项（默认值，按需修改） ---
set "MSVC_ROOT=F:\dev\VS\VC\Tools\MSVC\14.50.35717"
set "SDK_ROOT=C:\Program Files (x86)\Windows Kits\10"
set "SDK_VER=10.0.26100.0"
set "CUDA_PATH=F:\cuda"
set "OUT_NAME=NVBar"
set "SRC_GLOB=src\*.cpp"

:: 如果外部已定义这些变量，则使用外部定义的值（覆盖默认）
if defined VCINSTALLDIR set "MSVC_ROOT=%VCINSTALLDIR%Tools\MSVC"
if defined WindowsSdkDir set "SDK_ROOT=%WindowsSdkDir%"
if defined WindowsSDKVersion set "SDK_VER=%WindowsSDKVersion:~0,-1%"
if defined CUDA_PATH_V11_0 set "CUDA_PATH=%CUDA_PATH_V11_0%"
if defined CUDA_PATH_V12_0 set "CUDA_PATH=%CUDA_PATH_V12_0%"
if defined MSVC_ROOT set "MSVC_ROOT=%MSVC_ROOT%"
if defined SDK_ROOT set "SDK_ROOT=%SDK_ROOT%"
if defined SDK_VER set "SDK_VER=%SDK_VER%"
if defined CUDA_PATH set "CUDA_PATH=%CUDA_PATH%"

:: ==================================================
:: Start: normalize and enter project root
:: ==================================================
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

:: ==================================================
:: 清理旧的中间文件（.obj, .pdb, .ilk 等）
:: ==================================================
echo Cleaning intermediate build files...
for /r %%F in (*.obj *.pdb *.ilk *.iobj *.ipdb) do (
    if exist "%%F" del /f /q "%%F" 2>nul
)
echo Done cleaning.

if not exist "bin" mkdir bin

:: 检查必要的工具和头文件
if not exist "%MSVC_ROOT%\bin\Hostx64\x64\cl.exe" (
    echo Error: MSVC compiler not found at: %MSVC_ROOT%
    echo Please set VCINSTALLDIR or MSVC_ROOT environment variable, or edit this script.
    exit /b 1
)

if not exist "%SDK_ROOT%\Include\%SDK_VER%" (
    echo Warning: Windows SDK path not found at: %SDK_ROOT%\Include\%SDK_VER%
    echo If you have multiple SDK versions, set WindowsSDKVersion or SDK_VER env var.
)

if not exist "%CUDA_PATH%\include\nvml.h" (
    echo Warning: CUDA/NVML headers not found at: %CUDA_PATH%\include
    echo If you only need to link against nvml.lib, you may copy nvml.h and nvml.lib into local include/lib folders.
)

:: 设置编译环境
set PATH=%MSVC_ROOT%\bin\Hostx64\x64;%PATH%
set INCLUDE=%MSVC_ROOT%\include;%SDK_ROOT%\Include\%SDK_VER%\ucrt;%SDK_ROOT%\Include\%SDK_VER%\shared;%SDK_ROOT%\Include\%SDK_VER%\um;%CUDA_PATH%\include
set LIB=%MSVC_ROOT%\lib\x64;%SDK_ROOT%\Lib\%SDK_VER%\ucrt\x64;%SDK_ROOT%\Lib\%SDK_VER%\um\x64;%CUDA_PATH%\lib\x64

echo.
echo Building %OUT_NAME%...

:: 编译到临时文件，防止覆盖正在使用的文件
cl.exe /nologo /MT /O2 /EHsc /W3 %SRC_GLOB% /Fe"bin\%OUT_NAME%_new.exe" /link /INCREMENTAL:NO nvml.lib user32.lib gdi32.lib shell32.lib Advapi32.lib

if %errorlevel% neq 0 (
    echo.
    echo Error: Build failed.
    echo See above messages for details.
    exit /b %errorlevel%
)

:: 替换旧文件（如果存在）
if exist "bin\%OUT_NAME%.exe" (
    del /f /q "bin\%OUT_NAME%.exe" 2>nul
)
if exist "bin\%OUT_NAME%_new.exe" (
    ren "bin\%OUT_NAME%_new.exe" "%OUT_NAME%.exe"
)

:: 最后再清理一次中间文件（可选）
for /r %%F in (*.obj *.pdb *.ilk *.iobj *.ipdb) do (
    if exist "%%F" del /f /q "%%F" 2>nul
)

echo.
echo Build successful: bin\%OUT_NAME%.exe
echo ============================================

endlocal
