# Transporter: Mecanum Robot with Lifter

ESP32 firmware for a four-wheel mecanum transporter robot with a lifter, driven by a Bluetooth gamepad through Bluepad32.

<!-- TODO: add demo GIF/photo -->

## Overview

A gamepad (PS4/PS5/Xbox or any Bluepad32-supported controller) connects to the ESP32 over Bluetooth. Joystick input is shaped with a quadratic response curve, converted into four wheel speeds with mecanum inverse kinematics, normalized, and sent to the motor drivers as PWM + direction signals. A second axis drives a DC lifter. An SSD1306 OLED shows the UKSW logo on boot.

## Features

- Omnidirectional motion: forward/back, strafe and rotate at the same time
- Mecanum kinematics with normalization so no wheel exceeds full speed
- Quadratic joystick curve and dead zone (|raw| < 25) for fine low-speed control
- L2 precision mode (50% speed)
- Lifter up/down on the right stick
- Placeholders for a gripper on L1/R1

## Controls

| Input | Action |
| --- | --- |
| Left stick Y | Forward / backward |
| Left stick X | Strafe left / right |
| Right stick X | Rotate |
| Right stick Y | Lifter up / down |
| L2 (hold) | Half speed |
| L1 / R1 | Gripper (not implemented yet) |

## Kinematics

```
FL = Y + X + R
FR = Y - X - R
BL = Y - X + R
BR = Y + X - R
```

Values are divided by `max(1, |FL|, |FR|, |BL|, |BR|)` and scaled to 0 to 255 PWM.

## Wiring (ESP32 DevKit V1)

| Motor | PWM | DIR |
| --- | --- | --- |
| Front left | GPIO 13 | GPIO 12 |
| Front right | GPIO 4 | GPIO 2 |
| Back left | GPIO 14 | GPIO 27 |
| Back right | GPIO 33 | GPIO 32 |

| Other | Pin |
| --- | --- |
| Lifter PWM | GPIO 25 |
| Lifter direction | GPIO 26 |
| OLED SDA / SCL | GPIO 21 / 22 |

## Files

```
Main_Porter.ino     setup() and loop(), Bluepad32 init
Global_Porter.h     Pin map, OLED init, controller connect/disconnect callbacks
Motion_Porter.h     Joystick shaping, mecanum kinematics, PWM output, lifter
OLED.h              UKSW logo bitmap
```

## Build

1. Install the Bluepad32 board package for Arduino (see the [Bluepad32 docs](https://bluepad32.readthedocs.io/)) and select an ESP32 board from it.
2. Install `Adafruit GFX` and `Adafruit SSD1306`.
3. Open `Main_Porter.ino` and upload.
4. Put the gamepad in pairing mode. The connection is logged on the serial monitor at 115200 baud.
