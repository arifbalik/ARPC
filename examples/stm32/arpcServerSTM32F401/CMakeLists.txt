cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              20)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})
message("Build type: "              ${CMAKE_BUILD_TYPE})

#
# Core project settings
#
project(arpc-stm32f401-server-example)
enable_language(C CXX ASM)

#
# Core MCU flags, CPU, instruction set and FPU setup
# Needs to be set properly for your MCU
#
set(CPU_PARAMETERS
    -mthumb

    # This needs attention to properly set for used MCU
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# Set linker script
set(linker_script_SRC               ${PROJ_PATH}/STM32F401CCUX_FLASH.ld)
set(EXECUTABLE                      ${CMAKE_PROJECT_NAME})

#
# List of source files to compile
#
set(sources_SRCS
Core/Src/main.c 
Core/Src/stm32f4xx_it.c 
Core/Src/stm32f4xx_hal_msp.c 
USB_DEVICE/App/usb_device.c 
USB_DEVICE/App/usbd_desc.c 
USB_DEVICE/App/usbd_cdc_if.c 
USB_DEVICE/Target/usbd_conf.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c 
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c 
Core/Src/system_stm32f4xx.c 
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c 
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c 
Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c 
Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c 
Drivers/arpc_server/arpc_server.c 
Drivers/arpc_server/arpc_generic.c 
startup_stm32f401xc.s
)

#
# Include directories
#
set(include_path_DIRS
USB_DEVICE/App 
USB_DEVICE/Target 
Core/Inc 
Drivers/STM32F4xx_HAL_Driver/Inc 
Drivers/STM32F4xx_HAL_Driver/Inc/Legacy 
Middlewares/ST/STM32_USB_Device_Library/Core/Inc 
Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc 
Drivers/CMSIS/Device/ST/STM32F4xx/Include 
Drivers/CMSIS/Include 
Drivers/arpc_server
)

#
# Symbols definition
#
set(symbols_SYMB
-DUSE_HAL_DRIVER 
-DSTM32F401xC
)

# Executable files
add_executable(${EXECUTABLE} ${sources_SRCS})

# Include paths
target_include_directories(${EXECUTABLE} PRIVATE ${include_path_DIRS})

# Project symbols
target_compile_definitions(${EXECUTABLE} PRIVATE ${symbols_SYMB})

# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    # Full debug configuration
    -Og -g3 -ggdb
)

# Linker options
target_link_options(${EXECUTABLE} PRIVATE
    -T${linker_script_SRC}
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -u _printf_float                # STDIO float formatting support
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage
)

# Execute post-build to print size
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
)

# Convert output to hex and binary
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
)

# Convert to bin file -> add conditional check?
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
)