# Autonomous Fire Fighting Robot using Arduino

An autonomous embedded system powered by optimized, library-free **Embedded C** firmware. It is designed to detect localized fire sources using infrared flame sensors, navigate toward the fire using a 4-wheel chassis driven by an L298N driver, stop at a safe distance, and deploy a water pumping nozzle powered by a custom PWM-controlled servo sweeping mechanism.

## Features
* **Automated Fire Detection:** Uses a 3-sensor array (Left, Center, Right) to scan surroundings for fire signatures (wavelengths 760nm - 1100nm).
* **Safe-Distance Stopping:** Features logic to halt approximately 10 cm before reaching the source to prevent structural or electronic damage.
* **Sweeping Extinguishing Action:** A 5V submersible pump mounted on an SG90 servo sweeps from side to side using manual PWM pulse generation in C to distribute water evenly.
* **Dynamic Navigation:** Autonomous directional routing handling forward, left, and right trajectories with optimized H-bridge pin state control.

## Component Overview
* **Microcontroller:** Arduino UNO
* **Firmware Language:** Pure Embedded C (No external Arduino C++ libraries)
* **Detection:** 3x IR Flame Sensors
* **Actuation:** SG90 Servo Motor (driven via custom microsecond timing)
* **Motor Control:** L298N Dual H-Bridge Driver Module
* **Locomotion:** 4WD Chassis (4 DC gear motors & wheels)
* **Suppression:** 5V Mini Submersible Water Pump via a 5V SPDT Relay Module
