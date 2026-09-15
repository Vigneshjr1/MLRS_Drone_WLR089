# mLRS SAMR34/WLR089U0 Build and Test Guide

This guide walks you through building, flashing, and testing the mLRS port for the Microchip WLR089U0 Xplained Pro evaluation board (ATSAMR34J18B with integrated SX1276 LoRa radio).

## Hardware Required

| Item | Qty | Notes |
|------|-----|-------|
| WLR089U0 Xplained Pro | 2 | One for TX, one for RX |
| Micro-USB cables | 2 | Powers the board and provides EDBG programming/debug |
| UART-USB adapter (optional) | 1-2 | To connect serial port on EXT1 header to a PC or flight controller |

## Software Prerequisites

### 1. ARM GCC Toolchain

Download from [ARM Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). Any recent release supporting Cortex-M0+ will work.

Verify installation:
```
arm-none-eabi-gcc --version
```

### 2. GNU Make

- **Windows:** Available via Git Bash, or install with `choco install make`
- **Linux/macOS:** Usually pre-installed (`sudo apt install build-essential` on Ubuntu)

### 3. OpenOCD

Used for flashing via the onboard EDBG (CMSIS-DAP) debugger.

- **Windows:** Download from [OpenOCD releases](https://github.com/xpack-dev-tools/openocd-xpack/releases) or `choco install openocd`
- **Linux:** `sudo apt install openocd`
- **macOS:** `brew install openocd`

## Building

### Build the TX firmware

```
cd mLRS/tx-wlr089u0-xpro
make
```

Output files in `build/`:
- `mlrs-tx-wlr089u0-xpro.elf`
- `mlrs-tx-wlr089u0-xpro.hex`
- `mlrs-tx-wlr089u0-xpro.bin`

### Build the RX firmware

```
cd mLRS/rx-wlr089u0-xpro
make
```

Output files in `build/`:
- `mlrs-rx-wlr089u0-xpro.elf`
- `mlrs-rx-wlr089u0-xpro.hex`
- `mlrs-rx-wlr089u0-xpro.bin`

### Clean rebuild

```
make clean && make
```

### Expected build sizes

| Target | Flash (text) | RAM (data+bss) |
|--------|-------------|-----------------|
| RX | ~48 KB (19% of 256 KB) | ~15 KB (47% of 32 KB) |
| TX | ~55 KB (21% of 256 KB) | ~16 KB (49% of 32 KB) |

## Flashing

Connect the WLR089U0 Xplained Pro board via USB. The onboard EDBG debugger appears as a CMSIS-DAP device.

### Flash TX board

```
openocd -f interface/cmsis-dap.cfg -f target/at91samdXX.cfg \
  -c "program mLRS/tx-wlr089u0-xpro/build/mlrs-tx-wlr089u0-xpro.elf verify reset exit"
```

### Flash RX board

```
openocd -f interface/cmsis-dap.cfg -f target/at91samdXX.cfg \
  -c "program mLRS/rx-wlr089u0-xpro/build/mlrs-rx-wlr089u0-xpro.elf verify reset exit"
```

If you have both boards connected simultaneously, you may need to specify the serial number:

```
openocd -f interface/cmsis-dap.cfg -c "cmsis_dap_serial <SERIAL>" \
  -f target/at91samdXX.cfg \
  -c "program <path-to-elf> verify reset exit"
```

The serial number is printed on the board label or can be found via `openocd -f interface/cmsis-dap.cfg -c "cmsis_dap_serial; exit"`.

## Default Configuration

Both boards ship with these defaults (set at compile time):

| Parameter | Value |
|-----------|-------|
| Bind phrase | `mlrs.0` |
| Frequency band | 868 MHz |
| Mode | MODE_19HZ (19 Hz, SX127x) |
| TX serial baud | 57600 |
| RX serial baud | 57600 |
| TX channels source | None (MAVLink telemetry bridge) |
| RX radio status | PX4 format |
| RF power (default) | 10 dBm (10 mW) |

Available RF power levels: 0 dBm (1 mW), 10 dBm (10 mW), 17 dBm (50 mW).

The bind phrase must match on both TX and RX for them to connect. To change it, modify `SETUP_TX_BIND_PHRASE` and `SETUP_RX_BIND_PHRASE` in the source and rebuild.

## Serial Port Wiring (EXT1 Header)

The UART is on SERCOM0, exposed on the EXT1 header:

| Signal | Pin | EXT1 Header | Direction |
|--------|-----|-------------|-----------|
| UART TX | PA04 | EXT1 pin 13 | Board -> Host |
| UART RX | PA05 | EXT1 pin 14 | Host -> Board |
| GND | -- | EXT1 pin 19 or 20 | Common ground |

Connect a UART-USB adapter (3.3V logic) to these pins to communicate with a PC terminal or flight controller.

**Note:** The onboard EDBG Virtual COM Port (the USB connector) is used for debug output if enabled, but this port is **not** the mLRS serial link. The mLRS data serial is on EXT1 as described above.

## Verifying the Link

1. Flash TX firmware to one board and RX firmware to the other
2. Power both boards via USB
3. Watch the LEDs:
   - **Red LED (PA19):** Blinks during searching, solid or slow blink when connected
   - **Green LED (PA18, TX only):** Additional status indicator
4. Open a serial terminal (57600 baud) on the EXT1 UART of either board to observe MAVLink telemetry traffic once connected
5. Both boards should reach CONNECTED state within a few seconds if using the same bind phrase and frequency band

## Testing with a Flight Controller

For a full end-to-end test:

```
Flight Controller (TELEM port, 57600 baud)
    |
    | UART (3.3V)
    v
mLRS TX board (EXT1 header, PA04/PA05)
    |
    | 868 MHz radio link
    v
mLRS RX board (EXT1 header, PA04/PA05)
    |
    | UART (3.3V)
    v
GCS (QGroundControl / Mission Planner, 57600 baud)
```

1. Connect the TX board's EXT1 UART to the flight controller's TELEM port
2. Connect the RX board's EXT1 UART to your GCS computer via a UART-USB adapter
3. Open QGroundControl or Mission Planner at 57600 baud on the RX adapter's COM port
4. Power up both boards — you should see telemetry data flowing through

## Known Hardware Quirks

| Issue | Details |
|-------|---------|
| **GCLK2 divider broken** | The GCLK2 frequency divider does not work reliably on this MCU. All peripherals use GCLK0 (16 MHz) with TC prescalers instead. Do not attempt to use GCLK2 with a divider. |
| **TCXO requires manual init** | The WLR089U0 uses a TCXO instead of a crystal for the SX1276. Register 0x4B must be written with 0x19 after every radio reset, or the PLL will not lock. This is handled automatically in `sx_post_reset_init()`. |
| **EIC uses ULP32K clock** | GCLK routing to the External Interrupt Controller is broken. EIC is clocked from the internal ULP 32 kHz oscillator (`CKSEL=1`) instead. |
| **connect_sync_cnt_max = 15** | Overridden from the default of 5 to allow more frames for the setup data exchange handshake. May be reducible once link reliability is confirmed in your environment. |

## Troubleshooting

**OpenOCD can't find the board:**
- Ensure the USB cable supports data (not charge-only)
- Check that the EDBG drivers are installed (the board should appear as a CMSIS-DAP device)
- On Windows, you may need to install WinUSB drivers via [Zadig](https://zadig.akeo.ie/)

**Build fails with "arm-none-eabi-gcc: not found":**
- Ensure the ARM GCC toolchain `bin/` directory is in your system PATH

**Boards don't connect:**
- Verify both boards are flashed with matching firmware (same bind phrase and frequency band)
- Check that both boards are powered and the red LED is blinking (searching)
- Ensure adequate antenna or keep boards within a few meters of each other for testing
- If using 915 MHz FCC, both boards must be compiled with the same frequency band selection

**No serial data on EXT1:**
- Confirm you are connected to PA04 (TX out) and PA05 (RX in) on EXT1, not the EDBG USB virtual COM port
- Check baud rate is 57600
- Verify the UART-USB adapter uses 3.3V logic levels (not 5V)

**Radio PLL does not lock / no RF activity:**
- This is the TCXO init issue. Ensure `sx_post_reset_init()` is being called after radio reset. Check that TCXO_PWR_IO (PA09) is driven high before radio initialization.
