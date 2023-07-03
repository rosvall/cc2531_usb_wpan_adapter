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


## Protocol
### Control endpoint
In addition to the standard USB 2.0 control requests, the following control endpoint requests are supported:

|                     | bmRequestType | bRequest | wValue                                       | wIndex | Data                                             |
|---------------------|---------------|----------|----------------------------------------------|--------|--------------------------------------------------|
| Read XDATA          | 0xC0          | 0x00     | Register or RAM Address                      | *D/C*  | Contents of RAM or register(s)                   |
| Write XDATA         | 0x40          | 0x01     | Register or RAM Address                      | *D/C*  | Data to be written, starting at address *wValue* |
| Read FIFO           | 0xC0          | 0x02     | FIFO Address                                 | *D/C*  | Contents of FIFO                                 |
| Write FIFO          | 0x40          | 0x03     | FIFO Address                                 | *D/C*  | Bytes to be written into specified address       |
| Transmit            | 0x40          | 0x04     | Non-zero: Disable CSMA, transmit immediately | *D/C*  | IEEE 802.15.4 frame to be written to radio FIFO  |
| Set CSMA parameters | 0x40          | 0x05     | (retries << 8)\|(be_max << 4)\|(be_min << 0) | *D/C*  | *D/C*                                            |
| DFU_DETACH          | 0x21          | 0x00     | *D/C*                                        | *D/C*  | *D/C*                                            |

*D/C*: Don't care

### Status endpoint
Endpoint 1 (Interrupt IN) sends one byte status messages to host. Transmit success (0) or failure (non-zero).

### Receive endpoint
Endpoint 5 (Bulk IN) sends received IEEE 802.15.4 frames to host.

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
