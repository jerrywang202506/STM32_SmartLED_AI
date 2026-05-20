# FreeRTOS_LED_Effects — STM32F4-Discovery AI-Coding Demo

## 使用前提 / Prerequisites

**本项目仅包含作者自行生成的应用层代码、CMake 构建脚本及配套文档，不包含 ST 官方 HAL、BSP、CMSIS 或启动文件。**  
**This repository only contains the author-generated application code, CMake build scripts, and documentation. ST official HAL, BSP, CMSIS, or startup files are NOT included.**

因此，在编译前，你需要自行获取 ST 官方固件库：  
Therefore, you must obtain the official ST firmware library before building:

```bash
git clone https://github.com/STMicroelectronics/STM32CubeF4.git

如何使用 / How to Use
1. 获取官方库 / Get the Official Library
bash
git clone https://github.com/STMicroelectronics/STM32CubeF4.git
cd STM32CubeF4

2.置入本工程 / Place This Project
将本仓库的 FreeRTOS_LED_Effects 文件夹整体复制到官方库以下目录：
STM32CubeF4/Projects/STM32F4-Discovery/Applications/
复制后的路径应如下所示：
STM32CubeF4/
└── Projects/
    └── STM32F4-Discovery/
        └── Applications/
            └── FreeRTOS_LED_Effects/   <-- 本仓库内容 / this repo
                ├── CMakeLists.txt
                ├── build.sh
                ├── Inc/
                ├── Src/
                └── cmake/
3. 编译与烧录 / Build & Flash
bash
cd STM32CubeF4/Projects/STM32F4-Discovery/Applications/FreeRTOS_LED_Effects
./build.sh
cmake --build . --target flash
或者直接运行bat文件。

工程说明 / Project Brief
本项目基于 Kimi CLI 主导生成，展示如何在零商业 IDE、零 License 的环境下，利用 CMake + GCC + OpenOCD 完成 STM32F4 + FreeRTOS 的完整开发闭环。详细技术复盘请见 CSDN 原文。
This project is AI-generated (via Kimi CLI) and demonstrates a complete STM32F4 + FreeRTOS development workflow using CMake + GCC + OpenOCD, with zero commercial IDE dependency. For the full technical write-up, please refer to the CSDN article.

免责声明 / Disclaimer
本项目为作者利用个人业余时间完成的教育分享性质的技术探索，不涉及任何商业机密或专有技术。
