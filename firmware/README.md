# Vega Nest Firmware

This directory contains all firmware for the Vega Nest ecosystem: the Hardware
Abstraction Layer (HAL), reusable middleware, board support, drivers, and
example applications targeting C-DAC's VEGA RISC-V processors (THEJAS32 /
THEJAS64 SoCs, Aries development boards).

Coding conventions for everything under this directory are defined in CONTRIBUTING.md. Read that before contributing.

## Repository Structure

```
firmware/
├── app/                     Product-specific application logic, state
│                             machines, workflows. Never touches hardware
│                             registers directly.
|
├── bsp/                     Board init, clocks, GPIO muxing, startup code.
|
├── drivers/                 Low-level peripheral drivers (Display,Sensors,
│                              Flash). Reusable across
│                             projects.
├── hal/                     Hardware Abstraction Layer. Wraps VEGA SDK
│                             driver calls with a clean, validated API.
│   ├── gpio/                 hal_gpio.h / hal_gpio.c
│   └── timer/                hal_timer.h / hal_timer.c
├── middleware/               Reusable software components built on top of
│                             HAL/drivers (protocol stacks, OTA, crypto,
│                             file systems, periodic timers, etc).
│   └── periodic_timer/       periodic_timer.h / periodic_timer.c
├── network/                 TCP/IP, BLE, Wi-Fi, LoRa, RF Mesh, IPv6.
├── os/                       OS abstraction: tasks, queues, timers, mutexes.
├── services/                 Logging, config, diagnostics, power mgmt,
│                             telemetry, storage.
├── utilities/                Generic helpers: string utils, CRC, ring
│                             buffers, bit ops, math utils.
├── tests/                   Unit/integration tests, mocks. Never a
│                             dependency of production firmware.
└── example_applications/     Buildable example projects, one per feature.
    ├── gpio_example/
    │   └── basic_gpio/       GPIO HAL usage example.
    └── timer_example/
        ├── timer_interrupt/  HAL timer interrupt usage example.
        └── periodic_timer/   Periodic timer middleware usage example.
```

Each layer only depends on the layer(s) below it (`app → services →
middleware → network/os → drivers → bsp → hardware`) — see the Directory
Dependency Rules section of the coding standard for the full picture.

## How This Repo Links to the VEGA SDK

Vega Nest's HAL, middleware, and examples are built **on top of** the
official [VEGA SDK](https://gitlab.com/cdac-vega/vega-sdk) maintained by
C-DAC — this repo does not vendor or fork the SDK itself.

### 1. Install VEGA Tools and VEGA SDK

Follow C-DAC's official
[Installing VEGA SDK](https://cdac-vega.gitlab.io/sdkuserguide/installing.html)
guide. In short, on a supported Linux distro (Ubuntu 18.04/20.04):

```bash
# RISC-V toolchain, for THEJAS32 (Aries boards)
git clone https://gitlab.com/cdac-vega/vega-tools-rv32.git
cd vega-tools-rv32 && ./setup-env.sh && cd ..

# VEGA SDK itself
git clone https://gitlab.com/cdac-vega/vega-sdk.git
cd vega-sdk
git checkout aries
./setup.sh
```

`setup-env.sh`/`setup.sh` write out the environment configuration our
Makefiles expect at `~/.config/vega-tools/settings.mk`, defining
`VEGA_TOOLCHAIN_PATH` and `VEGA_SDK`.

### 2. How our Makefiles pick up the SDK

Every buildable project under `example_applications/` (and any new project
you create) follows the same pattern:

```makefile
CONFIG_PATH=~/.config/vega-tools/settings.mk
include $(CONFIG_PATH)
...
SDK_PATH=${VEGA_SDK}
include $(SDK_PATH)/bsp/common/config.mk
```

`config.mk` is the SDK's own build glue — it sets up the RISC-V toolchain
flags, defines `$(PROGRAM_ELF)`, `$(BIN)`, and auto-discovers `*.c` files
**only in the current project directory** (via `find .`).

Because our HAL and middleware sources live *outside* the project directory
(`firmware/hal/...`, `firmware/middleware/...`), `config.mk` never sees them
on its own. Each project's Makefile compensates by:

1. Adding an explicit `-I` include path for every HAL/middleware directory it needs.
2. Appending the corresponding `.o` files as extra prerequisites of `$(PROGRAM_ELF)`.
3. Providing a compile rule for each of those `.o` files.

See `example_applications/timer_example/periodic_timer/Makefile` for a
worked example that pulls in both `hal_timer.c` and `periodic_timer.c` this
way.

## Building a Project

```bash
cd firmware/example_applications/timer_example/periodic_timer
make
```

A successful build produces, inside a local `build/` directory:

| File | Contents |
| --- | --- |
| `*.elf` | Linked executable |
| `*.bin` | Raw binary — this is what gets flashed to the board |
| `*.hex` | Intel HEX format |
| `*.dump` | Disassembly, useful for debugging |
| `*.map` | Linker map |

Run `make clean` to remove a project's `build/` directory.

## Flashing / Running on the Target Board

Full reference: C-DAC's
[Running Program on Target Board](https://cdac-vega.gitlab.io/sdkuserguide/running.html)
guide.

### 1. Connect and open a serial session

Connect the board via micro USB, then open a serial terminal (the guide uses
`minicom`; any serial tool, e.g. `python-serial`, works):

```bash
sudo minicom aries
```

The correct serial device is usually `/dev/ttyUSB0`.

### 2. Reset the board

Press the board's **reset button** before every upload. On a good reset you
should see the VEGA bootloader banner (SoC/ISA info, transfer mode) print in
the terminal, followed by a prompt to send a file via XMODEM.

### 3. Choose an upload mode via the BOOT SEL switch

The Aries board's **BOOT SEL** switch selects how the bootloader accepts new
firmware:

- **BOOT SEL open → Upload over UART.** In `minicom`, press `Ctrl+A S`,
  choose `xmodem`, then select your project's `build/<name>.bin` file to
  send. The program starts executing as soon as the transfer finishes. This
  is the fastest path for day-to-day development — the binary runs directly
  from RAM/bootloader and is not persisted to flash.

- **BOOT SEL closed → Boot from Flash.** Use this to make firmware persist
  across power cycles:
  1. Download `flasher.bin` from C-DAC's site and place it in
     `vega-sdk/bin/`.
  2. In `minicom`, reset the board and send `flasher.bin` over XMODEM
     (`Ctrl+A S` → `xmodem`) — this erases flash and puts the board in a
     state ready to receive your program.
  3. Send your project's `build/<name>.bin` over XMODEM the same way.
  4. Place the jumper on header **J12 (BOOT_SEL)**.
  5. Reset the board — it now boots your program from flash.

## Further Reading

- [VEGA SDK User Guide](https://cdac-vega.gitlab.io/sdkuserguide.html) — full SDK docs (installing, building, creating a project, IDE integration).
- [Board Setup / Pin Mapping](https://cdac-vega.gitlab.io/boardsetup/pinmap.html) — reset button and BOOT SEL switch locations per board.
- [`Embedded Firmware Coding Standard.docx`](../Embedded_Firmware_Coding_Standard.docx) — coding, review, and CI requirements for everything in this directory.
