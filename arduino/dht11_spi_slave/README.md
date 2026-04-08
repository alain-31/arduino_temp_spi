# Arduino Firmware — DHT11 SPI Slave

## Overview

This Arduino firmware acts as an SPI slave for an STM32 master.

It performs:

* Temperature and humidity acquisition using a DHT11 sensor
* Local display on a 16x2 LCD (LCD1602)
* Data exchange with the STM32 over SPI

---

## Hardware

* Arduino Uno (or compatible)
* DHT11 sensor
* LCD1602 (16x2 character display)
* SPI connection to STM32

---

## Features

* Reads temperature and humidity from DHT11
* Displays values on LCD1602
* Responds to SPI requests from STM32
* Can echo or send structured sensor data

---

## Arduino IDE Setup

1. Open:

   ```
   arduino/dht11_spi_slave/dht11_spi_slave.ino
   ```

2. Select board:

   ```
   Arduino Uno
   ```

3. Select correct serial port

---

## Required Libraries

Install via Library Manager:

* DHT sensor library (Adafruit)
* Adafruit Unified Sensor
* LiquidCrystal (usually built-in)

---

## Wiring (example)

### DHT11

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
* Communication protocol is defined in the firmware (to be extended)

---

## Future Improvements

* Define structured SPI protocol (commands + responses)
* Add checksum or simple framing
* Improve LCD refresh strategy
* Replace DHT11 with DHT22 (better accuracy)
