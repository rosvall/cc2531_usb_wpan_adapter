# WPAN Adapter firmware for CC2531 USB Dongle

## Requirements
- CC2531 based USB dongle with [DFU bootloader](https://github.com/rosvall/cc2531_bootloader/)
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
