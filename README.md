<div align="center">

<img src="media/photos/final-version-01.png" width="360">

# Wash Buddy

An interactive robotic companion that gamifies hand-washing routines for children, guiding each step through facial expressions, organic motion, and RFID-triggered audio feedback.

[![License: CC BY-NC 4.0](https://img.shields.io/badge/License-CC%20BY--NC%204.0-lightgrey.svg)](LICENSE)
![Status](https://img.shields.io/badge/status-completed-6f42c1)

[![C++](https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-F5822A?logo=platformio&logoColor=white)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Arduino-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)

[Portuguese](README.pt.md) | English

</div>

## About

**Wash Buddy** is a standalone interactive robot: a child taps RFID-tagged toys (soap, sponge, towel) against it to advance through the wash cycle, and it responds natively through an OLED face, synchronized voice lines, and arm/head gestures — no smartphone or app involved.

The design was built in close collaboration with Design team **D04**, pairing a custom two-tier PCB with a firmware architecture tuned for expressive, real-time feedback.

## How it works

The wash cycle is a strict linear sequence, one physical tag per stage: **Idle → Wet → Soap → Scrub → Rinse → Dry → Success**. Four RFID tags drive it — `FAUCET` (start/rinse), `SOAP`, `SCRUB`, and `TOWEL` — each embedded in a real wash-themed toy.

1. **Idle.** The robot waits, blinking and glancing around. Every 30s of inactivity it flashes a faucet icon as a reminder. After 2 minutes with no tag, it shuts itself down to save battery.
2. **Tag validation.** Each ritual state expects exactly one "repeat" tag (the one that opened it) and one "next" tag (the one that advances the cycle). Reading the _current_ tag again replays the stage; reading the _next_ tag advances it; anything else — wrong tag, wrong order — drops straight to an `ERROR` state with corrective audio and a visual cue.
3. **Repeat guard.** A child can only repeat a stage once before the robot forces `ERROR` — this stops the ritual from looping forever on a single step, while still tolerating one honest retry.
4. **Per-stage timeouts.** Every stage has its own budget tuned to the task: Wet/Soap 12s, Scrub 25s (Scrub's floor follows the WHO/CDC 20-second hand-washing guideline), Rinse 20s, Dry 25s. Timing out drops into `WAITING`, which nudges the child with look-around animations, a "sad" mood after 15s, and a repeated audio reminder every 15s, for up to 45s before it gives up.
5. **Success and reset.** Completing Dry triggers a confetti-and-wave celebration, then resets ritual progress so the next child can start clean from Idle.

A physical debug button bypasses all of this for testing: a long press (1.5s) toggles a debug overlay, and short presses then cycle through every state in order — used during design sessions to tune choreography and audio without needing a real tag.

## Features

- **Expressive OLED face** — eyes, blinking, and idle look-around are computed procedurally at ~50 FPS by the `esp32-eyes`-based `ExpressionEngine`, not played back from pre-rendered frames.
- **Vignette-driven idle personality** — `BehaviorEngine` plays "vignettes" (`BehaviorVignette`): a mood, a look direction, per-axis servo choreography, and an optional sound, picked from a per-state pool with randomized rest pauses. It auto-pauses whenever an instruction icon is on screen, so idle fidgeting never distracts from the current step.
- **Custom particle system** — a fixed 40-particle pool renders bubbles, light/heavy rain, confetti, and wind effects, each with independent spawn-chance and limit tuning per `EffectType`.
- **Organic motion** — three SG90 servos (head + two arms), each modeled as an independent `MotorAxis` with smoothstep easing, per-axis angle clamping, and optional left/right inversion for mirrored arm choreography.
- **Resilient audio driver** — `AudioController` runs its own init state machine (`WAKING_UP → CONNECTING → SETTING_MODE → READY`) against the DFPlayer Pro over UART, so a slow-booting player never blocks the main loop; volume tracks a potentiometer with debounced ADC reads.
- **Physical auto-shutdown** — a MOSFET cuts battery power to the servos and, on shutdown, to the whole system via the charger module's `KEY` pin — eliminating standby drain from idle motors and preserving battery between sessions.

## Architecture

**Software** — modular C++ (PlatformIO/Arduino framework) split into a hardware abstraction layer (RFID, servos, audio, power) and a behavior/interface layer (particles, facial animation, idle motion):

- **State pattern** drives the wash-cycle FSM (`include/states/`, one class per stage, `RobotState` enum) through `GameController`, which owns a static pool of pre-allocated state instances (no runtime `new`/`delete` during play) and centralizes the repeat-guard and timeout rules described above.
- **Options pattern** for controller configuration (e.g. `AudioSettings`, `MotionSettings`, `DisplayConfig`), avoiding magic numbers scattered through the code.
- **FreeRTOS dual-core split** — Core 0 runs `DisplayTask` exclusively, driving `DisplayOrchestrator` at a fixed ~60 FPS frame interval so face rendering and particle effects never stutter; Core 1 runs the Arduino `loop()` (state logic, servo kinematics) plus a dedicated `RFIDTask` and `audioTask`, decoupled from Core 1's `loop()` via a FreeRTOS queue for tag UIDs.
- **`DisplayOrchestrator`** composites `ExpressionEngine` (eyes) and `ParticleSystem` into a single U8g2 buffer, with a curtain-style open/close transition whenever an instruction icon replaces the face, so nothing ever pops in or out abruptly.

**Hardware** — an ESP32-WROOM-32 orchestrates the peripherals (RC522 over SPI, SH1106 OLED over I2C at 800 kHz, DFPlayer Pro over UART, servos over PWM) on a stacked two-tier custom PCB: a lower power module (DFR1026 charger + MOSFET isolation) and an upper logic module, physically separated so motor electrical noise doesn't corrupt the SPI/I2C buses. Servos are powered directly from the battery (bypassing the charger module's current limit), gated by a MOSFET to eliminate holding-torque standby drain; the same MOSFET pattern drives auto-shutdown by pulling the charger module's `KEY` pin low.

| Component                      | Role                          |
| ------------------------------ | ----------------------------- |
| ESP32-WROOM-32                 | Main microcontroller          |
| RC522                          | RFID reader (13.56 MHz tags)  |
| OLED 1.3" 128×64 (SH1106, I2C) | Face / expression display     |
| DFPlayer Pro                   | Audio playback + 2× speakers  |
| 3× SG90 servo                  | Head + arm actuators          |
| DFR1026                        | Battery charging/power module |
| 2× 2200 mAh LiPo               | Power (4400 mAh total)        |

<table>
<tr>
<td width="50%" valign="top"><img src="media/photos/power-01.png" width="100%"><br>Power layer — hand-wired connections to the charger and MOSFET isolation circuit</td>
<td width="50%" valign="top"><img src="media/photos/logic-01.png" width="100%"><br>Logic layer — ESP32-WROOM-32 and DFPlayer Pro</td>
</tr>
<tr>
<td width="50%" valign="top"><img src="media/photos/power-02.png" width="100%"><br>Power layer — 1000 µF filter capacitors and MOSFET heatsink</td>
<td width="50%" valign="top"><img src="media/photos/logic-02.png" width="100%"><br>Logic layer — USB-C ports for ESP32 programming and DFPlayer Pro config</td>
</tr>
<tr>
<td width="50%" valign="top"><img src="media/photos/power-03.png" width="100%"><br>Power layer, side view</td>
<td width="50%" valign="top"><img src="media/photos/logic-03.png" width="100%"><br>Logic layer, side view</td>
</tr>
<tr>
<td colspan="2"><img src="media/photos/stack-01.png" width="100%"><br>The two-tier stack assembled — logic board on top, power board below</td>
</tr>
</table>

## Requirements

| Tool                                  | Minimum version |
| ------------------------------------- | --------------- |
| [PlatformIO](https://platformio.org/) | Core 6+         |

Board and library dependencies (`MFRC522`, `ESP32Servo`, `U8g2`, `DFRobot_DF1201S`) are declared in `platformio.ini` and installed automatically by PlatformIO.

## How to run

```bash
cd src/iot
pio run --target upload   # build and flash to the ESP32
pio device monitor        # serial log at 115200 baud
```

## Tests

```bash
cd src/iot
pio test -e esp32dev
```

The Unity suite (`test/test_logic/`) runs on-device and covers the motion easing math, servo axis clamping/inversion, and choreography step sequencing.

## Project structure

```
wash-buddy/
├── src/iot/            # PlatformIO firmware project
│   ├── include/        # Global headers, per-state headers, pins/audio constants
│   ├── lib/             # Engines and controllers (Behavior, Particle, Expression,
│   │                     #   Motion, Audio, Power, RFID, Display)
│   ├── src/             # main.cpp entry point, GameController, state implementations
│   └── test/            # Unity test suite
├── media/               # Milestone reports, BOMs, circuit diagrams, photos, sounds
└── LICENSE
```

## Documentation

#### Milestone 1

- [Report](media/milestone-1/report.pdf) — initial project proposal and requirements.

#### Milestone 2

- [Report](media/milestone-2/report.pdf) — architecture, hardware rationale, and task breakdown.
- [Circuit diagram (D04)](media/milestone-2/circuit-d04.pdf)
- [Bill of materials (D04)](media/milestone-2/bom-d04.xlsx)
- [Assembly manual](media/milestone-2/assembly-manual.png)
- [Physical model photo](media/milestone-2/physical-model.jpeg)
- [Demo video](media/milestone-2/video.mp4)

#### Sounds

- [Sound library](media/sounds/) — all voice lines and effects used by the firmware.

#### Photos

- [Photos](media/photos)

## Team

- [Nycolas Souza](https://github.com/nycocado) — firmware (C++/PlatformIO), FreeRTOS parallelization, visual engine and servo choreography math.
- [Luan Ribeiro](https://github.com/Ninjaok) — hardware engineering and circuit design; MOSFET isolation and auto-shutdown circuit.
- [Lohanne Guedes](https://github.com/lohanneguedes) — physical prototyping and lab integration; PCB soldering and chassis assembly.
- [Kira Sousa](https://github.com/Kira-Sousa) — state-machine business rules and sound design; audio curation and integration.

## License

Distributed under **CC BY-NC 4.0**, © 2026 Nycolas Souza, Luan Ribeiro, Lohanne Guedes, Kira Sousa.

Others may share and adapt this work with attribution, for non-commercial purposes only. The full text is in [LICENSE](LICENSE).
