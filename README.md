# SAKAMANAS System: Multi-Aquarium IoT Thermal Monitoring and Alert System 🐟🌡️

## Overview
The SAKAMANAS system is an embedded IoT baseline device designed for multi-tank freshwater bio-monitoring, specifically tracking the Wild strain, Mongolian strain, and Fry. Serving as the primitive, simplified precursor to the ASSIF system, it is built around the ESP32-WROOM-32 microcontroller. It continuously tracks water temperatures across multiple biological units, alerts operators to thermal deviations, and streams real-time telemetry to the cloud.

## Features
* **Multi-Zone Thermal Tracking:** Simultaneous 1-Wire bus monitoring of 3 waterproof DS18B20 temperature probes.
* **Dynamic Visual Display:** Asynchronous, non-blocking 2-second alternating cycle displaying individual tank temperatures on a 16x2 I2C LCD screen.
* **Local Alarm & Physical Mute:** Automatic audible buzzer trigger when temperatures breach the safe range of 20°C to 28°C, paired with a physical override push-button for temporary silencing.
* **Resilient Cloud Telemetry:** Multi-network Wi-Fi failover handling (WiFiMulti) and real-time streaming to a Blynk IoT dashboard via Virtual Pins V1, V2, and V3.

## Hardware Components & Wiring Guide
* **Microcontroller:** ESP32-WROOM-32 (3.3V Logic)
* **Temperature Probes:** 3× DS18B20 (Waterproof)
* **Display:** 16x2 I2C LCD (Address 0x27)
* **Alert & Control:** Active Buzzer, Push-Button (6A 125VAC rated)
* **Pull-Up Resistor:** 4.7kΩ resistor for the OneWire bus

### Pinout Configuration
| Component | ESP32 Pin | Notes / Additional Connections |
| :--- | :--- | :--- |
| **DS18B20 Probes (Data)** | GPIO 4 | All 3 data lines connected in parallel. Requires a 4.7kΩ pull-up resistor to 3.3V. |
| **LCD Display (SDA)** | GPIO 21 | I2C Data line. Powered by VIN (5V). |
| **LCD Display (SCL)** | GPIO 22 | I2C Clock line. |
| **Buzzer (Signal)** | GPIO 18 | Output pin. Driven HIGH during active thermal alerts. |
| **Mute Button (Signal)** | GPIO 19 | Connected to GND. Uses internal INPUT_PULLUP resistor. |

## Software Architecture
Written in C++ (Arduino Framework), SAKAMANAS utilizes an asynchronous, non-blocking architecture driven by `BlynkTimer`. This design eliminates execution delays, allowing simultaneous 1-Wire sensor polling, dynamic LCD updates, physical mute button reading, and background cloud telemetry sync via Blynk and WiFiMulti.

## Circuit Diagram & Flow
*(Insert your block diagram or wiring schematic image here)*
![Circuit Diagram](link_to_your_image.png)

---
*Primitive baseline version of the ASSIF system, developed for multi-aquarium IoT bio-monitoring.*
