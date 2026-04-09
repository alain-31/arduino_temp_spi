#!/bin/bash

set -e

BUILD_DIR=build
TARGET=arduino_temp_spi.elf
ELF_PATH="$BUILD_DIR/$TARGET"

if [ ! -f "$ELF_PATH" ]; then
    echo "Error: $ELF_PATH not found."
    echo "Build the project first with ./build.sh"
    exit 1
fi

if [ ! -s "$ELF_PATH" ]; then
    echo "Error: $ELF_PATH exists but is empty."
    exit 1
fi

echo "Flashing $ELF_PATH ..."
openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg \
    -c "adapter speed 4000" \
    -c "program $ELF_PATH verify reset exit"

echo "Flash complete."