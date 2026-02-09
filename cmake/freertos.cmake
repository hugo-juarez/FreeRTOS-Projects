# Usage:
# include(${CMAKE_SOURCE_DIR}/../cmake/freertos.cmake)

# Relative path to FreeRTOS-Kernel
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/FreeRTOS-Kernel)

# Verify submodule is initialized
if(NOT EXISTS ${FREERTOS_KERNEL_PATH}/tasks.c)
    message(FATAL_ERROR 
        "FreeRTOS submodule not initialized!\n"
        "Run: git submodule update --init --recursive")
endif()

# Set the port BEFORE adding the FreeRTOS subdirectory
set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")  # Adjust for your MCU

# Set heap implementation (1-5, or path to custom)
set(FREERTOS_HEAP 4 CACHE STRING "")

# Config library for FreeRTOS
add_library(freertos_config INTERFACE)

# Create the freertos_config target
function (add_freertos_library TARGET_NAME CONFIG_DIR)
    
    # Include FreeRTOSConfig.h
    target_include_directories(freertos_config
        INTERFACE
        ${CONFIG_DIR} # Where your FreeRTOSConfig.h lives
    )

    # Add FreeRTOS kernel as subdirectory
    add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

    target_compile_options(freertos_kernel PRIVATE
        ### Gnu/Clang C Options
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcolor-diagnostics>

        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wextra>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wpedantic>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Werror>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wconversion>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Weverything>

        # Suppressions required to build clean with clang.
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-unused-macros>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-padded>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-missing-variable-declarations>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-covered-switch-default>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-cast-align> )

    # Add static libraries for FreeRTOS
    target_link_libraries(${TARGET_NAME}
        freertos_kernel
        freertos_config
    )

    message(STATUS "FreeRTOS library '${TARGET_NAME}' configured:")
    message(STATUS "  - Port: ${FREERTOS_PORT}")
    message(STATUS "  - Heap: ${FREERTOS_HEAP}")
    message(STATUS "  - Config: ${CONFIG_DIR}")

endfunction()