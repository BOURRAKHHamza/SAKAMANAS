# SAKAMANAS System: Multi-Aquarium IoT Thermal Monitoring and Alert System 🐟🌡️[cite: 1]

## Overview
The SAKAMANAS system is an embedded IoT baseline device designed for multi-tank freshwater bio-monitoring, specifically tracking the Wild strain, Mongolian strain, and Fry[cite: 1]. Serving as the primitive, simplified precursor to the ASSIF system, it is built around the ESP32-WROOM-32 microcontroller[cite: 1]. It continuously tracks water temperatures across multiple biological units, alerts operators to thermal deviations, and streams real-time telemetry to the cloud[cite: 1].

## Features
* **Multi-Zone Thermal Tracking:** Simultaneous 1-Wire bus monitoring of 3 waterproof DS18B20 temperature probes[cite: 1].
* **Dynamic Visual Display:** Asynchronous, non-blocking 2-second alternating cycle displaying individual tank temperatures on a 16x2 I2C LCD screen[cite: 1].
* **Local Alarm & Physical Mute:** Automatic audible buzzer trigger when temperatures breach the safe range of 20°C to 28°C, paired with a physical override push-button for temporary silencing[cite: 1].
* **Resilient Cloud Telemetry:** Multi-network Wi-Fi failover handling (WiFiMulti) and real-time streaming to a Blynk IoT dashboard via Virtual Pins V1, V2, and V3[cite: 1].

## Hardware Components & Wiring Guide
* **Microcontroller:** ESP32-WROOM-32 (3.3V Logic)[cite: 1]
* **Temperature Probes:** 3× DS18B20 (Waterproof)[cite: 1]
* **Display:** 16x2 I2C LCD (Address 0x27)[cite: 1]
* **Alert & Control:** Active Buzzer, Push-Button (6A 125VAC rated)[cite: 1]
* **Pull-Up Resistor:** 4.7kΩ resistor for the OneWire bus[cite: 1]

### Pinout Configuration
| Component | ESP32 Pin | Notes / Additional Connections |
| :--- | :--- | :--- |
| **DS18B20 Probes (Data)** | GPIO 4 | All 3 data lines connected in parallel. Requires a 4.7kΩ pull-up resistor to 3.3V.[cite: 1] |
| **LCD Display (SDA)** | GPIO 21 | I2C Data line. Powered by VIN (5V).[cite: 1] |
| **LCD Display (SCL)** | GPIO 22 | I2C Clock line.[cite: 1] |
| **Buzzer (Signal)** | GPIO 18 | Output pin. Driven HIGH during active thermal alerts.[cite: 1] |
| **Mute Button (Signal)** | GPIO 19 | Connected to GND. Uses internal INPUT_PULLUP resistor.[cite: 1] |

## Software Architecture
Written in C++ (Arduino Framework), SAKAMANAS utilizes an asynchronous, non-blocking architecture driven by `BlynkTimer`[cite: 1]. This design eliminates execution delays, allowing simultaneous 1-Wire sensor polling, dynamic LCD updates, physical mute button reading, and background cloud telemetry sync via Blynk and WiFiMulti[cite: 1].

## Circuit Diagram & Flow
*(Insert your block diagram or wiring schematic image here)*[cite: 1]
![Circuit Diagram](link_to_your_image.png)

---
*Primitive baseline version of the ASSIF system, developed for multi-aquarium IoT bio-monitoring.*[cite: 1]
