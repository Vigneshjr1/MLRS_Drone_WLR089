# mLRS SAMR34/WLR089U0 Port

Port of mLRS to the **Microchip WLR089U0 Xplained Pro** evaluation board (SAMR34J18B — ARM Cortex-M0+ with integrated SX1276 LoRa transceiver).

Both RX (receiver) and TX (transmitter) firmware are supported.

---

## Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Programming / Flashing](#programming--flashing)
- [VS Code Setup](#vs-code-setup)
- [Hardware Details](#hardware-details)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Testing & Verification](#testing--verification)
- [Troubleshooting](#troubleshooting)

---

## Overview

| Item | Detail |
|------|--------|
| MCU | ATSAMR34J18B (Cortex-M0+ @ 16MHz) |
| Radio | Integrated SX1276 LoRa transceiver |
| Flash | 256 KB |
| RAM | 32 KB + 8 KB LPRAM |
| Board | WLR089U0 Xplained Pro |
| Debugger | Onboard EDBG (CMSIS-DAP over USB) |
| Frequency | 868 MHz / 915 MHz |
| Build system | Makefile + arm-none-eabi-gcc |

---

## Prerequisites

### 1. ARM GCC Toolchain

Download and install **ARM GNU Toolchain** (arm-none-eabi):

- **Windows**: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
  - Install to default path, ensure `arm-none-eabi-gcc` is in your PATH
  - Verify: `arm-none-eabi-gcc --version`

- **Linux**: `sudo apt install gcc-arm-none-eabi`
- **macOS**: `brew install arm-none-eabi-gcc`

### 2. Make

- **Windows**: Comes with Git Bash (install Git for Windows), or install via `choco install make`
- **Linux/macOS**: Already installed

### 3. Programming Tool (one of these)

| Tool | Free? | Notes |
|------|-------|-------|
| **OpenOCD** | Yes | Recommended. Works with EDBG/CMSIS-DAP |
| **Atmel Studio 7** | Yes | Windows only. Full IDE with built-in programmer |
| **MPLAB IPE** | Yes | Microchip's standalone programmer |
| **pyocd** | Yes | Python-based. `pip install pyocd` |

### 4. VS Code (optional but recommended)

- Install **VS Code**: https://code.visualstudio.com/
- Install extensions:
  - **C/C++** (Microsoft)
  - **Cortex-Debug** (for debugging via OpenOCD)

---

## Building

### Build RX Firmware

```bash
cd mLRS/rx-wlr089u0-xpro
make
```

Output files in `build/`:
- `mlrs-rx-wlr089u0-xpro.elf` — Debug/flash image
- `mlrs-rx-wlr089u0-xpro.hex` — Intel HEX for programming
- `mlrs-rx-wlr089u0-xpro.bin` — Raw binary
- `mlrs-rx-wlr089u0-xpro.map` — Linker map

### Build TX Firmware

```bash
cd mLRS/tx-wlr089u0-xpro
make
```

Output: `build/mlrs-tx-wlr089u0-xpro.{elf,hex,bin}`

### Clean Build

```bash
make clean    # remove build directory
make          # rebuild from scratch
```

### Build Size Reference

| Target | Flash (text) | RAM (data+bss) |
|--------|-------------|-----------------|
| RX | ~48 KB (19% of 256 KB) | ~15 KB (47% of 32 KB) |
| TX | ~55 KB (21% of 256 KB) | ~16 KB (49% of 32 KB) |

---

## Programming / Flashing

Connect the WLR089U0 Xplained Pro board via USB to the **EDBG** port (not the target USB port).

### Option 1: OpenOCD (Recommended)

Install OpenOCD:
- **Windows**: `choco install openocd` or download from https://github.com/xpack-dev-tools/openocd-xpack/releases
- **Linux**: `sudo apt install openocd`
- **macOS**: `brew install openocd`

Flash the firmware:

```bash
# Flash RX
openocd -f interface/cmsis-dap.cfg -f target/at91samdXX.cfg \
  -c "program build/mlrs-rx-wlr089u0-xpro.elf verify reset exit"

# Flash TX
openocd -f interface/cmsis-dap.cfg -f target/at91samdXX.cfg \
  -c "program build/mlrs-tx-wlr089u0-xpro.elf verify reset exit"
```

If `at91samdXX.cfg` doesn't work, try creating a custom config or use:
```bash
openocd -f interface/cmsis-dap.cfg \
  -c "transport select swd" \
  -c "set CHIPNAME at91samd21" \
  -c "source [find target/at91samdXX.cfg]" \
  -c "program build/mlrs-rx-wlr089u0-xpro.elf verify reset exit"
```

### Option 2: Atmel Studio 7

1. Open **Tools > Device Programming**
2. Tool: **EDBG**
3. Device: **ATSAMR34J18B**
4. Interface: **SWD**
5. Click **Apply**, then **Read** (Device Signature) to verify connection
6. Go to **Memories** tab
7. Browse to `build/mlrs-rx-wlr089u0-xpro.hex`
8. Click **Program**

### Option 3: MPLAB IPE

1. Download from https://www.microchip.com/mplab/mplab-integrated-programming-environment
2. Select device: **ATSAMR34J18B**
3. Select tool: **EDBG**
4. Load `.hex` file
5. Click **Program**

### Option 4: pyocd

```bash
pip install pyocd
pyocd flash -t atsamr34j18b build/mlrs-rx-wlr089u0-xpro.hex
```

### Adding Flash Target to Makefile

You can also flash directly from make (requires OpenOCD):

```bash
make flash    # if flash target is added to Makefile
```

---

## VS Code Setup

### Recommended Extensions

- **C/C++** (`ms-vscode.cpptools`) — IntelliSense, code navigation
- **Cortex-Debug** (`marus25.cortex-debug`) — ARM debugging via OpenOCD

### Build Tasks

The `.vscode/tasks.json` file (created alongside this README) provides:

- **Build RX** (`Ctrl+Shift+B` > "Build RX")
- **Build TX** (`Ctrl+Shift+B` > "Build TX")
- **Clean RX** / **Clean TX**
- **Build & Flash RX** / **Build & Flash TX**

### Debugging

The `.vscode/launch.json` file provides:

- **Debug RX (OpenOCD)** — Build, flash, and debug the RX firmware
- **Debug TX (OpenOCD)** — Build, flash, and debug the TX firmware

To debug:
1. Connect the WLR089U0 Xplained Pro via USB
2. Press `F5` or go to **Run > Start Debugging**
3. Select "Debug RX (OpenOCD)" or "Debug TX (OpenOCD)"
4. Set breakpoints, step through code, inspect variables

---

## Hardware Details

### WLR089U0 Xplained Pro Board

```
+--------------------------------------------------+
|  WLR089U0 Xplained Pro                           |
|                                                    |
|  [USB EDBG]    [USB TARGET]                       |
|                                                    |
|  [LED0 PA19]   [LED1 PA18]   [SW0 PA28]          |
|                                                    |
|  +--EXT1 Header--+  +--EXT3 Header--+            |
|  | PA04 (TX)     |  |               |            |
|  | PA05 (RX)     |  |               |            |
|  | ...           |  |               |            |
|  +---------------+  +---------------+            |
|                                                    |
|  [WLR089U0 Module]                                |
|  +-Internal SX1276 Radio-+                        |
|  | SPI: SERCOM4          |                        |
|  | MOSI: PB30 (PAD2)    |                        |
|  | MISO: PC19 (PAD0)    |                        |
|  | SCK:  PC18 (PAD3)    |                        |
|  | CS:   PB31           |                        |
|  | RESET: PB15          |                        |
|  | DIO0: PB16 (EXTINT0) |                        |
|  | RF_SW: PA13          |                        |
|  | TCXO:  PA09          |                        |
|  +-----------------------+                        |
|                                                    |
+--------------------------------------------------+
```

### Pin Mapping

| Function | Pin | Notes |
|----------|-----|-------|
| **SPI MOSI** | PB30 | SERCOM4 PAD2 (to SX1276) |
| **SPI MISO** | PC19 | SERCOM4 PAD0 (from SX1276) |
| **SPI SCK** | PC18 | SERCOM4 PAD3 |
| **SPI CS** | PB31 | GPIO (chip select) |
| **SX1276 RESET** | PB15 | Active low reset |
| **SX1276 DIO0** | PB16 | EXTINT0, rising edge (RxDone/TxDone) |
| **RF Switch** | PA13 | LOW=TX (PA_BOOST), HIGH=RX (LNA) |
| **TCXO Power** | PA09 | Active high, 2ms stabilization |
| **UART TX** | PA04 | SERCOM0 PAD0 (serial port) |
| **UART RX** | PA05 | SERCOM0 PAD1 (serial port) |
| **LED0 (red)** | PA19 | Active low |
| **LED1 (green)** | PA18 | Active low |
| **Button SW0** | PA28 | Active low, pull-up |

### Clock Configuration

```
OSC16M (16 MHz) ──> GCLK0 ──> CPU, SERCOM, peripherals
                ──> GCLK2 (/16 = 1 MHz) ──> TC3 (rxclock), TC4 (micros)
XOSC32K (32.768 kHz) ──> GCLK1 ──> RTC (future low-power)
SysTick (16 MHz) ──> 1ms tick (millis32, doSysTask)
```

### Memory Map

```
0x00000000 +-----------------+
           | Flash (256 KB)  |
           |                 |
0x0003F000 | EEPROM (4 KB)   |  <-- mLRS parameter storage
0x00040000 +-----------------+

0x20000000 +-----------------+
           | RAM (32 KB)     |
           | .data + .bss    |
           | Stack (4 KB)    |
0x20008000 +-----------------+

0x30000000 +-----------------+
           | LPRAM (8 KB)    |  <-- retained in standby
0x30002000 +-----------------+
```

### Interrupt Priorities (RX)

| IRQ | Priority | Source |
|-----|----------|--------|
| SysTick | 15 (lowest) | 1ms tick |
| TC3 (CLOCK) | 10 | RX frame timing |
| SERCOM0 (UARTB) | 11 | Serial port |
| EIC (SX DIO0) | 13 | Radio interrupt |

---

## Architecture

### How the SAMR34 Port Fits into mLRS

mLRS has a layered architecture with platform-independent core logic and platform-specific HAL layers:

```
┌─────────────────────────────────────────────────┐
│              Core Application Logic              │
│   (FHSS, framing, ARQ, MAVLink, channel order)  │
│         mlrs-rx.cpp / mlrs-tx.cpp               │
├──────────┬──────────────┬───────────────────────┤
│  STM32   │     ESP      │       SAMR34          │  <-- Platform branches
│  glue.h  │  esp-glue.h  │   samr34-glue.h       │
├──────────┼──────────────┼───────────────────────┤
│stm32ll-  │   esp-lib/   │    samr34-lib/        │  <-- Peripheral drivers
│  lib/    │  esp-spi.h   │  samr34-spi.h         │
│          │  esp-uart.h  │  samr34-uartb.h       │
│          │  esp-delay.h │  samr34-delay.h       │
│          │  etc.        │  etc.                  │
├──────────┼──────────────┼───────────────────────┤
│ STM32    │  Arduino/    │    CMSIS bare-metal    │  <-- Hardware access
│ LL/HAL   │  ESP-IDF     │  (register-level)     │
├──────────┼──────────────┼───────────────────────┤
│ STM32F1  │  ESP8285     │    SAMR34J18B         │  <-- Silicon
│ STM32G4  │  ESP32       │   (WLR089U0 module)   │
│ STM32WL  │  ESP32-C3    │                       │
│ STM32L4  │  ESP32-S3    │                       │
└──────────┴──────────────┴───────────────────────┘
```

### Platform Selection

The `#if` chain in `mlrs-rx.cpp` / `mlrs-tx.cpp` selects the platform:

```cpp
#if defined ESP8266 || defined ESP32
  // ESP platform includes
#elif defined SAMR34
  // SAMR34 platform includes (samr34-glue.h, samr34-lib/*, samr34-timer.h, etc.)
#else
  // STM32 platform includes (default)
#endif
```

The board define (`RX_WLR089U0_XPRO` or `TX_WLR089U0_XPRO`) is set in the Makefile and flows through:

```
Makefile (-D RX_WLR089U0_XPRO)
  └─> device_conf.h → sets DEVICE_NAME, DEVICE_HAS_SX127x, frequency bands
       └─> hal.h → includes samr34/rx-hal-wlr089u0-xpro.h (pin defs, sx_* functions)
            └─> sx12xx_driver.h → selects Sx127xDriver (SX1276 driver)
```

### Radio Driver Stack

The integrated SX1276 is accessed over standard SPI (unlike STM32WL's internal SUBGHZSPI):

```
Sx127xDriver (sx127x_driver.h)       ← mLRS radio driver wrapper
  │ overrides SpiSelect/Deselect/Transfer
  │ calls spi_select(), spi_transfer(), etc.
  ▼
samr34-spi.h                          ← SERCOM4 SPI master driver
  │ bare-metal register access
  ▼
SERCOM4 → PB30(MOSI), PC19(MISO), PC18(SCK), PB31(CS)
  │
  ▼
Integrated SX1276 radio
```

---

## Project Structure

```
mLRS/
├── modules/samr34-lib/                    # SAMR34 platform library
│   ├── cmsis/                             # CMSIS device headers (74 files)
│   │   ├── samr34.h, samr34j18b.h         #   Device register definitions
│   │   ├── core_cm0plus.h                 #   ARM Cortex-M0+ core
│   │   ├── component/*.h (28 files)       #   Peripheral register structs
│   │   ├── instance/*.h (39 files)        #   Peripheral base addresses
│   │   └── pio/samr34j18b.h              #   Pin I/O definitions
│   └── src/                               # Bare-metal peripheral drivers
│       ├── samr34-peripherals.h           #   GPIO, clock init, NVIC
│       ├── samr34-spi.h                   #   SERCOM4 SPI (→ SX1276)
│       ├── samr34-uartb.h                 #   SERCOM UART (ISR + ring buffer)
│       ├── samr34-delay.h                 #   SysTick delay_us/ms/ns
│       ├── samr34-eeprom.h                #   NVM flash EEPROM emulation
│       ├── samr34-mcu.h                   #   MCU UID, bootloader
│       └── samr34-stack.h                 #   Stack watermark monitor
│
├── Common/hal/
│   ├── samr34-glue.h                      # Platform shims (IRQHANDLER, ATOMIC, main_main)
│   ├── samr34-timer.h                     # millis32, micros16 (SysTick + TC4)
│   ├── samr34-rxclock.h                   # tRxClock (TC3 compare/match)
│   ├── samr34-powerup.h                   # Power-up counter stub
│   ├── samr34/
│   │   ├── rx-hal-wlr089u0-xpro.h        # RX board HAL (pins, SPI, EIC, LEDs)
│   │   └── tx-hal-wlr089u0-xpro.h        # TX board HAL
│   ├── device_conf.h                      # [MODIFIED] added WLR089U0 entries
│   └── hal.h                              # [MODIFIED] added samr34/ includes
│
├── CommonRx/mlrs-rx.cpp                   # [MODIFIED] added SAMR34 include branch
├── CommonTx/mlrs-tx.cpp                   # [MODIFIED] added SAMR34 include branch
│
├── rx-wlr089u0-xpro/                      # RX build target
│   ├── Makefile                           #   Build: make (defines -D RX_WLR089U0_XPRO)
│   ├── main.cpp                           #   Clock config → main_main()
│   ├── startup_samr34.c                   #   Vector table, Reset_Handler
│   ├── system_samr34.c                    #   SystemInit, SystemCoreClock
│   ├── samr34j18b_flash.ld               #   Linker script (256K flash, 32K RAM)
│   └── build/                             #   Build output (.elf, .hex, .bin, .map)
│
└── tx-wlr089u0-xpro/                      # TX build target
    ├── Makefile                           #   Build: make (defines -D TX_WLR089U0_XPRO)
    ├── main.cpp
    ├── startup_samr34.c
    ├── system_samr34.c
    ├── samr34j18b_flash.ld
    └── build/
```

---

## Testing & Verification

Follow these steps in order after flashing the firmware:

### Step 1: Basic Boot

Flash the RX firmware and verify:
- LED0 (PA19) should blink — indicates the main loop is running
- Connect a USB-to-UART adapter to PA04 (TX) / PA05 (RX) on EXT1 header at 115200 baud
- You should see device name "WLR089U0 Xpro RX" on the serial output

### Step 2: SX1276 Radio Communication

The first sign the SPI → SX1276 link works is the firmware not hanging at radio init. If you add a debug read of register 0x42 (RegVersion), it should return `0x12`.

### Step 3: Pair with Existing mLRS Hardware

To verify the full telemetry link:
1. Flash the WLR089U0 board as **RX** (receiver)
2. Use an existing mLRS **TX** (e.g., STM32G4 or ESP32-based transmitter) on the same frequency band (868 or 915 MHz)
3. Both devices should bind and establish a link
4. Verify MAVLink telemetry flows through the serial port

### Step 4: Range / RF Test

- TCXO should be powered (PA09 high) — the WLR089U0 module has an integrated TCXO
- RF switch (PA13) toggles between TX (PA_BOOST) and RX (LNA) paths
- Expected output power: up to +17 dBm with PA_BOOST

---

## Troubleshooting

### Build fails: "arm-none-eabi-gcc: command not found"

Add the toolchain to your PATH:
```bash
export PATH="/c/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin:$PATH"
```
Or add it permanently in Windows Environment Variables.

### Build fails: "make: command not found"

Install make via Git Bash or Chocolatey:
```bash
choco install make
```

### OpenOCD: "Error: no device found"

- Ensure the USB cable is connected to the **EDBG** port (not the target USB port)
- Install the EDBG USB driver (comes with Atmel Studio, or use Zadig to set the driver to WinUSB)
- Try: `openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/at91samdXX.cfg`

### Firmware hangs at boot

- Check that the clock configuration in `main.cpp` matches your board's oscillator
- The WLR089U0 module has an integrated TCXO — ensure `TCXO_PWR_IO` (PA09) is driven high before radio init
- Verify the linker script stack size (4 KB) is sufficient — increase if hitting stack overflow

### SX1276 not responding (SPI fails)

- The internal SX1276 is on **SERCOM4**: MOSI=PB30, MISO=PC19, SCK=PC18, CS=PB31
- Verify pin mux is set to function F (SERCOM4) for MOSI/MISO/SCK
- Check SPI clock polarity: CPOL=0, CPHA=0 (SPI mode 0)
- SPI baudrate should be 2-8 MHz (configured as 4 MHz by default)

### Linker warnings about _close, _read, _write, _lseek

These are **normal** for bare-metal with `nosys.specs`. The referenced syscalls are never called at runtime.

---

## References

- [SAMR34 Datasheet](https://www.microchip.com/en-us/product/ATSAMR34J18)
- [WLR089U0 Module Datasheet](https://www.microchip.com/en-us/product/WLR089U0)
- [WLR089U0 Xplained Pro User Guide](https://www.microchip.com/en-us/development-tool/EV23M25A)
- [mLRS Project](https://github.com/olliw42/mLRS)
- [SX1276 Datasheet](https://www.semtech.com/products/wireless-rf/lora-connect/sx1276)
