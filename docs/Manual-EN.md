# OS-Flux Manual

![](boardoverviewfront.png) ![](boardoverviewrear.png)

---

## Introduction

The OS-Flux is a compact DCC decoder built around an ATtiny85 microcontroller. It has four LED/output pins and fits directly into a signal housing or a locomotive.

It ships in two distinct roles depending on which firmware is flashed:

| Firmware | Role |
|---|---|
| **Flux-Signal** | Static signal decoder — controls up to 4 LED aspects via DCC extended signal commands |
| **Flux-Mobile** | Mobile function decoder — controls up to 4 auxiliary outputs on a locomotive via DCC function packets |

Choose your firmware before programming. The two hex files are distributed separately.

---

## Board Overview

The board has **two snap-away tabs**.

![](snapaway.png)
*The two breakaway tabs*

### Top tab — programming connector

This is the ICSP connector used to flash firmware onto the ATtiny85. It is only needed during programming. **Snap it off afterwards** — it serves no purpose in the field and is cleaner without it.

See the full programming guide here:
[Flashing an OS Decoder with firmware](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)

### Bottom tab — address button + JST connector

This tab carries:

- A **push button** for setting the DCC address
- A **JST connector** for easy DCC bus connection

**Keep this tab** on signal decoder installations. The JST connector lets you plug and unplug the signal from the DCC bus, making removable signals straightforward.

For mobile (locomotive) use, the bottom tab can be snapped off too if it is in the way.

---

## Rear solder pads

![](rearsidesolderpads.png)
*Rear side solder pads*

The rear of the board exposes solder pads for the four outputs. These are the same electrical nodes as the front-side pads — use whichever side suits your installation.

---

## Programming

1. Choose **Flux-Signal.hex** or **Flux-Mobile.hex** from the releases page.
2. Connect the programming tab to an Arduino running ArduinoISP. Full wiring and instructions:
   [Flashing an OS Decoder with firmware](https://github.com/Open-Source-Model-Railway-Electronics/docs/blob/master/Program_OS-DCC-Decoder/Program_OS-DCC-Decoder-EN.md)
3. Flash the hex file.
4. **Snap off the programming tab.**

---

## Flux-Signal — Signal Decoder

### How it works

The Flux-Signal decoder controls four LED outputs. The decoder does not know or care what colour each LED is — that depends entirely on how you wire your signal. The bit positions simply map to the four physical output pads:

| Bit position | Output pad |
|---|---|
| Bit 0 | LED 0 |
| Bit 1 | LED 1 |
| Bit 2 | LED 2 |
| Bit 3 | LED 3 |

The decoder listens for **DCC extended signal commands** (`notifyDccSigOutputState`) addressed to its configured address. The 8-bit state value in that command directly controls which LEDs are on and which blink:

| State byte bits | Function |
|---|---|
| Bits 3:0 (lower nibble) | Aspect mask — which outputs are on |
| Bits 7:4 (upper nibble) | Blink mask — which of those outputs blink instead of staying steady |

Set a bit in the aspect mask to turn that output on. Set the same bit in the blink mask to make it blink instead of staying steady. Outputs whose aspect bit is 0 are off, regardless of the blink mask.

Blink period is **333 ms**.

### Composing the state byte

The easiest way to build a state value is to use the **Windows Calculator in Programmer mode with Binary display**. Switch to BIN and type the 8 bits directly — the upper four bits are the blink mask, the lower four are the aspect mask. The calculator instantly shows the HEX value to send.

![](calculator.png)
*Windows Calculator — Programmer mode, Binary view. Upper nibble = blink mask, lower nibble = aspect mask.*

The example above shows `0010 1011` (hex `0x2B`):

| Nibble | Bits (4 3 2 1) | Meaning |
|---|---|---|
| Upper `0010` | bit 1 set | LED 1 blinks |
| Lower `1011` | bits 0, 1, 3 set | LED 0 on, LED 1 on, LED 2 off, LED 3 on |

Result: LED 0 steady on — LED 1 blinking — LED 2 off — LED 3 steady on.

While the decoder is waiting to learn a new address, all four LEDs blink to indicate this mode.

### Default settings

| Setting | Default |
|---|---|
| DCC address | 1 |
| Command mode | DCC extended (`dccExt = 1`) |

### Setting the DCC address

Hold the **button on the bottom tab** for **1 second**. All LEDs start blinking — the decoder is now waiting for an address.

Send any DCC command to the address you want to assign:

- If you send a **DCC extended signal command** — the decoder stores that address and stays in extended mode (`dccExt = 1`). This is normal operation.
- If you send a **DCC accessory turnout command** — the decoder stores that address and switches to turnout mode (`dccExt = 0`). In this mode the decoder no longer responds to extended signal commands. Useful if your command station cannot send extended signal commands.

The new address and mode are saved to EEPROM immediately.

---

## Flux-Mobile — Mobile Function Decoder

### How it works

The Flux-Mobile firmware registers as a **multifunction (locomotive) DCC decoder**. It has four auxiliary outputs that can each be linked to a DCC function number and configured with a range of behaviours via CV programming.

Default address is **3** (short address).

### Default output configuration

On a fresh board the four outputs behave as conventional front/rear headlights plus two function-controlled auxiliaries:

| Output | Pin | Default behaviour |
|---|---|---|
| Aux 0 | 3 | Front headlight — on when driving forward, logically inverted (active-low LED) |
| Aux 1 | 4 | Rear headlight — on when driving in reverse |
| Aux 2 | 1 | Controlled by F1 |
| Aux 3 | 0 | Controlled by F2 |

### DCC address

Set via standard CV programming on the DCC programming track:

| CV | Function |
|---|---|
| CV 1 | Short address (1–127) |
| CV 17 + CV 18 | Long address (write CV17 first, then CV18) |
| CV 8 = 8 | Factory reset |

### Output CV table

Each of the four outputs has a block of 10 CVs starting at CV 30:

| Output | CV block start |
|---|---|
| Aux 0 | CV 30 |
| Aux 1 | CV 40 |
| Aux 2 | CV 50 |
| Aux 3 | CV 60 |

Within each block:

| Offset | CV (Aux 0 example) | Name | Description |
|---|---|---|---|
| +0 | CV 30 | `onDirChange` | 1 = output follows the loco direction bit |
| +1 | CV 31 | `inverted` | 1 = output is logically inverted |
| +2 | CV 32 | `number` | DCC function number that controls this output (0 = none) |
| +3 | CV 33 | `bleeps` | Number of bleeps (under development) |
| +4 | CV 34 | `bleepOn` | Bleep on-time (under development) |
| +5 | CV 35 | `bleepOff` | Bleep off-time (under development) |
| +6 | CV 36 | `pulseTime` | Pulse duration in units of 100 ms (0 = disabled) |
| +7 | CV 37 | `slaved` | Mirror another output: set to the source output pin number (0 = disabled) |
| +8 | CV 38 | *(reserved)* | — |
| +9 | CV 39 | `state` | Last known output state — updated automatically, do not write |

### Output behaviour logic

Each output runs the following priority chain every loop:

1. **Slaved** (`CV+7 ≠ 0`) — output mirrors the pin specified in this CV, optionally inverted.
2. **Pulsed** (`CV+6 ≠ 0`) — output fires a single pulse of `pulseTime × 100 ms` on each rising edge of the controlling function.
3. **Steady** — output follows the function state, optionally inverted via `CV+1`.

The `onDirChange` flag (`CV+0`) XORs the output with the current loco direction, allowing the same function to control different outputs depending on travel direction — which is how the default front/rear headlights work.
