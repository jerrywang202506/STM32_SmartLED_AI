@par Example Description

This example demonstrates GPIO control of the four user LEDs on the
STM32F4-Discovery board using FreeRTOS. A periodic 10 ms task drives
multiple LED chasing / effects patterns. Pressing the USER button
cycles through nine different visual effects.

New in this enhanced version: the on-board MEMS accelerometer
(LIS302DL or LIS3DSH) is used to detect board tilt and motion gestures,
adding three interactive LED modes and shake-to-switch support.

@note On STM32F4-Discovery the LEDs are mapped as:
  - LED4 (red)    on PD12  (left)
  - LED3 (green)  on PD13  (center-left)
  - LED5 (orange) on PD14  (center-right)
  - LED6 (blue)   on PD15  (right)

@par LED Effects (cycle with USER button or SHAKE board)

  1. Chasing         : Single LED flows left-to-right (100 ms step)
  2. Dual Chase      : Two LEDs chase with a 2-position gap (100 ms step)
  3. Blink All       : All four LEDs blink together (500 ms toggle)
  4. Knight Rider    : Scanning back-and-forth like a scanner (80 ms step)
  5. Random Sparkle  : Random LEDs light up unpredictably (150 ms update)
  6. Binary Counter  : 4-bit binary count displayed on LEDs (200 ms step)
  7. Tilt Level      : Bubble-level effect; tilt board left/right to move the
                       "bubble" (50 ms update)
  8. Motion Chase    : Tilt direction controls chase flow; steeper tilt = faster
                       speed. Hold level to pause. (10 ms update)
  9. Tilt Ball       : Virtual ball rolls under gravity with bounce physics.
                       Tilt board to accelerate; friction slows it down.

@par Accelerometer Features

  - Tilt sensing     : X/Y axes sampled at 20 Hz to determine board attitude.
  - Shake detect     : A sharp motion (e.g. shaking the board) triggers a
                       mode switch without pressing the button.
                       Cool-down period prevents accidental double-switches.
  - Auto calibration : Random seed for sparkle effects is generated from
                       accelerometer noise on startup.

@par FreeRTOS Configuration

  - Tick rate                  : 1 kHz
  - LED Effects task period    : 10 ms (osDelay)
  - Accelerometer task period  : 50 ms (osDelay)
  - Button task priority       : Above Normal
  - Minimal stack size         : 128 words
  - Total heap                 : 8 Kbytes

@par Directory contents

  - Inc/main.h                  : Main application header
  - Inc/stm32f4xx_hal_conf.h    : HAL configuration
  - Inc/stm32f4xx_it.h          : Interrupt handler prototypes
  - Inc/FreeRTOSConfig.h        : FreeRTOS configuration
  - Src/main.c                  : Main program with LED effects, button,
                                  and accelerometer tasks
  - Src/stm32f4xx_it.c          : Interrupt service routines
  - Src/system_stm32f4xx.c      : System clock configuration
  - CMakeLists.txt              : CMake build script
  - cmake/arm-none-eabi.cmake   : Cross-compiler toolchain file
  - build.sh / build.bat        : Convenience build wrappers
  - STM32CubeIDE/               : STM32CubeIDE project files

@par Hardware and Software environment

  - This example runs on STM32F407VG devices.
  - This example has been tested with STMicroelectronics STM32F4-Discovery
    boards and can be easily tailored to any other supported device
    and development board.
  - Build tools: arm-none-eabi-gcc + CMake 3.20+

@par How to build with CMake (command line)

  1. Using the provided wrapper script (recommended):
     > Linux / Git Bash:  ./build.sh
     > Windows CMD:       build.bat

  2. Manual CMake steps:
     > mkdir build && cd build
     > cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake -G "Unix Makefiles"
     > cmake --build . --parallel

  Build artifacts (elf / hex / bin / map) will be placed in the build/ directory.

@par How to flash (optional, requires OpenOCD)

  After building:
  > cd build
  > cmake --build . --target flash

@par How to use it with STM32CubeIDE

  - File -> Open Projects from File System...
  - Select the STM32CubeIDE folder inside this example
  - Build and Debug (Ctrl+B then F11)
  - Observe the LED patterns on the Discovery board
  - Press the blue USER button (PA0) to switch to the next effect
  - OR shake the board firmly to switch modes hands-free
  - In Tilt Level / Motion Chase / Tilt Ball modes, tilt the board
    left or right to interact with the LEDs
