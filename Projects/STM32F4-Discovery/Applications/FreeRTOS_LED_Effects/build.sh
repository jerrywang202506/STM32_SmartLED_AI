#!/bin/bash
# Build script for FreeRTOS_LED_Effects using CMake

set -e

cd "$(dirname "$0")"

# Auto-clean if previous generator was different (e.g. MinGW Makefiles from CMD)
if [ -f build/CMakeCache.txt ]; then
    if grep -q "CMAKE_GENERATOR:INTERNAL=MinGW Makefiles" build/CMakeCache.txt 2>/dev/null; then
        echo "[Detected previous MinGW Makefiles generator, cleaning build directory...]"
        rm -rf build
    fi
fi

mkdir -p build
cd build

echo "[Configuring with CMake...]"
if [ -f CMakeCache.txt ]; then
    cmake ..
else
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake -G "Unix Makefiles"
fi

echo "[Building...]"
cmake --build . --parallel

echo ""
echo "Build successful! Artifacts in: $(pwd)"
cd ..
