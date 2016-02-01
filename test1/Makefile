ROM_NAME=test1
ROM_HEADERNAME="libnus test"

AS = $(N64_INST)/bin/mips64-elf-as
AR = $(N64_INST)/bin/mips64-elf-ar
CC = $(N64_INST)/bin/mips64-elf-gcc
OBJCOPY = $(N64_INST)/bin/mips64-elf-objcopy
N64TOOL = $(N64_INST)/bin/n64tool
CHKSUM64 = $(N64_INST)/bin/chksum64

CFLAGS = -Wall -std=c99 -I$(N64_INST)/include -I..

OPTFLAGS = -Os -march=vr4300 \
		   -mno-gpopt -G8 -mno-extern-sdata \
		   -flto -ffat-lto-objects \

ASMFILES =
CFILES = test1.c

OBJFILES = \
		   $(ASMFILES:.S=.o) \
		   $(CFILES:.c=.o)

DEPFILES = $(OBJFILES:.o=.d)

all: $(ROM_NAME).z64


$(ROM_NAME).z64: $(ROM_NAME).elf
	$(OBJCOPY) $(ROM_NAME).elf $(ROM_NAME).bin -O binary
	$(N64TOOL) -l 2M -t $(ROM_HEADERNAME) -h ../header.bin -o $(ROM_NAME).z64 $(ROM_NAME).bin
	$(CHKSUM64) $(ROM_NAME).z64

$(ROM_NAME).elf: $(OBJFILES)
	$(CC) $(CFLAGS) $(OPTFLAGS) -T../rom.ld -o $@ $(OBJFILES) -Wl,-Map=$(ROM_NAME).map,-L${N64_INST}/lib,-L..,-lnus,-lfb,-lc


%.o: %.S
	$(CC) $(CFLAGS) $(OPTFLAGS) -MMD -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -MMD -c $< -o $@

#
# Clean project target.
#
.PHONY: clean
clean:
	$(RM) $(ROM_NAME).map $(ROM_NAME).elf $(ROM_NAME).z64 \
		$(DEPFILES) $(OBJFILES) $(ROM_NAME).bin

#
# Use computed dependencies.
#
-include $(DEPFILES)

