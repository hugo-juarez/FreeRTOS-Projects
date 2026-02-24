# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

STM32F407VGT6 (Cortex-M4F, 168 MHz) FreeRTOS learning project with SEGGER SystemView integration. Target board: STM32F407G-DISC1. Designed for CLion (primary) and STM32CubeIDE.

## Build Commands

```bash
# FreeRTOS submodule init (required before first build)
git submodule update --init --recursive

# Build a project using CMakePresets.json (preferred, from the project directory)
cmake --preset Debug      # configures into build/Debug/
cmake --build --preset Debug

# Alternative: explicit toolchain (no presets)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
cmake --build build

# For STM32CubeIDE: use project dir as source, project/build-cubeide as build dir
```

## Architecture

```
FreeRTOS/
├── 001_Tasks/           # Individual project (each project is self-contained)
│   ├── CMakeLists.txt   # Links stm32cubemx + FreeRTOS/SEGGER via shared cmake
│   ├── Core/Inc/        # FreeRTOSConfig.h, main.h, HAL config
│   ├── Core/Src/        # main.c, syscalls.c, interrupts
│   ├── Drivers/         # STM32 HAL + CMSIS (per-project copy)
│   ├── cmake/stm32cubemx/        # CubeMX-generated cmake (HAL sources, flags)
│   ├── cmake/gcc-arm-none-eabi.cmake  # GCC cross-compiler toolchain
│   ├── cmake/starm-clang.cmake        # Clang cross-compiler toolchain (alternative)
│   ├── CMakePresets.json              # Debug/Release presets (Ninja generator)
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

### SEGGER SystemView UART recording (real-time streaming)

An alternative to single-shot that streams data live over USART2 (PA2/TX, PA3/RX).

In `CMakeLists.txt`, set before including `segger.cmake`:
```cmake
set(SEGGER_UART_REC 1)
include(${CMAKE_SOURCE_DIR}/../cmake/segger.cmake)
add_freertos_segger_library(...)
```

In `main.c`, replace `SEGGER_SYSVIEW_Start()` with:
```c
extern void SEGGER_UART_init(U32 baud);
SEGGER_UART_init(500000);
SEGGER_SYSVIEW_Conf();
// Do NOT call SEGGER_SYSVIEW_Start() — the host triggers it
```

Connect a USB-UART adapter to PA2/PA3, then in SystemView: Target > Recorder Configuration > UART, set baud rate, click Start Recording.

Notes:
- UART IRQ runs at priority 6, which is intentionally below `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (5)
- `ThirdParty/SEGGER/Rec/segger_uart.c` is STM32F407-specific (USART2 on APB1 at 42 MHz); modify for other MCUs
- Single-shot and UART recording are mutually exclusive

### SEGGER configuration files

- `ThirdParty/SEGGER/Config/SEGGER_SYSVIEW_ConfDefault.h` — RTT buffer size, MCU core
- `ThirdParty/SEGGER/Sample/FreeRTOSV10/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c` — app name (`SYSVIEW_APP_NAME`) and device string (`SYSVIEW_DEVICE_NAME`)

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
