# LightScheduler

Arduino library for scheduling a PWM-controlled light with configurable on/off times and a gradual sunset dimming effect.

**Created by [Gonçalo Torres](https://github.com/IAMTorres)**

## What it does

- Turns a light **on at full brightness** at a configured time
- At the configured off time, begins a **smooth gradual fade** (simulating a natural sunset) over a configurable duration
- **EEPROM persistence** — schedule survives power cuts
- **Works with any RTC** — you pass the current hour and minute, the library handles the rest
- **Non-blocking** — uses `millis()` internally, never calls `delay()`

## Installation

### Arduino Library Manager
Search for **LightScheduler** and click Install.

### Manual
Download this repo as a ZIP and in the Arduino IDE go to:
`Sketch → Include Library → Add .ZIP Library`

## Quick Start

```cpp
#include <LightScheduler.h>

LightScheduler scheduler(9);  // PWM pin

void setup() {
    scheduler.setOnTime(8, 0);     // turn on  at 08:00
    scheduler.setOffTime(22, 0);   // dim start at 22:00
    scheduler.setDimDuration(60);  // 60-minute gradual fade
    scheduler.loadFromEEPROM();    // restore saved schedule on boot
}

void loop() {
    // Pass current time from your RTC
    scheduler.update(currentHour, currentMinute);
}
```

## API Reference

### Constructor

```cpp
LightScheduler(int pwmPin, uint16_t dimDurationMinutes = 60);
```

| Parameter | Description |
|-----------|-------------|
| `pwmPin` | Arduino pin connected to PWM input of your LED driver. Must support `analogWrite()`. |
| `dimDurationMinutes` | How long the gradual dimming takes. Default: 60 minutes. |

---

### Schedule Configuration

```cpp
void setOnTime(uint8_t hour, uint8_t minute);
void setOffTime(uint8_t hour, uint8_t minute);
void setDimDuration(uint16_t minutes);
```

Overnight schedules are supported (e.g. on at `20:00`, off at `06:00`).

---

### EEPROM Persistence

```cpp
void saveToEEPROM(int address = 0);
void loadFromEEPROM(int address = 0);
```

Stores the on/off schedule to EEPROM at the given address. Uses **4 bytes**.
If you have multiple schedulers, use different addresses (e.g. `0` and `4`).

---

### Main Loop

```cpp
void update(uint8_t currentHour, uint8_t currentMinute);
```

Call this every loop iteration with the current time from your RTC.
Handles full turn-on, gradual dimming and turn-off automatically.

---

### Status

```cpp
bool    isOn();          // true if light is on (including during dimming)
bool    isDimming();     // true if gradual dimming is in progress
uint8_t getBrightness(); // current PWM value (0–255)

ScheduleTime getOnTime();   // returns { hour, minute }
ScheduleTime getOffTime();  // returns { hour, minute }
```

---

## Wiring Example (DS1302 + L298N)

```
  ┌──────────┐       ┌─────────────────────────┐       ┌─────────────┐
  │  DS1302  │       │       Arduino Uno        │       │  LCD 16x2   │
  │  (RTC)   ├─D5───→ RST               SDA ───┼──────→│   (I2C)     │
  │          ├─D6───→ DAT               SCL ───┼──────→│             │
  │          ├─D7───→ CLK                      │       └─────────────┘
  └──────────┘       │                         │
                     │                D11 ─────┼──→ [Button 1]
                     │                D12 ─────┼──→ [Button 2]
                     │                         │
                     │          D2 (IN1) ──────┼──┐
                     │          D3 (IN2) ──────┼──┤    ┌──────────────────┐
                     │          D9 (ENA) ──────┼──┴───→│     L298N        │
                     └─────────────────────────┘       │                  │
                                               12V ───→│ VS          OUT+ ├──→ LED (+)
                                               GND ───→│ GND         OUT- ├──→ LED (-)
                                                        └──────────────────┘
```

## Examples

| Example | Description |
|---------|-------------|
| `BasicScheduler` | Minimal usage with a simulated software clock |
| `WithDS1302` | Full example with DS1302 RTC, EEPROM and buttons |

## How the Dimming Works

```
08:00                    22:00              23:00
  │                        │                  │
  │◄─── Full brightness ──►│◄─── 60-min dim ─►│ OFF
  │       PWM = 255        │   255 → 0        │ PWM = 0
```

At `offTime` the library begins a linear fade from PWM 255 to 0 over `dimDuration` minutes using `millis()`. The light only starts dimming if it was actually turned on during that day's cycle — preventing false triggers on boot.

## Real-World Use Cases

- Bird / reptile / chicken coop lighting
- Aquarium lighting cycles
- Indoor plant grow lights
- Greenhouse automation

## Used In

| Project | Description |
|---------|-------------|
| [arduino-bird-light](https://github.com/IAMTorres/arduino-bird-light) | Original bird presence light — DS1302 RTC, L298N motor driver, 16x2 LCD, 2-button menu, EEPROM persistence |
| [arduino-bird-light-v2](https://github.com/IAMTorres/arduino-bird-light-v2) | Rebuilt with state machine menu, always-visible status screen, separate hour/minute config and hold-to-fast-increment |

## Origin

This library was extracted and generalised from a personal project — a presence light built for a family member's bird that needed consistent daylight cycles. See [arduino-bird-light](https://github.com/IAMTorres/arduino-bird-light) for the original hardware project.

## Author

**Gonçalo Torres** — [github.com/IAMTorres](https://github.com/IAMTorres)

## License

MIT
