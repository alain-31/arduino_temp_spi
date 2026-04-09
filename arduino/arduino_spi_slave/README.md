# Arduino Firmware — temp SPI Slave

## Overview

This Arduino firmware acts as an SPI slave for an STM32 master.

It performs:

* Temperature acquisition using a temp sensor
* Local display on a 16x2 LCD (LCD1602)
* Data exchange with the STM32 over SPI

---

## Hardware

* Arduino Uno (or compatible)
* MPU6050 temperature sensor
* LCD1602 (16x2 character display)
* SPI connection to STM32

---

## Features

* Reads temperature from sensor
* Displays values on LCD1602
* Responds to SPI requests from STM32
* Can echo or send structured sensor data

---

## Arduino IDE Setup

1. Open:

   ```
   arduino/arduino_spi_slave/arduino_spi_slave.ino
   ```

2. Select board:

   ```
   Arduino Uno
   ```

3. Select correct serial port 

   example: 

   ```
   /dev/ttyACM0
   ```

---

## Required Libraries

Install via Library Manager:
* MPU6050 (Jeff Rowberg)
* LiquidCrystal (usually built-in)

---

## Wiring (example)

### temp

* VCC → 5V
* GND → GND
* DATA → Digital pin (e.g. D2)

### LCD1602 (parallel mode)

* RS, E, D4–D7 → Arduino digital pins
* VCC → 5V
* GND → GND
* VO → potentiometer (contrast)

### SPI (Arduino as Slave)

* MISO → STM32 MOSI
* MOSI → STM32 MISO
* SCK  → STM32 SCK
* SS   → STM32 NSS

---

## Notes

* Arduino is configured as SPI slave
* STM32 is the SPI master
* Communication protocol is defined in the firmware

---
