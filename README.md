# FreeRTOS on STM32F407

[![CMake](https://img.shields.io/badge/CMake-3.22+-blue.svg)](https://cmake.org/)
[![FreeRTOS](https://img.shields.io/badge/FreeRTOS-Kernel-brightgreen.svg)](https://freertos.org/)
[![Platform](https://img.shields.io/badge/board-STM32F407G--DISC1-orange.svg)](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)
[![SEGGER SystemView](https://img.shields.io/badge/debug-SEGGER%20SystemView-lightgrey.svg)](https://www.segger.com/downloads/systemview/)
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](LICENSE)

A hands-on FreeRTOS learning project for the **STM32F407G-DISC1** (Cortex-M4F, 168 MHz), covering fundamental RTOS concepts through progressively more complex examples. Each project is self-contained and demonstrates a specific FreeRTOS feature, with **SEGGER SystemView** integration for real-time task scheduling visualization and debugging.

---

## Features

- Six self-contained example projects covering core FreeRTOS concepts
- Shared CMake modules — add FreeRTOS or FreeRTOS + SEGGER to any project in two lines
- SEGGER SystemView tracing with two recording modes: single-shot (no extra hardware) and UART streaming (real-time)
- Supports both CLion and STM32CubeIDE
- FreeRTOS-Kernel as a Git submodule (easy version pinning and upgrades)
- Strict compiler warnings on kernel code (`-Wall -Wextra -Wpedantic -Werror -Wconversion`)

---

## Projects

Each numbered directory is an independent project. They build on each other conceptually:

| # | Project | FreeRTOS Concept |
|---|---------|-----------------|
| 001 | `001_Tasks` | Task creation and preemptive scheduling |
| 002 | `002_LED_Tasks` | Multiple concurrent tasks controlling LEDs |
| 003 | `003_ISR_Notify` | Task notification from an interrupt (`vTaskNotifyGiveFromISR`) |
| 004 | `004_Queue_RTC_LED` | Inter-task communication with queues, RTC timestamps, and LED effects |
| 005 | `005_Binary_Semaphore_Tasks` | Producer/consumer synchronization with binary semaphores |
| 006 | `006_Mutex` | Shared resource protection with mutexes (race condition prevention) |

---

## Prerequisites

| Tool | Version | Purpose |
|------|---------|---------|
| [arm-none-eabi-gcc](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) | GCC 11+ | ARM cross-compiler |
| [CMake](https://cmake.org/) | 3.22+ | Build system |
| [Ninja](https://ninja-build.org/) | Any | Build generator (used by presets) |
| [CLion](https://www.jetbrains.com/clion/) or [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) | — | IDE (either works) |
| [SEGGER SystemView](https://www.segger.com/downloads/systemview/) | — | Trace analysis (optional) |

---

## Getting Started

```bash
git clone git@github.com:hugo-juarez/FreeRTOS-Projects.git
cd FreeRTOS-Projects
git submodule update --init --recursive   # pulls FreeRTOS-Kernel
```

> The FreeRTOS-Kernel submodule must be initialized before any project will build.

---

## Building

Each project lives in its own directory. Navigate into a project before building.

### CLion (Recommended)

Open a project directory (e.g. `001_Tasks/`) as a CMake project. CLion will automatically pick up `CMakePresets.json` and configure the ARM toolchain — no manual setup needed.

### Command Line

```bash
cd 001_Tasks
cmake --preset Debug          # configure (outputs to build/Debug/)
cmake --build --preset Debug  # compile
```

Use `--preset Release` for an optimized build (`-Os`).

### STM32CubeIDE

1. Open STM32CubeIDE and set up a workspace
2. **File > Open Projects from File System** — set the project path (e.g. `001_Tasks/`) as source
3. Set `<project>/build-cubeide` as the build directory

---

## Project Structure

```
FreeRTOS/
├── 001_Tasks/                       # Self-contained project
│   ├── Core/
│   │   ├── Inc/                     # FreeRTOSConfig.h, main.h, HAL config
│   │   └── Src/                     # main.c, syscalls.c, interrupt handlers
│   ├── Drivers/                     # STM32 HAL + CMSIS (per-project copy)
│   ├── cmake/
│   │   ├── stm32cubemx/             # CubeMX-generated CMake (HAL sources, flags)
│   │   ├── gcc-arm-none-eabi.cmake  # GCC cross-compiler toolchain
│   │   └── starm-clang.cmake        # Clang cross-compiler toolchain (alternative)
│   ├── CMakeLists.txt
│   ├── CMakePresets.json            # Debug / Release presets (Ninja)
│   ├── STM32F407XX_FLASH.ld         # Linker script
│   └── startup_stm32f407xx.s        # Startup assembly
├── cmake/
│   ├── freertos.cmake               # FreeRTOS-only CMake module
│   └── segger.cmake                 # FreeRTOS + SEGGER CMake module
└── ThirdParty/
    ├── FreeRTOS-Kernel/             # Git submodule (port: GCC_ARM_CM4F, heap: 4)
    └── SEGGER/                      # RTT + SystemView sources (not a submodule)
```

---

## Adding FreeRTOS to a New Project

1. Create a project directory following the structure above (copy an existing project as a template)
2. Add `FreeRTOSConfig.h` to `Core/Inc/` — use a template from the [official FreeRTOS demos](https://github.com/FreeRTOS/FreeRTOS/tree/main/FreeRTOS/Demo)
3. Add to the end of your `CMakeLists.txt`:

**Without SEGGER:**
```cmake
include(${CMAKE_SOURCE_DIR}/../cmake/freertos.cmake)
add_freertos_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

**With SEGGER** (replaces the above — not additive):
```cmake
include(${CMAKE_SOURCE_DIR}/../cmake/segger.cmake)
add_freertos_segger_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

### Adapting for a Different MCU

1. Update `FREERTOS_PORT` in `cmake/freertos.cmake` (e.g. `GCC_ARM_CM3`, `GCC_ARM_CM0`)
2. Update `TARGET_FLAGS` in `cmake/gcc-arm-none-eabi.cmake` for your CPU/FPU
3. Replace the linker script and startup file

### Updating the FreeRTOS Version

```bash
cd ThirdParty/FreeRTOS-Kernel
git fetch && git checkout <desired-tag>
cd ../..
git add ThirdParty/FreeRTOS-Kernel
git commit -m "Update FreeRTOS-Kernel to <version>"
```

---

## SEGGER SystemView

[SEGGER SystemView](https://www.segger.com/downloads/systemview/) lets you visualize FreeRTOS task scheduling, CPU load, and inter-task events in real time. RTT and SystemView sources are included in `ThirdParty/SEGGER/`.

### FreeRTOSConfig.h Setup

Add these macros:

```c
#define configUSE_TRACE_FACILITY        1
#define INCLUDE_xTaskGetIdleTaskHandle  1
#define INCLUDE_pxTaskGetStackStart     1
```

At the **bottom** of the file:

```c
#include "SEGGER_SYSVIEW_FreeRTOS.h"
```

### main.c Initialization

```c
// After SystemClock_Config() — enables cycle counter required by SystemView
DWT->CTRL |= (1 << 0);

// Before creating any tasks
SEGGER_SYSVIEW_Conf();
SEGGER_SYSVIEW_Start();
```

In `HAL_MspInit()`:

```c
NVIC_SetPriorityGrouping(0);
```

> Interrupt priorities: ISRs that call FreeRTOS API must have priority ≥ `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (5).

### Recording Mode 1 — Single-Shot (no extra hardware)

Captures a trace buffer snapshot after a debug session. No extra hardware required.

1. Pause execution in the debugger
2. Evaluate `_SEGGER_RTT.aUp[1].pBuffer` (buffer address) and `_SEGGER_RTT.aUp[1].WrOff` (byte count)
3. Open the Memory Browser, export that range as a raw binary file with a `.SVdat` extension
4. In SystemView: **File > Load Data**

### Recording Mode 2 — UART Streaming (real-time)

Streams trace data live over USART2 (PA2/TX, PA3/RX). Requires a USB-to-UART adapter.

> **Note:** `ThirdParty/SEGGER/Rec/segger_uart.c` is STM32F407-specific (USART2 on APB1 at 42 MHz). Modify it for other MCUs.

**CMakeLists.txt** — add before `include(segger.cmake)`:

```cmake
set(SEGGER_UART_REC 1)
include(${CMAKE_SOURCE_DIR}/../cmake/segger.cmake)
add_freertos_segger_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

**main.c** — replace `SEGGER_SYSVIEW_Start()` with:

```c
extern void SEGGER_UART_init(U32 baud);
SEGGER_UART_init(500000);
SEGGER_SYSVIEW_Conf();
// Do NOT call SEGGER_SYSVIEW_Start() — the host triggers it
```

**Host:** In SystemView go to **Target > Recorder Configuration > UART**, set the baud rate, then click **Start Recording**.

> The UART IRQ runs at priority 6 — below `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (5) — so it does not interfere with FreeRTOS. Single-shot and UART recording are mutually exclusive.

---

## Printf Output: SWO vs SEGGER RTT

When `segger_rtt_syscalls` is linked (the default with `add_freertos_segger_library`), `printf()` is routed to SEGGER RTT — **nothing appears on SWO**.

| Mode | How to use |
|------|-----------|
| SEGGER RTT | `SEGGER_SYSVIEW_PrintfTarget("msg")` (visible in SystemView), or J-Link RTT Viewer for raw `printf` |
| SWO | Remove `segger_rtt_syscalls` from link libraries in `cmake/segger.cmake` |

---

## Resources

- [FreeRTOS Documentation](https://www.freertos.org/)
- [FreeRTOS-Kernel on GitHub](https://github.com/FreeRTOS/FreeRTOS-Kernel)
- [SEGGER SystemView Download](https://www.segger.com/downloads/systemview/)
- [SEGGER SystemView on GitHub](https://github.com/SEGGERMicro/SystemView)
- [SEGGER RTT on GitHub](https://github.com/SEGGERMicro/RTT)
- [STM32F407G-DISC1 Product Page](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)

---

## License

The original source code in this repository (project `Core/` directories and `cmake/` modules) is licensed under the [MIT License](LICENSE).

Third-party components retain their own licenses:

| Component | License | Location |
|-----------|---------|----------|
| FreeRTOS-Kernel | MIT | `ThirdParty/FreeRTOS-Kernel/LICENSE.md` |
| CMSIS | Apache 2.0 | `Drivers/CMSIS/LICENSE.txt` |
| STM32 HAL Driver | BSD-3-Clause | `Drivers/STM32F4xx_HAL_Driver/LICENSE.txt` |
| SEGGER RTT / SystemView | BSD-1-Clause | File headers in `ThirdParty/SEGGER/` |
