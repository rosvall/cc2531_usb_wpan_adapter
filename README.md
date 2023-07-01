# WPAN Adapter firmware for CC2531 USB Dongle

Firmware for Texas Instruments CC2531 Dongle to be used with [this Linux kernel driver for CC2531 WPAN Adapter firmware](https://github.com/rosvall/cc2531_linux).

Enables a CC2531 dongle to be used as a proper WPAN adapter through the Linux WPAN subsystem.


## Features
- All packets received are send to USB host.
- Transmit with optional IEEE 802.15.4 CSMA.
- Read/write all registers over USB.
- 2Mbaud 1N8 uart log output on pin P1.6
- SPI-like real time TX/RX packet sniffer output (tx: P1.3, data: P1.4, clock: P1.5)
- Supports USB Device Firmware Upgrade (with [DFU bootloader](https://github.com/rosvall/cc2531_bootloader))


## Requirements
- [dfu-util](https://sourceforge.net/projects/dfu-util/)
- CC2531 based USB dongle with [DFU bootloader](https://github.com/rosvall/cc2531_bootloader/).

    A stock CC2531USB-RD dongle can be flashed with the DFU bootloader using [this hack](https://github.com/rosvall/cc2531_oem_flasher)


## Build requirements
- [SDCC](https://sourceforge.net/projects/sdcc/)
- [dfu-util](https://sourceforge.net/projects/dfu-util/)
- [binutils](https://www.gnu.org/software/binutils/)
- [make](https://www.gnu.org/software/make/)


## How to build
```sh
# Check out repo with all sub-modules:
git clone --recursive 'https://github.com/rosvall/cc2531_usb_wpan_adapter.git' 
cd cc2531_usb_wpan_adapter

# Build
make

# Flash to USB dongle using device firmware upgrade
make download
```


## How to flash DFU image to CC2531 dongle running [DFU bootloader](https://github.com/rosvall/cc2531_bootloader/).
Requires [dfu-util](https://sourceforge.net/projects/dfu-util/).

```sh
dfu-util -D wpan_fw.dfu
```


## See also
 - [Flash a stock Texas Instruments CC2531USB-RD dongle, no tools required](https://github.com/rosvall/cc2531_oem_flasher)
 - [Simple USB DFU bootloader for TI CC2531](https://github.com/rosvall/cc2531_bootloader)
 - [Linux kernel driver for CC2531 WPAN Adapter firmware](https://github.com/rosvall/cc2531_linux)
