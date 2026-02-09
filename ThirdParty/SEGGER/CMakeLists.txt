# SEGGER RTT and SystemView CMake configuration
# Provides modular libraries that can be linked to different projects

cmake_minimum_required(VERSION 3.22)

# ============================================================================
# SEGGER RTT Library (basic RTT functionality)
# ============================================================================
add_library(segger_rtt STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER/SEGGER_RTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER/SEGGER_RTT_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER/SEGGER_RTT_ASM_ARMv7M.S
)

target_include_directories(segger_rtt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
)

# ============================================================================
# SEGGER RTT Syscalls (optional - for printf redirection via RTT)
# ============================================================================
add_library(segger_rtt_syscalls STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c
)

target_link_libraries(segger_rtt_syscalls PUBLIC segger_rtt)

# ============================================================================
# SEGGER SystemView Library (includes RTT + SystemView core)
# ============================================================================
add_library(segger_sysview STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER/SEGGER_SYSVIEW.c
)

target_include_directories(segger_sysview PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SEGGER
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/OS
)

target_link_libraries(segger_sysview PUBLIC segger_rtt)

# ============================================================================
# SEGGER SystemView FreeRTOS Integration
# Must be added AFTER freertos_kernel is defined
# ============================================================================
function(add_segger_sysview_freertos)
    add_library(segger_sysview_freertos STATIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/OS/SEGGER_SYSVIEW_FreeRTOS.c
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c
    )

    target_include_directories(segger_sysview_freertos PUBLIC
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/OS
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/Config
    )

    target_link_libraries(segger_sysview_freertos PUBLIC
        segger_sysview
        freertos_kernel
    )

    target_compile_options(segger_sysview_freertos PRIVATE
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wall>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wextra>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-conversion>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-sign-conversion>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-unused-parameter>
    )
endfunction()

# ============================================================================
# Compiler options for core SEGGER libraries
# ============================================================================
set(SEGGER_CORE_TARGETS segger_rtt segger_rtt_syscalls segger_sysview)

foreach(target ${SEGGER_CORE_TARGETS})
    target_compile_options(${target} PRIVATE
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wall>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wextra>
        # Disable some warnings for third-party code
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-conversion>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-sign-conversion>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wno-unused-parameter>
    )
endforeach()
