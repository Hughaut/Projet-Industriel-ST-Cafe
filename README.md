Zigbee Weight Sensor — STM32 NUCLEO-WBA65RI

1. Project Overview

This project aims to develop a wireless weight monitoring system using Zigbee communication.

The STM32 NUCLEO-WBA65RI collects weight measurements from an I2C sensor and communicates them to a Raspberry Pi acting as the Zigbee coordinator.

The transmitted information includes:

Weight: measured value in grams (g)

Validity: indicates whether the measurement is valid (1) or invalid (0)

2. Hardware

Component

Role

STM32 NUCLEO-WBA65RI

Zigbee sensor node

I2C weight sensor

Weight measurement

Raspberry Pi

Central server

Zigbee USB dongle

Zigbee coordinator interface

3. Software

STM32CubeIDE

STM32CubeWBA firmware package

STM32 Zigbee stack

C programming language

4. System Architecture

Weight Sensor (I2C)
        |
        v
STM32 NUCLEO-WBA65RI
        |
        | Zigbee
        v
Zigbee Coordinator (USB Dongle)
        |
        v
Raspberry Pi
        |
        v
Server / Web Interface
