# FreeRTOS on STM32F407

Project for FreeRTOS on the STM32F407G-DISC1 board with SEGGER SystemView integration for real-time debugging and profiling.

## Prerequisites

- [arm-none-eabi-gcc](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) (GCC 11+)
- [CMake](https://cmake.org/) 3.22+
- [SEGGER SystemView](https://www.segger.com/downloads/systemview/) (for trace analysis)
- [CLion](https://www.jetbrains.com/clion/) or [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)

## Installation

```bash
git clone <repo-url>
cd FreeRTOS
git submodule update --init --recursive
```

## Building

### CLion

Open the project directory (e.g. `001_Tasks/`) as a CMake project. CLion should detect the toolchain from `cmake/gcc-arm-none-eabi.cmake` automatically via `CMakePresets.json`.

### STM32CubeIDE

1. Set up an STM32CubeIDE workspace (does not need to be in this root folder)
2. Select the project path as source directory (e.g. `001_Tasks/`)
3. Select `<project>/build-cubeide` as the build directory

### Command Line

```bash
cd 001_Tasks
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
cmake --build build
```

## Project Structure

```
FreeRTOS/
├── 001_Tasks/                  # Example project: FreeRTOS tasks
│   ├── CMakeLists.txt          # Project build configuration
│   ├── Core/
│   │   ├── Inc/                # FreeRTOSConfig.h, main.h, HAL config
│   │   └── Src/                # main.c, syscalls.c, interrupts
│   ├── Drivers/                # STM32 HAL + CMSIS
│   ├── cmake/
│   │   ├── stm32cubemx/        # CubeMX-generated CMake config
│   │   └── gcc-arm-none-eabi.cmake  # ARM cross-compiler toolchain
│   ├── STM32F407XX_FLASH.ld    # Linker script
│   └── startup_stm32f407xx.s   # Startup assembly
├── cmake/
│   ├── freertos.cmake          # FreeRTOS-only CMake module
│   └── segger.cmake            # FreeRTOS + SEGGER CMake module
└── ThirdParty/
    ├── FreeRTOS-Kernel/        # Git submodule
    └── SEGGER/                 # RTT + SystemView sources
```

Each numbered directory (e.g. `001_Tasks/`) is a self-contained project with its own `CMakeLists.txt`, drivers, and configuration. Shared CMake modules in `cmake/` and third-party sources in `ThirdParty/` are referenced via relative paths.

## Adding FreeRTOS to a New Project

1. Create a new project directory with `Core/Inc/`, `Core/Src/`, and `Drivers/` following the same structure as `001_Tasks/`

2. Add a `FreeRTOSConfig.h` in `Core/Inc/`. Templates are available from the [official FreeRTOS demos](https://github.com/FreeRTOS/FreeRTOS/tree/main/FreeRTOS/Demo)

3. Add to the end of your `CMakeLists.txt`:

**Without SEGGER:**
```cmake
include(${CMAKE_SOURCE_DIR}/../cmake/freertos.cmake)
add_freertos_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

**With SEGGER (replaces the above, not additive):**
```cmake
include(${CMAKE_SOURCE_DIR}/../cmake/segger.cmake)
add_freertos_segger_library(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Core/Inc)
```

To modify the heap implementation or FreeRTOS port for a different MCU, edit `cmake/freertos.cmake`.

### Updating the FreeRTOS Version

```bash
cd ThirdParty/FreeRTOS-Kernel
git fetch
git checkout <desired-tag-or-branch>
cd ../..
git add ThirdParty/FreeRTOS-Kernel
git commit -m "Update FreeRTOS-Kernel to <version>"
```

### Using a Different MCU

1. Update `FREERTOS_PORT` in `cmake/freertos.cmake` to match your core (e.g. `GCC_ARM_CM3`, `GCC_ARM_CM0`)
2. Update `TARGET_FLAGS` in `cmake/gcc-arm-none-eabi.cmake` for your CPU/FPU
3. Replace the linker script and startup file
4. If the portable directory doesn't include your port, get it from the [FreeRTOS-Kernel repo](https://github.com/FreeRTOS/FreeRTOS-Kernel)

## SEGGER SystemView Setup

This project uses [SEGGER SystemView](https://www.segger.com/downloads/systemview/) for FreeRTOS debugging and profiling. The RTT and SystemView sources are included in `ThirdParty/SEGGER/`. To get updated versions, see [SEGGER SystemView](https://github.com/SEGGERMicro/SystemView) and [SEGGER RTT](https://github.com/SEGGERMicro/RTT).

### FreeRTOSConfig.h Requirements

Add the following macros to your `FreeRTOSConfig.h`:

```c
#define INCLUDE_xTaskGetIdleTaskHandle  1
#define INCLUDE_pxTaskGetStackStart     1
```

And at the **bottom** of the file:

```c
#include "SEGGER_SYSVIEW_FreeRTOS.h"
```

To stop using SEGGER, remove the macros and include above from `FreeRTOSConfig.h`.

### main.c Initialization

After `SystemClock_Config()`, enable the DWT cycle counter (required for ARM Cortex-M3/M4):

```c
DWT->CTRL |= (1 << 0);
```

Before FreeRTOS task creation:

```c
SEGGER_SYSVIEW_Conf();
SEGGER_SYSVIEW_Start();
```

In `HAL_MspInit()`:

```c
NVIC_SetPriorityGrouping(0);
```

### SEGGER Configuration Files

In `SEGGER_SYSVIEW_ConfDefault.h`, update for your MCU if needed:

```c
#define SEGGER_SYSVIEW_CORE             SEGGER_SYSVIEW_CORE_YOUR_MCU
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE  1024
```

In `SEGGER_SYSVIEW_Config_FreeRTOS.c`, update the application name and device:

```c
#define SYSVIEW_APP_NAME    "FreeRTOS Projects Application"
#define SYSVIEW_DEVICE_NAME "STM32F407-DISC-1"
```

### Single-Shot Recording

1. Get the RTT buffer address and byte count by evaluating:
   - `_SEGGER_RTT.aUp[1].pBuffer` (buffer address)
   - `_SEGGER_RTT.aUp[1].WrOff` (number of bytes written)
2. Open the Memory Browser in STM32CubeIDE or CLion
3. Export the memory from the buffer address for the byte count length
4. Save as a raw binary file with a `.SVdat` extension
5. In SystemView: **File > Load Data** and select the file

## Printf Output: SWO vs SEGGER RTT

When `segger_rtt_syscalls` is linked (default with `add_freertos_segger_library`), `printf()` output is routed to SEGGER RTT. **No output will appear on SWO.**

To see printf output with SEGGER enabled, use:

```c
SEGGER_SYSVIEW_PrintfTarget("Hello from Task-1");
```

If you need SWO output instead, remove `segger_rtt_syscalls` from the link libraries in `cmake/segger.cmake` and remove the corresponding calls.

## Resources

- [FreeRTOS Documentation](https://www.freertos.org/)
- [FreeRTOS-Kernel GitHub](https://github.com/FreeRTOS/FreeRTOS-Kernel)
- [SEGGER SystemView Download](https://www.segger.com/downloads/systemview/)
- [SEGGER SystemView GitHub](https://github.com/SEGGERMicro/SystemView)
- [SEGGER RTT GitHub](https://github.com/SEGGERMicro/RTT)
