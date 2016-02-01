AS = $(N64_INST)/bin/mips64-elf-as
AR = $(N64_INST)/bin/mips64-elf-ar
CC = $(N64_INST)/bin/mips64-elf-gcc
OBJCOPY = $(N64_INST)/bin/mips64-elf-objcopy
N64TOOL = $(N64_INST)/bin/n64tool
CHKSUM64 = $(N64_INST)/bin/chksum64

CFLAGS = -Wall -std=c99 -I$(N64_INST)/include -I.

OPTFLAGS = -Os -march=vr4300 \
		   -mno-gpopt -G8 -mno-extern-sdata \
		   -flto -ffat-lto-objects \

ASMFILES_NUS = crt0.S context_switch.S interrupt_handler.S

CFILES_NUS = libnus.c
CFILES_FB = libfb.c

OBJFILES_NUS = \
			   $(ASMFILES_NUS:.S=.o) \
			   $(CFILES_NUS:.c=.o)

OBJFILES_FB = $(CFILES_FB:.c=.o)

DEPFILES_NUS = $(OBJFILES_NUS:.o=.d)
DEPFILES_FB = $(OBJFILES_FB:.o=.d)

all: libnus.a libfb.a

libnus.a: $(OBJFILES_NUS)
	$(AR) -rcs -o libnus.a $(OBJFILES_NUS)

libfb.a: $(OBJFILES_FB)
	$(AR) -rcs -o libfb.a $(OBJFILES_FB)

%.o: %.S
	$(CC) $(CFLAGS) $(OPTFLAGS) -MMD -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(OPTFLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	rm -f libnus.a libfb.a $(OBJFILES_NUS) $(OBJFILES_FB) $(DEPFILES_NUS) $(DEPFILES_FB)

-include $(DEPFILES)

