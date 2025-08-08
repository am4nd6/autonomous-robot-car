# Autonomous Robot Car with TOF and Ultrasonic Sensors 🚗🤖

This project is an **autonomous robot car** designed to detect obstacles and navigate without human intervention.  
It uses two different distance sensors – a **Time of Flight (VL53L0X)** and an **ultrasonic sensor (HC-SR04)** – to improve accuracy and adaptability in various environments.

## 📌 Features
- **Autonomous Navigation** – The car moves and avoids obstacles automatically.
- **Dual Sensor System** – Combines TOF and ultrasonic sensors for better precision.
- **Real-time Distance Measurement** – Uses infrared and sound waves to detect obstacles.
- **Arduino Compatible** – Simple to build and program using Arduino IDE.
- **Compact Design** – Easy to assemble with off-the-shelf components.

## 🛠 Components Used
- **VL53L0X Time of Flight Sensor** – For precise short-range measurements.
- **HC-SR04 Ultrasonic Sensor** – For longer-range obstacle detection.
- **Arduino Board** (UNO, Nano, or compatible).
- **DC Motors + Motor Driver**.
- **Power Supply** (batteries or LiPo).
- **Chassis & Wheels**.

## ⚙️ How It Works
1. The **TOF sensor** measures short distances with high precision using infrared light.
2. The **ultrasonic sensor** measures longer distances using sound waves.
3. An **Arduino microcontroller** processes both readings and decides the car’s movement.
4. If an obstacle is detected, the car changes direction to avoid it.

## 🚀 Getting Started
### Requirements
- Arduino IDE
- VL53L0X Library ([Adafruit VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X))
- NewPing Library (for HC-SR04)

### Uploading the Code
1. Clone this repository:
   ```bash
   git clone https://github.com/am4nd6/autonomous-robot-car.git
