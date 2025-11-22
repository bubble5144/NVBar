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

1.  Download `NVBar.exe` from the [Releases](https://github.com/bubble5144/NVBar/releases) page.
2.  Run it.
3.  Right-click to configure colors or set to run at startup.

## Build

To build from source:

1.  Install **CUDA Toolkit** (for NVML headers/libs) and **Visual Studio** (MSVC).
2.  Open `scripts\build.bat` and update the `MSVC_ROOT`, `SDK_ROOT`, and `CUDA_PATH` variables to match your installation paths.
3.  Run `scripts\build.bat`.

## License

MIT
