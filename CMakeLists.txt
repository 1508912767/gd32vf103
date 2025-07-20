cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

###########################################################
# Set the project name
set(CMAKE_PROJECT_NAME gd32vf103)

set(APP_ADDR "0x08000000")      # 从0x08000000启动
add_definitions(-DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP -DDOWNLOAD_MODE_STRING=\"FLASHXIP\" -D__IDE_RV_DE_RV_CORE=n203 -DSYSTEM_CLOCK=108000000 -DSYSCLK_USING_HXTAL)

# BAREMETAL_MODE
# FREERTOS_MODE
# THREADX_MODE
set(SYSTEM_MODE BAREMETAL_MODE)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

###########################################################
# Include toolchain file
set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  riscv)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# riscv64-unknown-elf- must be part of path environment
set(TOOLCHAIN_PREFIX                riscv64-unknown-elf-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)
set(CMAKE_OpenOCD                   openocd)


set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU specific flags
set(TARGET_FLAGS "-march=rv32imac -mabi=ilp32 -mtune=nuclei-200-series -mcmodel=medlow -mno-save-restore")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -fno-common")


if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g -gdwarf-2")
endif()
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()
if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
endif()
if(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
endif()

set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")

set(CMAKE_C_LINK_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T \"${CMAKE_SOURCE_DIR}/nuclei_sdk/SoC/gd32vf103/Board/gd32vf103v_rvstar/Source/GCC/gcc_gd32vf103_flashxip.ld\"")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -nostartfiles")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--check-sections -Wl,--no-warn-rwx-segments")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--print-memory-usage")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}  -Wl,--start-group,-lstdc++,-lc_nano,-lgcc,--end-group")
##########################################################

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# must need 
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# 汇编的宏定义
# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    stm32cubemx
    # Add user defined libraries
)

##########################################################
add_library(stm32cubemx INTERFACE)

target_compile_definitions(stm32cubemx INTERFACE 
	# TX_INCLUDE_USER_DEFINE_FILE 
)

target_include_directories(stm32cubemx INTERFACE

    if(SYSTEM_MODE MATCHES BAREMETAL_MODE)
        application/baremetal/
    endif()
    if(SYSTEM_MODE MATCHES FREERTOS_MODE)
        application/freertos/
        nuclei_sdk/OS/FreeRTOS/Source/include
        nuclei_sdk/OS/FreeRTOS/Source/portable
    endif()
    if(SYSTEM_MODE MATCHES THREADX_MODE)
        application/threadx/
        nuclei_sdk/OS/ThreadX/common/inc
        nuclei_sdk/OS/ThreadX/ports/nuclei/
    endif()

    nuclei_sdk/NMSIS/Core/Include/
    nuclei_sdk/SoC/gd32vf103/Board/gd32vf103v_rvstar/Include/
    nuclei_sdk/SoC/gd32vf103/Common/Include/
)

file(GLOB SRC_LIB_0 nuclei_sdk/SoC/gd32vf103/Common/Source/Drivers/*.c)
file(GLOB SRC_LIB_1 nuclei_sdk/SoC/gd32vf103/Common/Source/Stubs/newlib/*.c)
file(GLOB SRC_LIB_2 nuclei_sdk/SoC/gd32vf103/Common/Source/*.c)
file(GLOB SRC_LIB_3 nuclei_sdk/SoC/gd32vf103/Board/gd32vf103v_rvstar/Source/*.c)

if(SYSTEM_MODE MATCHES BAREMETAL_MODE)
    file(GLOB SRC_APP_0 application/baremetal/*.c)
    file(GLOB SRC_APP_1 )
    file(GLOB SRC_APP_2 )
    file(GLOB SRC_APP_3 )
    file(GLOB SRC_APP_4 )
    file(GLOB SRC_APP_5 )
endif()
if(SYSTEM_MODE MATCHES FREERTOS_MODE)
    file(GLOB SRC_APP_0 application/freertos/*.c)
    file(GLOB SRC_APP_1 nuclei_sdk/OS/FreeRTOS/Source/*.c)
    file(GLOB SRC_APP_2 application/freertos/*.c)
    file(GLOB SRC_APP_3 nuclei_sdk/OS/FreeRTOS/Source/portable/*.c)
    file(GLOB SRC_APP_4 nuclei_sdk/OS/FreeRTOS/Source/portable/MemMang/*.c)
    file(GLOB SRC_APP_5 nuclei_sdk/OS/FreeRTOS/Source/portable/GCC/portasm.S)
endif()
if(SYSTEM_MODE MATCHES THREADX_MODE)
    file(GLOB SRC_APP_0 application/threadx/*.c)
    file(GLOB SRC_APP_1 nuclei_sdk/OS/ThreadX/common/src/*.c)
    file(GLOB SRC_APP_2 nuclei_sdk/OS/ThreadX/ports/nuclei/*.c)
    file(GLOB SRC_APP_3 nuclei_sdk/OS/ThreadX/ports/nuclei/gcc/context.S)
    file(GLOB SRC_APP_4 nuclei_sdk/OS/ThreadX/ports/nuclei/gcc/interrupt.S)
    file(GLOB SRC_APP_5 )
endif()

target_sources(stm32cubemx INTERFACE
    ${SRC_APP_0}
    ${SRC_APP_1}
    ${SRC_APP_2}
    ${SRC_APP_3}
    ${SRC_APP_4}
    ${SRC_APP_5}

    ${SRC_LIB_0}
    ${SRC_LIB_1}
    ${SRC_LIB_2}
    ${SRC_LIB_3}

    # GLOB *.S doesn't work
    nuclei_sdk/SoC/gd32vf103/Common/Source/GCC/startup_gd32vf103.S
    nuclei_sdk/SoC/gd32vf103/Common/Source/GCC/intexc_gd32vf103.S
)
##########################################################
# For Download & Debug
set(ELF_FILE ${PROJECT_NAME}.elf)
set(HEX_FILE ${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_NAME}.bin)
set(DFU_FILE ${PROJECT_NAME}.dfu)

# PRE_BUILD（预构建）、PRE_LINK（链接前）和POST_BUILD（构建后）用于指定自定义命令在构建过程中的执行时机。
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary -S ${ELF_FILE} ${BIN_FILE}
    COMMAND ${CMAKE_OBJCOPY} -Oihex  ${ELF_FILE} ${HEX_FILE}
    COMMENT "Building ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"
)

# print size
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf
    COMMENT "Invoking: Cross ARM GNU Print Size"
)

# make openocd_flash
add_custom_target(openocd_flash
	COMMAND ${CMAKE_OpenOCD} -f "${CMAKE_SOURCE_DIR}/nuclei_sdk/SoC/gd32vf103/Board/gd32vf103v_rvstar/openocd_gd32vf103.cfg" -c "program gd32vf103.elf verify reset exit"
	COMMENT "USE OpenOCD to make flash at ${APP_ADDR}"
)
