cmake_minimum_required(VERSION 3.20)

# Prevent in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use a separate build directory.")
endif()

# Project definition
project(FreeRTOS_LED_Effects
    LANGUAGES C ASM
    VERSION 1.0.0
)

# ------------------------------------------------------------------------------
# MCU Configuration
# ------------------------------------------------------------------------------
set(MCU_FAMILY    STM32F4xx)
set(MCU_MODEL     STM32F407xx)
set(MCU_CORE      cortex-m4)
set(MCU_FPU       fpv4-sp-d16)
set(MCU_FLOAT_ABI hard)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32CubeIDE/STM32F407VGTX_FLASH.ld)

# ------------------------------------------------------------------------------
# Directories (relative to project root)
# ------------------------------------------------------------------------------
set(PROJ_ROOT     ${CMAKE_CURRENT_SOURCE_DIR})
set(HAL_DRV_SRC   ${PROJ_ROOT}/../../../../Drivers/STM32F4xx_HAL_Driver/Src)
set(HAL_DRV_INC   ${PROJ_ROOT}/../../../../Drivers/STM32F4xx_HAL_Driver/Inc)
set(CMSIS_DEV_INC ${PROJ_ROOT}/../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include)
set(CMSIS_CORE_INC ${PROJ_ROOT}/../../../../Drivers/CMSIS/Include)
set(BSP_SRC       ${PROJ_ROOT}/../../../../Drivers/BSP/STM32F4-Discovery)
set(BSP_INC       ${PROJ_ROOT}/../../../../Drivers/BSP/STM32F4-Discovery)
set(BSP_COMP_INC  ${PROJ_ROOT}/../../../../Drivers/BSP/Components/Common)
set(LIS302DL_INC  ${PROJ_ROOT}/../../../../Drivers/BSP/Components/lis302dl)
set(LIS3DSH_INC   ${PROJ_ROOT}/../../../../Drivers/BSP/Components/lis3dsh)
set(FRTOS_SRC     ${PROJ_ROOT}/../../../../Middlewares/Third_Party/FreeRTOS/Source)
set(FRTOS_PORT    ${PROJ_ROOT}/../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F)
set(FRTOS_INC     ${PROJ_ROOT}/../../../../Middlewares/Third_Party/FreeRTOS/Source/include)
set(FRTOS_CMIS    ${PROJ_ROOT}/../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS)

# ------------------------------------------------------------------------------
# Include directories
# ------------------------------------------------------------------------------
set(PROJECT_INCLUDES
    ${PROJ_ROOT}/Inc
    ${HAL_DRV_INC}
    ${CMSIS_DEV_INC}
    ${CMSIS_CORE_INC}
    ${BSP_INC}
    ${BSP_COMP_INC}
    ${LIS302DL_INC}
    ${LIS3DSH_INC}
    ${FRTOS_PORT}
    ${FRTOS_INC}
    ${FRTOS_CMIS}
)

# ------------------------------------------------------------------------------
# Global compile definitions
# ------------------------------------------------------------------------------
add_compile_definitions(
    USE_HAL_DRIVER
    ${MCU_MODEL}
    USE_STM32F4_DISCO
)

# ------------------------------------------------------------------------------
# Global compile options
# ------------------------------------------------------------------------------
add_compile_options(
    -mcpu=${MCU_CORE}
    -mthumb
    -mfpu=${MCU_FPU}
    -mfloat-abi=${MCU_FLOAT_ABI}
    -Wall
    -fdata-sections
    -ffunction-sections
    -g
    -O0
)

# ------------------------------------------------------------------------------
# Global link options
# ------------------------------------------------------------------------------
add_link_options(
    -mcpu=${MCU_CORE}
    -mthumb
    -mfpu=${MCU_FPU}
    -mfloat-abi=${MCU_FLOAT_ABI}
    -T${MCU_LINKER_SCRIPT}
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -specs=nano.specs
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
)

# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------
set(PROJ_SOURCES
    ${PROJ_ROOT}/Src/main.c
    ${PROJ_ROOT}/Src/stm32f4xx_it.c
    ${PROJ_ROOT}/Src/system_stm32f4xx.c
)

set(HAL_SOURCES
    ${HAL_DRV_SRC}/stm32f4xx_hal.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_cortex.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_dma.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_flash.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_flash_ex.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_gpio.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_pwr.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_pwr_ex.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_rcc.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_rcc_ex.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_i2c.c
    ${HAL_DRV_SRC}/stm32f4xx_hal_spi.c
)

set(BSP_SOURCES
    ${BSP_SRC}/stm32f4_discovery.c
    ${BSP_SRC}/stm32f4_discovery_accelerometer.c
    ${BSP_SRC}/../Components/lis302dl/lis302dl.c
    ${BSP_SRC}/../Components/lis3dsh/lis3dsh.c
)

set(FRTOS_SOURCES
    ${FRTOS_SRC}/list.c
    ${FRTOS_SRC}/queue.c
    ${FRTOS_SRC}/tasks.c
    ${FRTOS_SRC}/timers.c
    ${FRTOS_CMIS}/cmsis_os.c
    ${FRTOS_SRC}/portable/MemMang/heap_4.c
    ${FRTOS_PORT}/port.c
)

set(STARTUP_SOURCES
    ${PROJ_ROOT}/STM32CubeIDE/Application/Startup/startup_stm32f407vgtx.s
)

# ------------------------------------------------------------------------------
# Executable target
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    ${PROJ_SOURCES}
    ${HAL_SOURCES}
    ${BSP_SOURCES}
    ${FRTOS_SOURCES}
    ${STARTUP_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDES})

# Set startup file properties (assembly)
set_source_files_properties(${STARTUP_SOURCES} PROPERTIES LANGUAGE ASM)

# ------------------------------------------------------------------------------
# Post-build commands: HEX, BIN, and size summary
# ------------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating HEX/BIN and printing size..."
)

# ------------------------------------------------------------------------------
# Flash target (optional, using OpenOCD if available)
# ------------------------------------------------------------------------------
find_program(OPENOCD openocd)
if(OPENOCD)
    add_custom_target(flash
        COMMAND ${OPENOCD}
            -f interface/stlink.cfg
            -f target/stm32f4x.cfg
            -c "program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit"
        DEPENDS ${PROJECT_NAME}
        COMMENT "Flashing target via OpenOCD..."
    )
else()
    add_custom_target(flash
        COMMAND ${CMAKE_COMMAND} -E echo "OpenOCD not found. Install it to enable 'make flash'."
    )
endif()
