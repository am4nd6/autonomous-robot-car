# Autonomous Robot Car with VL53L0X and HC-SR04

Project for a robot car capable of moving autonomously and choosing paths based on the distance to obstacles. The system uses a time-of-flight laser distance sensor (`VL53L0X`) mounted on a servo to observe the front and sides, as well as an ultrasonic sensor (`HC-SR04`) facing backward.

This guide was written so that someone without prior experience can identify the components, assemble the connections, install the software, upload the program, and perform the first tests safely.

> **Important:** the repository contains the program and the pinout used by the code, but does not specify the exact Arduino, motor driver, motor, or battery model. Before connecting the power supply, check your hardware manual and confirm the pinout.

## Table of Contents

- [What the project does](#what-the-project-does)
- [Required components](#required-components)
- [How the system works](#how-the-system-works)
- [Pinout](#pinout)
- [Assembly and power](#assembly-and-power)
- [Software installation](#software-installation)
- [Uploading the program](#uploading-the-program)
- [First test](#first-test)
- [Operation and serial monitor](#operation-and-serial-monitor)
- [Adjustable settings](#adjustable-settings)
- [Troubleshooting](#troubleshooting)
- [Limitations and improvements](#limitations-and-improvements)
- [Repository structure](#repository-structure)
- [License](#license)

## What the project does

When powered on, the Arduino:

1. Initializes the servo and the `VL53L0X` sensor.
2. Measures the distances to the front, right, left, and rear.
3. Chooses the direction with the most available space when the front path is blocked.
4. Activates the driver to move the motors.
5. While moving forward, continuously checks the front and diagonal areas.
6. Stops when it detects a nearby obstacle and reassesses the environment.

The distances used by the program are expressed in centimeters. The `VL53L0X` measures in millimeters and the code converts the result to centimeters; the `HC-SR04` is converted directly to centimeters.

## Required components

- An Arduino board compatible with the sketch, such as an Arduino Uno or Nano.
- A `VL53L0X` time-of-flight sensor compatible with the Adafruit library.
- An `HC-SR04` ultrasonic sensor.
- A servo motor to rotate the `VL53L0X`.
- A two-channel motor driver. The model is not defined by this project.
- Two DC motors with wheels.
- Chassis, free wheel/castor, screws, and mounts.
- A battery or power supply suitable for the motors.
- Jumper wires and, if necessary, a breadboard.
- A USB cable for programming the Arduino.

## How the system works

### VL53L0X sensor

The `VL53L0X` emits infrared light and calculates the time required for the signal to return. It is installed on a servo and pointed at different angles:

| Direction | Approximate servo angle |
|---|---:|
| Right | 10 degrees |
| Right 2 | 45 degrees |
| Front | 85 degrees |
| Left 2 | 115 degrees |
| Left | 150 degrees |

The `DIREITA2` and `ESQUERDA2` values are used while moving. The complete directional reading uses front, right, left, and rear.

### HC-SR04 sensor

The `HC-SR04` faces backward. The Arduino sends a pulse through the `TRIG` pin, measures the return time on the `ECHO` pin, and calculates the distance. The sensor must be correctly aligned and must not receive a voltage incompatible with the board.

### Movement decision

- `distancia_minima = 30`: below 30 cm, the direction is considered dangerous.
- `distancia_para_andar = 50`: a direction is considered to have enough space from 50 cm onward.
- If the front is clear, the car moves forward.
- If the front is blocked, the program compares the sides and rear.
- In a tie or an unforeseen situation, the chosen direction may depend on the values read and the last movement.

## Pinout

The connections below are those used by `Code/versao4_carrinho.ino`.

### Motor driver

| Arduino | Name in code | Function |
|---:|---|---|
| D3 | `entrada1` | Motor/channel 1 control, forward direction |
| D5 | `entrada2` | Motor/channel 1 control, reverse direction |
| D6 | `entrada3` | Motor/channel 2 control, reverse direction |
| D11 | `entrada4` | Motor/channel 2 control, forward direction |

Pins D3, D5, D6, and D11 also support PWM on Uno/Nano boards, allowing speed control. The exact mapping between driver inputs and motors depends on the driver model.

### Sensors and servo

| Arduino | Component | Pin/signal |
|---:|---|---|
| D2 | HC-SR04 | Rear `TRIG` |
| D7 | HC-SR04 | Rear `ECHO` |
| D9 | Servo | Control signal |
| D4 | Auxiliary LED | Reserved in code |
| `SDA`/`SCL` | VL53L0X | I2C communication |

On the Arduino Uno, `SDA` is A4 and `SCL` is A5. On the Arduino Nano, they are also normally A4 and A5. On other boards, consult the board documentation. Also connect `VCC` and `GND` on the modules according to the voltage they support.

## Assembly and power

1. Secure the motors, wheels, free wheel, and board to the chassis.
2. Install the servo at the front and secure the `VL53L0X` to the servo arm.
3. Install the `HC-SR04` at the rear, facing away from the front.
4. Connect the driver's four control signals to the indicated pins.
5. Connect the servo to D9 and the `VL53L0X` to the I2C bus.
6. Connect `TRIG` to D2 and `ECHO` to D7.
7. Connect the Arduino, driver, and sensor GNDs together.
8. Power the motors through the driver's dedicated input, using a battery compatible with the motors.

### Mandatory precautions

- Do not power the motors through the Arduino pins.
- Do not connect the motor battery directly to a signal pin.
- Confirm the voltage of the `VL53L0X`, servo, and `HC-SR04` before connecting VCC.
- Do not change the wiring while the battery is connected.
- During initial tests, keep the wheels suspended or use a support that prevents the car from moving away.
- Use an open area free of people, animals, stairs, or fragile objects.

## Software installation

### 1. Install the Arduino IDE

Download the Arduino IDE from the official website: [arduino.cc/en/software](https://www.arduino.cc/en/software). Install the version appropriate for your operating system.

### 2. Download the project

In the terminal, run:

```bash
git clone https://github.com/am4nd6/autonomous-robot-car.git
cd autonomous-robot-car
```

You can also download the project as a ZIP file using the **Code > Download ZIP** button on GitHub.

### 3. Install the library

The sketch uses:

- `Servo.h`, normally included with the Arduino IDE.
- `Adafruit_VL53L0X`, available in the [official Adafruit repository](https://github.com/adafruit/Adafruit_VL53L0X).

To install the library in the IDE:

1. Open **Sketch > Include Library > Manage Libraries**.
2. Search for **Adafruit VL53L0X**.
3. Install the library published by Adafruit.
4. Accept the dependencies requested by the manager.

The project **does not use the NewPing library**, although previous versions of the README mentioned that name.

## Uploading the program

1. Open `Code/versao4_carrinho.ino` in the Arduino IDE.
2. Connect the Arduino to the computer using the USB cable.
3. Under **Tools > Board**, select the correct board model.
4. Under **Tools > Port**, select the corresponding serial port.
5. Click **Verify** to compile.
6. If compilation finishes without errors, click **Upload**.
7. Wait for the completion message.
8. Disconnect the USB cable only if the motor power supply has been installed safely.

The Arduino IDE may ask for confirmation to install library dependencies. This is expected. If the board does not appear, install the USB driver corresponding to the board's converter and replace the USB cable.

## First test

Perform the tests in this order:

1. With the wheels suspended, power only the Arduino and confirm that the program starts.
2. Open the **Serial Monitor** at `9600 baud`.
3. Look for the message `INICIANDO...`.
4. Check that the servo centers at approximately 85 degrees.
5. Confirm that the VL53L0X sensor initializes. If it fails, the internal LED will blink and the program will stop.
6. With motor power off, bring objects near the sensors and observe the diagnostic messages.
7. If necessary, uncomment `testarmotores();` in `setup()` to test the motors. Do this only with the car immobilized.
8. Turn the motors on at low speed and test on a flat, clear surface.

## Operation and serial monitor

Open the Serial Monitor at **9600 baud**. The program may display messages such as:

- `INICIANDO...`
- `Failed to boot VL53L0X`
- `Front distance`
- `Right distance`
- `Left distance`
- `Rear distance`
- `Moving forward`
- `Moving backward`
- `Moving right`
- `Moving left`

The internal LED toggles approximately every 200 ms while the program is running. The `ping` message is also sent at that interval.

## Adjustable settings

The values can be changed at the beginning of the sketch:

| Variable | Current value | Effect |
|---|---:|---|
| `velocidade` | 60.5 | Nominal speed of one side |
| `velocidade2` | 58 | Nominal speed of the other side |
| `velocidade_tras` | 80 | Reverse speed |
| `distancia_minima` | 30 cm | Obstacle-detection threshold |
| `distancia_para_andar` | 50 cm | Space considered sufficient |
| `velocidade_arranque` | 180 | Initial motor power |
| `TEMPO_MOVIMENTO_SERVO` | 200 ms | Wait time after moving the servo |
| `SERVO_CENTRO` | 85 degrees | Sensor center position |

Adjust the speeds first. High values can make the car start abruptly. If the car veers, adjust `velocidade` and `velocidade2` gradually. If the servo reaches its mechanical limit, reduce the scanning angles.

## Troubleshooting

### The program does not compile

Confirm that the **Adafruit VL53L0X** library is installed and that the open file is `versao4_carrinho.ino`. The `Servo` library should be available in the Arduino IDE installation.

### The VL53L0X does not initialize

Turn off the power, check VCC, GND, SDA, and SCL, and confirm the module voltage. Also check that the sensor is not loose on the servo. During the failure, the code blinks the internal LED and remains waiting.

### The servo does not move or vibrates

Check the signal on D9, the power supply, and the common GND. Servos may require more current than the USB port can provide; use a suitable external power supply while keeping the GND shared.

### The motors do not turn

Check the driver's separate power supply, common GND, D3/D5/D6/D11 connections, and driver enable state. Some drivers have `ENA` and `ENB` pins or jumpers that must be configured.

### The car moves backward or turns the wrong way

Turn off the battery and reverse the wires of the affected motor or adapt the movement-function logic. Never change the wiring while the circuit is powered.

### The car veers

This can result from differences between motors, wheels, or friction. Adjust `velocidade` and `velocidade2` in small increments.

### The distances seem incorrect

Clean the VL53L0X window, check sensor alignment, and avoid very steep, transparent, or absorbent surfaces. The `HC-SR04` should be free of very nearby obstacles and excessive vibration.

### The car does not stop

Immediately turn off motor power. Then check the pinout, driver power supply, and activation logic. Perform new tests with the wheels suspended.

## Limitations and improvements

This is an experimental project. The code has no encoders for measuring wheel rotation, PID control, remote communication, or environment mapping. Movement time is based on `delay()`, so turning accuracy depends on the battery, floor, motors, and chassis weight.

Possible improvements include adding encoders, replacing delays with `millis()`-based timing, filtering invalid readings, using a documented specific driver, adding more precise speed control, and creating automated tests for the decision logic.

## Repository structure

```text
autonomous-robot-car/
|-- README.md
|-- LICENSE
`-- Code/
    `-- versao4_carrinho.ino
```

## License

This project is distributed under the [MIT license](LICENSE). Third-party libraries remain subject to their own licenses.

## References

- [Arduino IDE](https://www.arduino.cc/en/software)
- [Adafruit VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X)
- [Arduino documentation](https://docs.arduino.cc/)
