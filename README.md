# Squid Game Challenge – Embedded Motion Control System
<img width="785" height="354" alt="AdobeExpressPhotos_1a7e66570cfc48d2a3ec30a32bd483de_CopyEdited" src="https://github.com/user-attachments/assets/0276e5f3-c81d-4f25-93c8-550aa0a9a2b3" />


## Overview
This project is an embedded system inspired by the "Red Light / Green Light" game from Squid Game. It uses real-time sensing, motion detection, and actuator control to monitor player movement and trigger responses.

The system detects motion using sensors and determines whether movement occurs during restricted periods. If movement is detected when not allowed, the system triggers a response mechanism.

## Features
- Real-time motion detection using ultrasonic sensing
- Game logic implementation with timed "Red Light / Green Light" states
- Sensor-driven decision making for detecting player movement
- Actuator control using servos and motors
- Interrupt-driven timing for responsive system behavior
- Embedded control logic implemented across multiple modules

## System Architecture
This project is structured using multiple Arduino modules:

- `SQUID_GAME_FINAL_PROJECT.ino`  
  Main control logic, game state management, and system coordination

- `SQUID_GAME_CHALLENGEgoal.ino`  
  Sensor handling, movement detection, and supporting logic

Together, these modules handle:
- Sensor input processing
- Game state transitions
- Actuator control
- Timing and synchronization

## Hardware Components
- Microcontroller (Arduino-compatible board)
- Ultrasonic sensor (distance/motion detection)
- Servo motors (actuation)
- MPU6050 accelerometer (tilt/motion detection)
- Stepper motor (movement control)
- Supporting circuitry

## Technologies Used
- C/C++ (Arduino)
- Embedded systems programming
- Interrupts and timers
- Sensor integration (ultrasonic, accelerometer)
- Real-time control systems

## Key Challenges & Learnings
- Synchronizing sensor input with real-time game logic
- Handling noisy sensor data and tuning detection thresholds
- Managing timing and responsiveness using interrupts
- Coordinating multiple actuators with sensor feedback
- Debugging timing issues and inconsistent motion detection

## Future Improvements
- Improve motion detection accuracy using sensor fusion
- Add wireless control or monitoring (WiFi/BLE)
- Enhance game logic with multiple players or scoring system
- Optimize timing precision and system responsiveness

## Notes
This project demonstrates real-time embedded system design involving sensing, decision-making, and actuation. It emphasizes working with hardware constraints, debugging timing-related issues, and building responsive control systems.
