######################################################################
#  Rules etc. adapted from libopencm3
######################################################################

PREFIX		?= arm-none-eabi

TOP_DIR 	:= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
OPENCM3_DIR := $(abspath $(TOP_DIR)/contrib/libopencm3)
SRC_DIR     := $(abspath $(TOP_DIR)/src)

LIBNAME		= opencm3_stm32f1
DEFS		+= -DSTM32F1

FP_FLAGS	?= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd
ASFLAGS		= -mthumb -mcpu=cortex-m3

CC		:= $(PREFIX)-gcc
CXX		:= $(PREFIX)-g++
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
SIZE		:= $(PREFIX)-size
OBJDUMP		:= $(PREFIX)-objdump
GDB		:= $(PREFIX)-gdb
STFLASH		= $(shell which st-flash)
STYLECHECK	:= /checkpatch.pl
STYLECHECKFLAGS	:= --no-tree -f --terse --mailback
STYLECHECKFILES	:= $(shell find . -name '*.[ch]')
OPT		:= -Os -g
CSTD		?= -std=c99

TEMP1 		= $(patsubst %.c,%.o,$(SRCFILES))
TEMP2		= $(patsubst %.asm,%.o,$(TEMP1))
OBJS 		= $(patsubst %.cpp,%.o,$(TEMP2))

LDSCRIPT	?= $(TOP_DIR)/stm32f103c8t6.ld

TGT_CFLAGS	+= $(OPT) $(CSTD)
TGT_CFLAGS	+= $(ARCH_FLAGS)
TGT_CFLAGS	+= -Wextra -Wshadow -Wimplicit-function-declaration
TGT_CFLAGS	+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS	+= -fno-common -ffunction-sections -fdata-sections
TGT_CFLAGS	+= -I$(OPENCM3_DIR)/include

TGT_CXXFLAGS	+= $(OPT) $(CXXSTD)
TGT_CXXFLAGS	+= $(ARCH_FLAGS)
TGT_CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls  -Weffc++
TGT_CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

TGT_CPPFLAGS	+= -MD
TGT_CPPFLAGS	+= -Wall -Wundef
TGT_CPPFLAGS	+= $(DEFS)
TGT_CPPFLAGS	+= -I$(OPENCM3_DIR)/include

TGT_LDFLAGS	+= --static -nostartfiles
TGT_LDFLAGS	+= -T$(LDSCRIPT)
TGT_LDFLAGS	+= $(ARCH_FLAGS)
TGT_LDFLAGS	+= -Wl,-Map=$(*).map
TGT_LDFLAGS	+= -Wl,--gc-sections

LDLIBS		+= -specs=nosys.specs
LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LDLIBS		+= -L$(OPENCM3_DIR)/lib -lopencm3_stm32f1

.SUFFIXES:	.elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

elf:	$(DEPS) $(BINARY).elf
bin:	$(DEPS) $(BINARY).bin
hex:	$(DEPS) $(BINARY).hex
srec:	$(DEPS) $(BINARY).srec
list:	$(DEPS) $(BINARY).list

elf: build lib

lib:
	$(MAKE) -C $(OPENCM3_DIR)

build: lib
	@#printf "  $(OPENCM3_DIR)\n"
	$(MAKE) --directory=$(SRC_DIR) OPENCM3_DIR=$(OPENCM3_DIR)

clean:
	@#printf "  CLEAN\n"
	$(RM) *.o *.d generated.* $(OBJS) $(patsubst %.o,%.d,$(OBJS))

clobber: clean
	rm -f *.elf *.bin *.hex *.srec *.list *.map $(CLOBBER)

# Flash 64k Device
flash:	$(BINARY).bin
	$(STFLASH) $(FLASHSIZE) write $(BINARY).bin 0x8000000

# Flash 128k Device
bigflash: $(BINARY).bin
	$(STFLASH) --flash=128k write $(BINARY).bin 0x8000000
