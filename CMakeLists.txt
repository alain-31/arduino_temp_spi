cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(arduino_temp_spi C ASM)

set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
)

set(COMMON_FLAGS
    ${MCU_FLAGS}
    -O0
    -g3
    -Wall
    -Wextra
    -DSTM32F411xE
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Linker/STM32F411RETX_FLASH.ld)

add_executable(${PROJECT_NAME}.elf
    Core/Src/main.c
    Core/Src/system_stm32f4xx.c
    Core/Src/runtime_stubs.c
    Startup/startup_stm32f411xe.s
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/CMSIS/Device/ST/STM32F4xx/Include
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${COMMON_FLAGS}
)

set_source_files_properties(
    Startup/startup_stm32f411xe.s
    PROPERTIES
    COMPILE_FLAGS "-x assembler-with-cpp"
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -nostartfiles
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map
    --specs=nosys.specs
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
            ${PROJECT_NAME}.elf
            ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)