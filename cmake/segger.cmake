# SEGGER is a tool to help debug FreeRTOS this file creates function to add FreeRTOS and SEGGER for project

# Usage:
# include(${CMAKE_SOURCE_DIR}/../cmake/segger.cmake)

# Including FreeRTOS
include(${CMAKE_CURRENT_LIST_DIR}/freertos.cmake)

# Relative path to SEGGER lib
set(SEGGER_PATH ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/SEGGER)

# Add SEGGER RTT and SystemView BEFORE FreeRTOS (core libs only)
add_subdirectory(${SEGGER_PATH} SEGGER)

# Function to include freertos and segger
function (add_freertos_segger_library TARGET_NAME CONFIG_DIR)

    # Include FreeRTOSConfig.h
    target_include_directories(freertos_config INTERFACE
        ${SEGGER_PATH}/Config
        ${SEGGER_PATH}/OS
        ${SEGGER_PATH}/SEGGER
        ${CONFIG_DIR} # Where your FreeRTOSConfig.h lives
    )

    # Add FreeRTOS kernel as subdirectory
    add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

    # Link FreeRTOS kernel to SEGGER SystemView (for tracing support)
    target_link_libraries(freertos_kernel PUBLIC segger_sysview)

    # Now add the FreeRTOS-specific SEGGER integration (after freertos_kernel exists)
    add_segger_sysview_freertos()

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


    # Linking libraries to target
    target_link_libraries(${TARGET_NAME}
        freertos_kernel
        freertos_config
        segger_sysview_freertos
        segger_rtt_syscalls
    )

    # If set to use UART
    if(SEGGER_UART_REC EQUAL 1)
        message(STATUS "SEGGER UART enabled.")
        target_link_libraries(segger_uart PUBLIC stm32cubemx)
        target_compile_definitions(segger_uart PUBLIC SEGGER_UART_REC=1)
        # segger_sysview needs SEGGER_UART_REC to enable SEGGER_SYSVIEW_ON_EVENT_RECORDED
        target_compile_definitions(segger_sysview PUBLIC SEGGER_UART_REC=1)

        target_link_libraries(${TARGET_NAME} segger_uart)
    endif()

    message(STATUS "FreeRTOS and SEGGER library '${TARGET_NAME}' configured:")
    message(STATUS "  - Port: ${FREERTOS_PORT}")
    message(STATUS "  - Heap: ${FREERTOS_HEAP}")
    message(STATUS "  - Config: ${CONFIG_DIR}")

endfunction()