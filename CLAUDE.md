# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

STM32F407VGT6 (Cortex-M4F, 168 MHz) FreeRTOS learning project with SEGGER SystemView integration. Target board: STM32F407G-DISC1. Designed for CLion (primary) and STM32CubeIDE.

## Build Commands

```bash
# Build a project (from its directory, e.g. 001_Tasks/)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
cmake --build build

# For STM32CubeIDE: use project dir as source, project/build-cubeide as build dir

# FreeRTOS submodule init (required before first build)
git submodule update --init --recursive
```

## Architecture

```
FreeRTOS/
├── 001_Tasks/           # Individual project (each project is self-contained)
│   ├── CMakeLists.txt   # Links stm32cubemx + FreeRTOS/SEGGER via shared cmake
│   ├── Core/Inc/        # FreeRTOSConfig.h, main.h, HAL config
│   ├── Core/Src/        # main.c, syscalls.c, interrupts
│   ├── Drivers/         # STM32 HAL + CMSIS (per-project copy)
│   ├── cmake/stm32cubemx/  # CubeMX-generated cmake (HAL sources, flags)
│   ├── cmake/gcc-arm-none-eabi.cmake  # Cross-compiler toolchain
│   ├── STM32F407XX_FLASH.ld           # Linker script
│   └── startup_stm32f407xx.s          # Startup assembly
├── cmake/
│   ├── freertos.cmake   # FreeRTOS-only integration (add_freertos_library)
│   └── segger.cmake     # FreeRTOS + SEGGER integration (add_freertos_segger_library)
└── ThirdParty/
    ├── FreeRTOS-Kernel/ # Git submodule, port: GCC_ARM_CM4F, heap: 4
    └── SEGGER/          # RTT + SystemView (not a submodule)
```

### Adding FreeRTOS to a new project

Without SEGGER:
```cmake
include(${CMAKE_SOURCE_DIR}/../cmake/freertos.cmake)
add_freertos_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

With SEGGER (replaces the above, not additive):
```cmake
include(${CMAKE_SOURCE_DIR}/../cmake/segger.cmake)
add_freertos_segger_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

### I/O routing: SWO vs SEGGER RTT

`segger.cmake` links `segger_rtt_syscalls`, which provides a **non-weak** `_write()` routing printf to SEGGER RTT. The `syscalls.c` has a **weak** `_write()` routing to ITM/SWO. When both are linked, **RTT wins and SWO shows nothing**.

- To use SWO: remove `segger_rtt_syscalls` from the link libraries in segger.cmake
- To use SEGGER RTT: use `SEGGER_SYSVIEW_PrintfTarget()` for output visible in SystemView, or use J-Link RTT Viewer for raw printf

### SEGGER SystemView single-shot recording

1. Check `_SEGGER_RTT.aUp[1].pBuffer` for buffer address and `_SEGGER_RTT.aUp[1].WrOff` for byte count
2. Memory dump from that address for that length via IDE memory browser
3. Save as raw binary with `.SVdat` extension
4. Open in SEGGER SystemView: File > Load Data

### Key SEGGER init requirements in main.c

```c
DWT->CTRL |= (1 << 0);      // Enable cycle counter (after SystemClock_Config)
SEGGER_SYSVIEW_Conf();        // Before task creation
SEGGER_SYSVIEW_Start();
```

And in HAL_MspInit: `NVIC_SetPriorityGrouping(0);`

## FreeRTOS Configuration Notes

- FreeRTOSConfig.h is per-project in `Core/Inc/`
- SEGGER requires: `configUSE_TRACE_FACILITY 1`, `INCLUDE_xTaskGetIdleTaskHandle 1`, `INCLUDE_pxTaskGetStackStart 1`, and `#include "SEGGER_SYSVIEW_FreeRTOS.h"` at the bottom
- Interrupt priorities: `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` is 5 (ISRs calling FreeRTOS API must have priority >= 5)
- Port is GCC_ARM_CM4F; change in `freertos.cmake` if targeting different MCU

## Toolchain

- Compiler: arm-none-eabi-gcc (GCC 11+), C11
- MCU flags: `-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard`
- Debug: `-O0 -g3`, Release: `-Os -g0`
- Uses nano.specs (newlib-nano)
- Strict warnings on FreeRTOS kernel: `-Wall -Wextra -Wpedantic -Werror -Wconversion`
