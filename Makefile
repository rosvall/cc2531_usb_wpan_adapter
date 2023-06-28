# Needed for both dfu-suffix and compilation
USB_VID     ?= 0x1608
USB_PID     ?= 0x154f


# Bootloader takes up first 2 kB
CODE_OFFSET  = 0x800


# Toolchain
AS           = sdas8051
AR           = sdar8051
LD           = sdld
CC           = sdcc
CPP			 = sdcpp


# Toolchain flags
ASFLAGS      = -pwg
CFLAGS       = -mmcs51 --Werror --fomit-frame-pointer --fverbose-asm --code-loc $(CODE_OFFSET)


# Sources
C_FILES      = $(wildcard *.c)
S_FILES      = $(wildcard *.S)


# Generated files
DEP_FILES	 = $(C_FILES:.c=.d)
I_FILES      = $(C_FILES:.c=.i)
ASM_FILES    = $(C_FILES:.c=.asm) $(S_FILES:.S=.asm)
LST_FILES	 = $(ASM_FILES:.asm=.lst)
REL_FILES	 = $(ASM_FILES:.asm=.rel)
SYM_FILES	 = $(ASM_FILES:.asm=.sym)
MAP_FILES	 = $(ASM_FILES:.asm=.map)
MEM_FILES	 = $(ASM_FILES:.asm=.mem)
RST_FILES	 = $(ASM_FILES:.asm=.rst)
LST_FILES	 = $(ASM_FILES:.asm=.lst)
UPLOADED_BIN = uploaded.bin
IMG_BIN      = img.bin
IMG_IHX		 = $(IMG_BIN:.bin=.ihx)
IMG_DFU      = $(IMG_BIN:.bin=.dfu)
IMG_MEM      = $(IMG_BIN:.bin=.mem)
LK_FILES     = $(IMG_BIN:.bin=.lk)
GENERATED    = $(DEP_FILES) $(I_FILES) $(ASM_FILES) $(LST_FILES) $(REL_FILES) \
               $(SYM_FILES) $(MAP_FILES) $(MEM_FILES) $(RST_FILES) \
               $(LST_FILES) $(IMG_IHX) $(IMG_BIN) $(IMG_DFU) $(IMG_MEM) \
               $(LK_FILES) $(UPLOADED_BIN)


# Version tag
TAG_COMMIT   = $(shell git rev-list --abbrev-commit --tags --max-count=1)
TAG          = $(shell git describe --abbrev=0 --tags ${TAG_COMMIT} 2>/dev/null || true)
COMMIT       = $(shell git rev-parse --short HEAD)
DATE         = $(shell git log -1 --format=%cd --date=format:"%Y%m%d")
VERSION      = $(TAG:v%=%)
VERSION_PARTS := $(subst ., ,$(VERSION))
VERSION_MAJOR := $(word 1, $(VERSION_PARTS))
VERSION_MINOR := $(word 2, $(VERSION_PARTS))
ifneq ($(COMMIT),$(TAG_COMMIT))
	VERSION := $(VERSION)-next-$(COMMIT)-$(DATE)
endif
ifeq ($(VERSION),"")
	VERSION := $(COMMIT)-$(DATE)
endif
ifneq ($(shell git status --porcelain -uno),)
	VERSION := $(VERSION)-dirty
endif
CPPFLAGS    += -DGIT_VERSION_STR="\"$(VERSION)\""
CPPFLAGS    += -DGIT_VERSION_MAJOR=$(VERSION_MAJOR)
CPPFLAGS    += -DGIT_VERSION_MINOR=$(VERSION_MINOR)
CPPFLAGS    += -DUSB_PID=$(USB_PID)
CPPFLAGS    += -DUSB_VID=$(USB_VID)
CPPFLAGS    += -DCODE_OFFSET=$(CODE_OFFSET)


all: info $(IMG_BIN)

clean:
	rm -f $(GENERATED)

info: $(IMG_MEM)
	cat $(IMG_MEM)

download_all: $(IMG_DFU) info
	sh dfu_all.sh dfu-util -D $< -R

download: $(IMG_DFU) info
	dfu-util -D $< -R

%.dfu: %.bin
	cp $< $@.tmp
	dfu-suffix -v $(USB_VID) -p $(USB_PID) --add $@.tmp
	mv $@.tmp $@

upload:
	rm -f $(UPLOADED_BIN)
	dfu-util -U $(UPLOADED_BIN) -R

flash: $(IMG_BIN)
	# My own hacked-together ESP32 based flasher
	ccflash write --erase --reset --verify $<

%.bin: %.ihx
	objcopy --input-target=ihex --output-target=binary $< $@

$(IMG_IHX) $(IMG_MEM) $(RST_FILES): $(REL_FILES) $(LST_FILES)
	$(CC) -V $(CFLAGS) -o $(IMG_IHX) $(REL_FILES)

%.i: %.c
	$(CC) -E $(CPPFLAGS) $(CFLAGS) $< > $@

%.asm: %.S
	$(CPP) -P $(CPPFLAGS) $< -o $@

%.asm: %.c
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $< -o $@

%.rel: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

%.rel: %.asm %.lst
	$(AS) $(ASFLAGS) -o $@ $<

%.lst: %.asm
	$(AS) $(ASFLAGS) -l $@ $<

%.sym: %.asm
	$(AS) $(ASFLAGS) -s $@ $<

%.d: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM $< >$@

.PHONY: all clean info download_all download upload

-include $(DEP_FILES)
