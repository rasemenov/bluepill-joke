######################################################################
#  Rules etc. adapted from libopencm3
######################################################################

PREFIX		?= arm-none-eabi
BINARY      := main

TOP_DIR 	:= $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CONTRIB_DIR := $(TOP_DIR)/contrib
OPENCM3_DIR := $(CONTRIB_DIR)/libopencm3
SRC_DIR     := ./src
BUILD_DIR   := ./build
INC_DIRS    := $(shell find $(SRC_DIR) -maxdepth 1 -type d)
INC_DIRS    += $(shell find $(CONTRIB_DIR) -maxdepth 1 -type d)
CONTRIB     := printf

DEFS		+= -DSTM32F1

FP_FLAGS	?= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd
ASFLAGS		= -mthumb -mcpu=cortex-m3

CC		:= $(PREFIX)-gcc
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
SIZE		:= $(PREFIX)-size
OBJDUMP		:= $(PREFIX)-objdump
GDB		:= $(PREFIX)-gdb
STFLASH		= $(shell which st-flash)
OPT		:= -Os -g
CSTD		?= -std=c99

SRCFILES    = $(shell find $(SRC_DIR) -name \*.c)
SRCFILES   += $(foreach dep,$(CONTRIB),$(shell find $(CONTRIB_DIR)/$(dep) -name \*.c))
OBJS       := $(SRCFILES:%.c=$(BUILD_DIR)/%.o)
INCLUDES   := $(addprefix -I,$(INC_DIRS))

LDSCRIPT	?= $(SRC_DIR)/stm32f103c8t6.ld

TGT_CFLAGS	+= $(OPT) $(CSTD)
TGT_CFLAGS	+= $(ARCH_FLAGS)
TGT_CFLAGS	+= -Wextra -Wshadow -Wimplicit-function-declaration
TGT_CFLAGS	+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS	+= -fno-common -ffunction-sections -fdata-sections

TGT_CXXFLAGS	+= $(OPT)
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
TGT_LDFLAGS	+= -Wl,-Map=$(BUILD_DIR)/$(BINARY).map
TGT_LDFLAGS	+= -Wl,--gc-sections

LDLIBS		+= -specs=nosys.specs
LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LDLIBS		+= -L$(OPENCM3_DIR)/lib -lopencm3_stm32f1

.SUFFIXES:	.elf .bin .hex .srec .list .map .images

$(BUILD_DIR)/$(BINARY).bin: $(BUILD_DIR)/$(BINARY)
	@#printf "  OBJCOPY $(*).bin\n"
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

$(BUILD_DIR)/$(BINARY): lib contrib $(OBJS)
	@#printf "  $(OPENCM3_DIR)\n"
	@echo building
	$(LD) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@.elf
	$(SIZE) $(BUILD_DIR)/$(BINARY).elf

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	@echo $(INCLUDES)
	$(CC) $(TGT_CFLAGS) $(CFLAGS) $(INCLUDES) $(TGT_CPPFLAGS) $(CPPFLAGS) -c $< -o $@

lib:
	$(MAKE) -C $(OPENCM3_DIR) TARGETS=stm32/f1 all

contrib:
	for dep in $(CONTRIB) ; do \
		$(MAKE) -C $(CONTRIB_DIR)/$$(dep) ; \
	done

clean:
	@#printf "  CLEAN\n"
	rm -rf $(BUILD_DIR)

clobber: clean
	$(MAKE) -C $(OPENCM3_DIR) clean
	$(foreach dep,$(CONTRIB),$(MAKE) -C $(CONTRIB_DIR)/$(dep) clean)

# Flash 64k Device
flash:	$(BUILD_DIR)/$(BINARY).bin
	$(STFLASH) $(FLASHSIZE) write $(BUILD_DIR)/$(BINARY).bin 0x8000000

# Flash 128k Device
bigflash: $(BINARY).bin
	$(STFLASH) --flash=128k write $(BINARY).bin 0x8000000
