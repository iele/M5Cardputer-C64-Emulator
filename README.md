# M5Cardputer C64 Emulator

## English Version

### Table of Contents
- [Introduction](#introduction)
- [Development Status](#development-status)
- [Installation](#installation)
- [Usage](#usage)
- [Supported and Unsupported Features](#supported-and-unsupported-features)
- [File Structure](#file-structure)
- [License](#license)
- [Contributing](#contributing)
- [Acknowledgements](#acknowledgements)

### Introduction
M5Cardputer C64 Emulator is a project that emulates the Commodore 64, designed specifically for the M5Cardputer device. The project is based on [retroelec/T-HMI-C64](https://github.com/retroelec/T-HMI-C64).

### Development Status
The project is currently under development, and features and functionalities may be updated and improved continuously. Stay tuned for the latest progress and feel free to provide valuable feedback.

### Installation
To install and run this project, follow these steps:

1. Clone this repository:
    ```sh
    git clone https://github.com/iele/M5Cardputer-C64-Emulator.git
    ```
2. Navigate to the project directory:
    ```sh
    cd M5Cardputer-C64-Emulator
    ```
3. Upload the program to the device using PlatformIO:
    ```sh
    pio run -t upload
    ```

### Usage
After successfully installing and compiling, upload the program to the M5Cardputer device and run it. The emulator supports BAS (BASIC code) and PRG file formats. Upon startup, you have two options:
- `Direct to BASIC`: Directly enter the BASIC 2.0 interface.
- `Load from SD`: Load files from the SD card.

#### Button Functions
- `BtnA（Button A）`:
  - Long press: Soft reset to the selection menu.
  - Short press: Toggle between keyboard or joystick mode.

#### Joystick Mapping
- `Fire`: Mapped to the `A` key on the keyboard.
- `Direction keys`: Mapped to the `Arrow keys` on the keyboard.

### Supported and Unsupported Features

#### Supported Features
- `BASIC/PRG file execution`.

#### Unsupported Features
Based on the T-HMI-C64 project, the following features are currently unsupported or have issues:
- SID chip emulation is not supported.
- Tape/disk drive emulation is not supported.
- VIC register `$d011` is only partially implemented (TODO: bits 3 and 4).
- Not all "illegal" opcodes of the 6502 CPU are implemented.
- Some games have graphical errors.
- Some games do not run at all.

Refer to the source code and comments for detailed usage methods and commands.

### File Structure
- `.vscode/` - Visual Studio Code configuration files
- `include/` - Header files
- `lib/` - Library files
- `src/` - Source code files
- `test/` - Test code
- `.gitignore` - Git ignore file list
- `LICENSE` - License file
- `platformio.ini` - PlatformIO configuration file

### License
This project is licensed under the [GPL-3.0 License](https://opensource.org/licenses/GPL-3.0). See the `LICENSE` file for details.

### Contributing
Contributions and issues are welcome. If you have any suggestions or have found a bug, please submit an issue or pull request via GitHub.

### Acknowledgements
Special thanks to the [retroelec/T-HMI-C64](https://github.com/retroelec/T-HMI-C64) project for its support and inspiration.

---

## 中文版

### 目录
- [简介](#简介)
- [开发状态](#开发状态)
- [安装](#安装)
- [使用](#使用)
- [支持与不支持的功能](#支持与不支持的功能)
- [文件结构](#文件结构)
- [许可](#许可)
- [贡献](#贡献)
- [致谢](#致谢)

### 简介
M5Cardputer C64 Emulator 是一个模拟 Commodore 64 的项目，专为 M5Cardputer 设备设计。该项目基于 [retroelec/T-HMI-C64](https://github.com/retroelec/T-HMI-C64) 开发。

### 开发状态
当前项目仍然在开发中，功能和特性可能会不断更新和改进。欢迎关注项目的最新进展，并提出宝贵意见。

### 安装
要安装并运行该项目，请按照以下步骤操作：

1. 克隆此仓库：
    ```sh
    git clone https://github.com/iele/M5Cardputer-C64-Emulator.git
    ```
2. 进入项目目录：
    ```sh
    cd M5Cardputer-C64-Emulator
    ```
3. 使用 PlatformIO 上传程序到设备：
    ```sh
    pio run -t upload
    ```

### 使用
在成功安装并编译后，将程序上传到 M5Cardputer 设备并运行。该模拟器支持 BAS（BASIC 代码）和 PRG 文件格式，启动后有以下两种选项：
- `Direct to BASIC`：直接进入 BASIC 2.0 界面。
- `Load from SD`：从 SD 卡加载文件。

#### 按钮功能
- `BtnA（Button A）`：
  - 长按：软复位至选择界面。
  - 短按：切换键盘或者摇杆模式。

#### 摇杆映射
- `Fire`：映射为键盘上的 `A` 键。
- `方向键`：映射为键盘上的 `上下左右` 键。

### 支持与不支持的功能

#### 支持的功能
- `BASIC/PRG 文件执行`。

#### 不支持的功能
基于 T-HMI-C64 项目，以下功能当前不支持或存在问题：
- 不支持 SID 芯片模拟。
- 不支持磁带/磁盘驱动器模拟。
- VIC 寄存器 `$d011` 仅部分实现（待办：第 3 和第 4 位）。
- 并未实现 6502 CPU 的全部 "非法" 操作码。
- 一些游戏存在图形错误。
- 一些游戏根本无法运行。

具体使用方法和命令请参考项目的源代码和注释。

### 文件结构
- `.vscode/` - Visual Studio Code 配置文件
- `include/` - 头文件
- `lib/` - 库文件
- `src/` - 源代码文件
- `test/` - 测试代码
- `.gitignore` - Git 忽略文件列表
- `LICENSE` - 许可证文件
- `platformio.ini` - PlatformIO 配置文件

### 许可
该项目采用 [GPL-3.0 许可证](https://opensource.org/licenses/GPL-3.0)。详细条款请参阅 `LICENSE` 文件。

### 贡献
欢迎贡献代码和提交 issue。如果你有任何建议或发现了问题，请通过 GitHub 提交 issue 或 pull request。

### 致谢
特别感谢 [retroelec/T-HMI-C64](https://github.com/retroelec/T-HMI-C64) 项目提供的支持和灵感。

---

感谢你对 M5Cardputer C64 Emulator 项目的关注和支持！
