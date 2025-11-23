# NVBar

**NVBar** 是一个极简的透明 GPU 监控小工具，固定显示在 Windows 任务栏旁，呈现 GPU 占用、温度与显存使用率。

## Features

- **极简**：透明背景，仅展示必要信息。
- **智能吸附**：自动定位到任务栏系统托盘左侧，不挡其他图标。
- **交互**：支持拖拽、右键菜单改色与开机自启。
- **原生高效**：基于 Win32 API + NVML，单文件可执行，体积小、启动快。

## Requirements

- NVIDIA GPU（驱动包含 NVML）
- Windows 10/11

## Usage

1. 从 Releases 页面下载可执行文件（见下方）。
2. 运行 `NVBar.exe`。
3. 右键打开菜单更改颜色或设置开机自启。

## Releases

- 最新版本：**v0.1.0**
- 下载页面： `bin\NVBar.exe`

## Build

1. 安装 CUDA Toolkit（或者仅复制 `nvml.h` 与 `nvml.lib` 到本地 `include`/`lib`）和 Visual Studio（MSVC）。
2. 在 `scripts\build.bat` 中设置 `MSVC_ROOT`、`SDK_ROOT`、`CUDA_PATH`（若需要）。
3. 运行：

```powershell
.\scripts\build.bat
```

编译成功后可执行文件位于 `bin\NVBar.exe`。

## License

MIT
# NVBar

**NVBar** is a minimalist, transparent GPU monitor that sits quietly on your Windows taskbar.

## Features

*   **Minimalist**: Transparent background, only essential stats (GPU, Temp, Memory).
*   **Smart Positioning**: Automatically docks to the right side of the taskbar (left of the system tray).
*   **Interactive**:
    *   **Drag**: Click and drag anywhere to reposition.
    *   **Right-Click**: Access menu for Colors, Autorun, and Exit.
*   **Portable**: Single executable, no installation required.
*   **Efficient**: Built with Win32 API and NVML.

## Requirements

*   **NVIDIA GPU** with latest drivers.
*   **Windows 10/11**.

## Usage

1.  Download `NVBar.exe` from `bin\`.
2.  Run it.
3.  Right-click to configure colors or set to run at startup.

## Build

To build from source:

1.  Install **CUDA Toolkit** (for NVML headers/libs) and **Visual Studio** (MSVC).
2.  Open `scripts\build.bat` and update the `MSVC_ROOT`, `SDK_ROOT`, and `CUDA_PATH` variables to match your installation paths.
3.  Run `scripts\build.bat`.

## License

MIT
