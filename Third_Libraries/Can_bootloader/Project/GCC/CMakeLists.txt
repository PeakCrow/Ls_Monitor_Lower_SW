cmake_minimum_required(VERSION 3.10)

message(STATUS "Project root is: ${CMAKE_CURRENT_SOURCE_DIR}")

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)#这个不重要，arm也能行
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(STM32F407_Project C CXX ASM)

# 设置 CMake 输出目录
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# 定义 STM32F103xB 作为目标设备
add_definitions(-DSTM32F407IGTx)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DSTM32F407xx)
add_definitions(-DUSE_FULL_LL_DRIVER)



# 设置编译器和工具链路径（确保 arm-none-eabi 工具链已安装并正确配置）
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SZ arm-none-eabi-size)


# STM32 系列 MCU 配置
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb ")
set(FPU_FLAGS "")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/stm32f407.ld")
set(LIBS "-lc -lm -lnosys") 

# 添加需要编译的所有源码
set(C_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/demo_canfd_update.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/bsp.c
	${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/bsp_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/stm32f4xx_it.c
	${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/src/bsp_can_bus.c
	${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/src/bsp_cpu_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/src/bsp_key.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/src/bsp_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/src/bsp_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../User/bsp/src/bsp_uart_fifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}../../../Libraries/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    # 如有更多文件，请在此处添加
)

#启动文件
set(ASM_SOURCES
    "${CMAKE_SOURCE_DIR}/startup_stm32f407xx.S"
)

# 指定头文件目录
include_directories(
    "${CMAKE_SOURCE_DIR}/../../User/bsp"
    "${CMAKE_SOURCE_DIR}/../../User/bsp/inc"
    "${CMAKE_SOURCE_DIR}/../../Libraries/CMSIS/Device/ST/STM32F4xx/Include"
	"${CMAKE_SOURCE_DIR}/../../Libraries/CMSIS/Include"
    "${CMAKE_SOURCE_DIR}/../../Libraries/STM32F4xx_HAL_Driver/Inc"
)

# 设置 CFLAGS
set(CMAKE_C_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -Wall -fdata-sections -ffunction-sections -Og")

# 添加调试选项（如果是调试构建）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -gdwarf-2")
endif()

# 设置汇编文件的编译标志
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} ${FPU_FLAGS} -x assembler-with-cpp -Wall -fdata-sections -ffunction-sections -Og")

# 创建目标文件（对象文件）
add_executable(${PROJECT_NAME}.elf ${C_SOURCES} ${ASM_SOURCES})

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T ${LINKER_SCRIPT}
	-specs=nosys.specs
    -specs=nano.specs
    # ${LIBS}
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

# target_link_libraries(${PROJECT_NAME} -lc -lm -lnosys)


# 添加生成 .hex 和 .bin 文件的自定义命令
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
)

# 设置清理命令
add_custom_target(clean-all
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/*
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/*.elf
    COMMENT "Cleaning build files..."
)
