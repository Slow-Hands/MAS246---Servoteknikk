# Elevator Project – MAS246 Motion Control

![Arduino Mega 2560](https://img.shields.io/badge/board-Arduino%20Mega%202560-00979D?logo=arduino&logoColor=white)
![C++](https://img.shields.io/badge/language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![University of Agder](https://img.shields.io/badge/UiA-MAS246%20autumn%202026-555)

<p align="center">
  <img src="docs/images/elevator_module.png" alt="The MAS246 elevator module" width="560">
</p>

A scaled eight-floor elevator controlled by an Arduino Mega 2560. The cabin
travels along a jerk-limited **S-curve**, a **plain PID controller** tracks the
reference, and a **collective queue** serves every request in the current
direction before the cabin turns around. The project is built on the open
elevator framework of Sanfilippo *et al.* [1] and the requirements document
of Hoang [2] (REQ 1–16, SAF 8).

> The elevator is a simulation: the cabin is the DC motor shaft with its
> encoder, so no real cabin mass or passenger load acts on the motor.
> 1000 encoder counts correspond to 1 m, and each floor is 4 m (4000 counts).

---

## Highlights

| | |
|---|---|
| **Motion** | Trapezoidal ramp smoothed by a 12-sample moving average → S-curve with $v_{\max}$ = 1.5 m/s, $a_{\max}$ = 2 m/s² |
| **Control** | Discrete PID ($K_p$ = 0.4, $K_i$ = 0.35, $K_d$ = 0.12), filtered D-term, anti-windup, PWM limited to ±120 |
| **Accuracy** | No overshoot, within ±5 counts about 0.5 s after the reference arrives, final error 0–3 counts |
| **Queue** | Cabin calls, hall calls up/down, serves the current direction first (REQ 16) |
| **Door** | Stepper-motor door with OPEN / HALF / CLOSED, always closed while moving (SAF 8) |
| **Safety** | Overload detection (door stays open) and an emergency mode |
| **Display** | 16×2 LCD with floor, direction arrow and load, plus one LED per queued floor |

### Measured trips

| Trip | Reference done | Settled (±5 counts) | Max \|e\| | Overshoot | Final error |
|---|---:|---:|---:|---:|---:|
| Floor 0 → 7 | 19.48 s | 20.02 s | 150 counts | 0 | 0 counts |
| Floor 0 → 2 | 6.18 s | 6.68 s | 147 counts | 0 | 3 counts |

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/images/dark/trip_0_to_7.png">
    <img src="docs/images/light/trip_0_to_7.png" alt="Position, velocity and tracking error for the trip from floor 0 to 7" width="480">
  </picture>
  <br><em>Trip from floor 0 to 7: the measured position follows the S-curve reference without overshoot.</em>
</p>

---

## Getting started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software). The only
   library used is `LiquidCrystal`, which comes with the IDE.
2. Open `Heis_Program/Heis_Program.ino`. The IDE opens all the `.cpp` and
   `.h` files in the folder as tabs.
3. Select **Tools → Board → Arduino Mega or Mega 2560** and the right port.
4. Click **Upload**. The cabin starts at floor 0 with the door closing.

The program prints data on the serial port at **115 200 baud**, which can be
used for logging and plotting.

## How to use the elevator

| Action | How |
|---|---|
| Cabin call (inside the cabin) | Press the floor button for floor 0–7 |
| Hall call (outside the cabin) | Joystick left/right selects the floor, up/down selects the direction, then press the encoder button to confirm |
| Reopen the door | Press the button for the floor the cabin is at |
| Simulate passenger load | Turn the potentiometer. At 3300 kg or more the LCD shows `OVERVEKT` and the door stays open |
| Emergency mode | Turn the encoder knob 8 steps in either direction. The cabin stops at the next floor and keeps the door open |
| Reset emergency mode | Press the encoder button twice |

A request is served when the cabin reaches the floor and the door is fully
open. The door then stays open for 3 s.

---

## How it works

### Architecture

Each class has one job. `Heis` and `Ko` decide **where to go**, while
`Profil` and `Regulator` decide **how to move**.

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/images/dark/uml_class_diagram.png">
    <img src="docs/images/light/uml_class_diagram.png" alt="UML class diagram" width="900">
  </picture>
</p>

### Main loop

The control loop runs every **20 ms**. In each sample it reads the position,
updates the state machine, generates the reference, updates the LCD and sets
the motor output. The door stepper is updated in every pass of `loop()`,
outside the 20 ms timing, because it needs a step every 10 ms.

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/images/dark/sequence_diagram.png">
    <img src="docs/images/light/sequence_diagram.png" alt="UML sequence diagram of one pass of loop()" width="900">
  </picture>
</p>

### State machine

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/images/dark/state_machine.png">
    <img src="docs/images/light/state_machine.png" alt="State machine of the elevator" width="560">
  </picture>
</p>

The cabin can only leave a floor from `STAAR` or `DOR_LUKKER`, and only when
the door is closed, so the door is never open while the cabin moves.

### Request queue

`Ko` keeps one Boolean array per request type. The next stop above floor $k$
is the first floor with a cabin call or an up call. If there is none, it is
the highest floor with a down call, so the cabin goes to the last request
before turning around. The search below is symmetric.

### Control loop

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/images/dark/control_loop.png">
    <img src="docs/images/light/control_loop.png" alt="Control loop" width="620">
  </picture>
</p>

The trapezoidal ramp is filtered by a moving average over 12 samples
(0.24 s), which turns the steps in acceleration into ramps and limits the
jerk to about 8.3 m/s³.

---

## Hardware

Everything runs on the MAS246 laboratory board shown at the top, so no
extra wiring is needed.

### Component list

| Qty | Component | Details | Used for |
|---:|---|---|---|
| 1 | Arduino Mega 2560 Rev3 | ATmega2560, 16 MHz | Runs the elevator program |
| 1 | Geared DC motor | 12 V supply | Moves the cabin |
| 1 | DRV8840 H-bridge | Direction + PWM, duty cycle limited to 120/255 | Drives the DC motor |
| 1 | Quadrature encoder | Hall effect, 16 lines on the motor shaft, 2× decoding | Measures the cabin position |
| 1 | Bipolar stepper motor | Full-step mode, 100 steps from closed to open | Opens and closes the door |
| 1 | DRV8813 stepper driver | Dual H-bridge | Drives the stepper motor |
| 1 | MCP4922 DAC | 12-bit, two channels | Sets the stepper current |
| 8 | Push buttons | Internal pull-up | Floor buttons in the cabin |
| 8 | LEDs | One per floor | Shows queued floors |
| 1 | Joystick | Two analog axes | Selects floor and direction for hall calls |
| 1 | Rotary encoder with push button | Two channels + button | Confirms hall calls, emergency mode and reset |
| 1 | Potentiometer | Scaled to 0–5000 kg | Simulates the passenger load |
| 1 | LCD 16×2 | 4-bit mode, backlight | Floor, direction arrow, load and menu |
| 1 | USB cable | 115 200 baud serial | Upload and logging |

### Pin mapping

| Component | Function | Pins |
|---|---|---|
| DC motor + DRV8840 | Moves the cabin | DIR 6, PWM 7 |
| Quadrature encoder | Cabin position | A 21 (interrupt), B 20 |
| Stepper motor + DRV8813 | Door | 66–69 |
| MCP4922 DAC | Stepper current reference | 30–33 |
| 8 push buttons | Floor buttons in the cabin | 22–29 |
| 8 LEDs | Queued floors | 49–42 |
| Joystick X / Y | Hall call: floor / direction | A1 / A2 |
| Rotary encoder | Emergency mode | 18, 19 |
| Encoder push button | Confirm hall call, reset emergency | 2 |
| Potentiometer | Simulated passenger load | A0 |
| LCD 16×2 | Floor, arrow, load, menu | RS 41, E 40, D4–D7 37–34, BL 4 |

## Repository structure

```
Heis_Program/
├── Heis_Program.ino   main loop: timing, connects all modules, logging
├── Heis.h / .cpp      state machine (six states)
├── Ko.h / .cpp        request queue, buttons, joystick, LEDs, emergency mode
├── Profil.h / .cpp    S-curve reference (trapezoid + moving average)
├── Regulator.h / .cpp PID controller and motor output
├── Posisjon.h / .cpp  encoder reading, conversion between counts and floors
├── door.h / .cpp      stepper motor door (OPEN / HALF / CLOSED)
├── dac.h / .cpp       MCP4922 DAC for the stepper current
├── Vekt.h / .cpp      simulated load and overload check
└── Display.h / .cpp   LCD output
```

## Team

Anders Ørnevik, Torjus Tuft and Axel Leon Gunnar Wraa  
Department of Engineering Sciences, University of Agder, Grimstad

## References

1. F. Sanfilippo, M. B. Økter, T. C. Eie and M. Ottestad, "Teaching Motion
   Control in Mechatronics Education Using an Open Framework Based on the
   Elevator Model," *Machines*, vol. 10, no. 10, p. 945, 2022.
   [doi:10.3390/machines10100945](https://doi.org/10.3390/machines10100945)
2. T. S. Hoang, "An Elevator System – Requirements Document," ECS,
   University of Southampton, 2018.
