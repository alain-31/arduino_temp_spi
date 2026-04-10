# STM32 Bare-Metal SPI Communication

## What it does

This project demonstrates **SPI communication in bare-metal C on STM32**.

- STM32 acts as an **SPI master**
- Arduino acts as an **SPI slave**
- Arduino reads sensor data from an MPU6050 (via I2C)
- Sensor data (acceleration, gyroscope, temperature) is sent to the STM32 over SPI
- STM32 processes the received data and drives LEDs based on temperature

---

## Key ideas

- Bare-metal SPI configuration (no HAL, no libraries)
- Master/slave communication between two microcontrollers
- Transmission of a **structured data packet (mixed int + float)**
- Voltage level adaptation (**5V → 3.3V using a resistor divider**)
- Importance of **shared ground**
- Debugging using **GDB + VS Code (Cortex-Debug)**

---

## Hardware

### Main components

- STM32F411 (bare-metal)
- Arduino Uno (SPI slave)
- MPU6050 (I2C sensor)
- 3 LEDs (status output on STM32)

---

### SPI connections
```
STM32 (3.3V)                Arduino (5V)
--------------------------------------------
PA5  (SCK)   ----------->   D13 (SCK)
PA7  (MOSI)  ----------->   D11 (MOSI)

PA6  (MISO)  <---[DIVIDER]--- D12 (MISO)

PA4  (CS)    ----------->   D10 (SS)

GND          -------------- GND   (mandatory)

```
---

### Voltage divider (MISO protection)

```
Arduino D12 ---- R1 ----+----> STM32 PA6
                        |
                        R2
                        |
                       GND

```
Typical values:
- R1 = 1kΩ
- R2 = 2kΩ

---

### I2C (Arduino → MPU6050)

```
Arduino         MPU6050
------------------------
A4 (SDA)  ----> SDA
A5 (SCL)  ----> SCL
5V         ----> VCC
GND        ----> GND

```
---

### STM32 LEDs

```
PB0  → LED → R → GND
PB1  → LED → R → GND
PB10 → LED → R → GND

```
---

## Build

```
./build.sh

```
---

## Rebuild

```
./build.sh rebuild

```
---

## Flash

```
./flash.sh

```
---

## Run (build + flash)

```
./run.sh

```
---

## Requirements

- arm-none-eabi-gcc
- cmake
- openocd
- ST-Link debugger

---

## Project structure
```
.
└── 02_arduino_temp_spi
    ├── build.sh
    ├── CMakeLists.txt
    ├── CMSIS/
    ├── Core/
    ├── flash.sh
    ├── .gitignore
    ├── Linker/
    ├── README.md
    ├── run.sh
    └── Startup/
```
---

## Why this project matters

This project goes beyond a simple SPI example.

It demonstrates a **real embedded system workflow**:

- interfacing multiple microcontrollers
- handling voltage level differences safely
- structuring binary data exchange
- debugging at the register level
- solving real-world hardware issues (grounding, pin conflicts, signal integrity)

It also highlights a common reliability problem:

> The initial design used a DHT11 sensor, but due to unreliable behavior, it was replaced by an MPU6050, leading to a more robust and feature-rich system.

---
