# TrackedBot – Surveillance Rover

## Problem Statement
Defence and industrial security zones require continuous real-time surveillance in environments that are hazardous, restricted, or difficult for human access. Existing surveillance methods such as fixed monitoring systems and manual human observation suffer from limited coverage, reduced flexibility, and safety risks.

Human-based monitoring exposes personnel to dangerous conditions and may not provide consistent situational awareness. Therefore, a surveillance system is required to ensure reliable monitoring, operational safety, and consistent situational awareness under challenging conditions.

---

## Proposed Solution
TrackedBot is a tracked surveillance rover designed to operate in defence and industrial security environments. The system enables remote operation and real-time monitoring, reducing the need for direct human presence in hazardous or restricted zones.

The rover supports repeatable surveillance missions and demonstrates autonomous navigation concepts relevant to modern defence systems. A Return-to-Memory (RTM) capability ensures reliable recovery of the rover after mission execution, improving operational confidence and mission safety.

The system is operated through a simple remote control interface, allowing personnel to deploy and control the rover with minimal training. Live feedback enables operators to make informed decisions during surveillance missions while reducing manual intervention.

---

## Rover Design
![TrackedBot Rover Design](Design/DesignFile.jpeg)

---


## System Limitations
This project is developed as a prototype intended for controlled and semi-operational environments. GPS-based position awareness may be affected in indoor or obstructed areas. Wireless communication range and onboard power capacity limit long-duration operation.

Terrain irregularities and obstacles may affect mobility and path retracing accuracy, requiring careful deployment planning.

---

## Key Features
- Tracked mobile surveillance platform
- Remote operation through web-based control
- Real-time situational awareness
- Autonomous Return-to-Memory navigation
- Reduced human exposure to hazardous environments
- Cost-effective and scalable prototype design

---

## Technologies Used

### Hardware
- ESP32
- Tracked Rover Chassis
- DC Gear Motors
- Motor Driver (L298N / L293D)
- Li-ion Battery

### Software
- Arduino IDE
- Embedded C/C++

### Communication & Control
- WiFi
- Web-Based Control Interface

### Navigation & Autonomy
- Return-to-Memory (RTM)
- Autonomous Navigation Logic


## Repository Structure

TrackedBot/
├── Arduino_Code/        # Arduino firmware for ESP32
│   └── TrackedBot.ino
├── Design/              # Rover design and layout files
│   └── DesignFile.jpeg
├── LICENSE              # MIT License
└── README.md            # Project documentation

