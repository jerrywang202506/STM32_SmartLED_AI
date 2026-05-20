@echo off
REM Build script for FreeRTOS_LED_Effects using CMake + MinGW Makefiles
REM Usage: build.bat           - Build the project
REM        build.bat clean     - Remove build directory

cd /d "%~dp0"

if /I "%1"=="clean" (
    if exist build (
        echo [Cleaning build directory...]
        rmdir /s /q build
        echo Clean done.
    ) else (
        echo Nothing to clean.
    )
    exit /b 0
)

REM Auto-clean if previous generator was different (e.g. Unix Makefiles from Git Bash)
if exist build\CMakeCache.txt (
    findstr /C:"CMAKE_GENERATOR:INTERNAL=Unix Makefiles" build\CMakeCache.txt >nul 2>&1
    if not errorlevel 1 (
        echo [Detected previous Unix Makefiles generator, cleaning build directory...]
        rmdir /s /q build
    )
)

if not exist build mkdir build
cd build

echo [Configuring with CMake...]
if exist CMakeCache.txt (
    cmake ..
) else (
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake -G "MinGW Makefiles"
)
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

echo [Building...]
cmake --build . --parallel
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo.
echo Build successful! Artifacts in: %cd%
cd ..
