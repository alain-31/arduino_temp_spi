#!/bin/bash

set -e

BUILD_DIR=build
CMD="$1"

if [ "$CMD" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    exit 0
fi

if [ "$CMD" = "rebuild" ]; then
    echo "Rebuilding project..."
    rm -rf "$BUILD_DIR"
fi

echo "Configuring project..."
cmake -S . -B "$BUILD_DIR"

echo "Building project..."
cmake --build "$BUILD_DIR"

echo "Build complete."