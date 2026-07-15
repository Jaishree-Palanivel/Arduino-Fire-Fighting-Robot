# Autonomous Fire Fighting Robot using Arduino

An autonomous embedded system designed to detect localized fire sources using infrared flame sensors, navigate toward the fire using a 4-wheel chassis driven by an L298N driver, stop at a safe distance, and deploy a water pumping nozzle powered by a servo-guided sweeping mechanism.

## Features
* **Automated Fire Detection:** Uses a 3-sensor array (Left, Center, Right) to scan surroundings for fire signatures (wavelengths 760nm - 1100nm).
* **Safe-Distance Stopping:** Features logic to halt approximately 10 cm before reaching the source to prevent structural or electronic damage.
* **Sweeping Extinguishing Action:** A 5V submersible pump mounted on an SG90 servo sweeps from side to side to distribute water evenly and put out flames effectively.
* **Dynamic Navigation:** Autonomous directional routing handling forward, left, and right trajectories.

## Component Overview
* **Microcontroller:** Arduino UNO
* **Detection:** 3x IR Flame Sensors
* **Actuation:** SG90 Servo Motor (for sweeping nozzle action)
* **Motor Control:** L298N Dual H-Bridge Driver Module
* **Locomotion:** 4WD Chassis (4 DC gear motors & wheels)
* **Suppression:** 5V Mini Submersible Water Pump via a 5V SPDT Relay Module
