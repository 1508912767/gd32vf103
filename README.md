# 基于 Cmake 的 GD32VF103 工程
This is a project for gd32vf103 which has a RISC-V CPU core, the different between this projetc and NucleiStudio projetc is that the compile way,
here is cmake way to compile nuclei_sdk base on vscode environment, for speed I use NINJA.

## code generate
First of all, You need some code to be compile then download to Board and RISC-V MCU, you can just use NucleiStudio to generate a project, but there is 
no cmake or makefile you can simple use without NucleiStudio.

## the structure of this projetc
1. .vscode
    - launch.json
    - setting.json
    - task.json

    define Micro_OpenOCD_Debug and Micro_OpenOCD_Release

2. application
    - baremetal
    - freertos
    - threadx

    only one can be enable in same time, all of them is migrated from NucleiStudio

3. nuclei_sdk
    migrate from NucleiStudio project 

4. CMakeLists.txt
    - toolchain is riscv64-unknown-elf and openocd

## who can use 
1. platform is gd32vf103v_rvstar
2. download mode is FLASHXIP
3. compile condition is "-march=rv32imac -mabi=ilp32 -mtune=nuclei-200-series -mcmodel=medlow -mno-save-restore"

## toolchain place 
1. windows need set the location of riscv64-unknown-elf and openocd to PATH
2. if there many toolchain like arm-none-abi-gdb, you need add this cmd to specify gdb location for breakpoint debug in setting.json
```c
    "cortex-debug.gdbPath": "E:\\Program Files\\NucleiStudio\\toolchain\\gcc\\bin\\riscv64-unknown-elf-gdb.exe",
```

## how to use
1. change CMakeList.txt 
    ```c
    # BAREMETAL_MODE
    # FREERTOS_MODE
    # THREADX_MODE
    set(SYSTEM_MODE BAREMETAL_MODE)
    ```
2. in `运行和调试`, there is Micro_OpenOCD_Release, click it or press `F5`
