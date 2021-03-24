######################################################################
#  Project Makefile
######################################################################

BINARY		= main
SRCFILES	= main.c sys/serial.c sys/cmds.c sys/timer.c
SRCFILES   += contrib/printf/printf.c
LDSCRIPT	= stm32f103c8t6.ld
TGT_CFLAGS += -I./sys -I.
TGT_CXXFLAGS += -I./sys -I.

# start: elf bin

include ../Makefile.incl

######################################################################
#  NOTES:
#
#	1. remove any modules you don't need from SRCFILES
#
#	2. "make clean" will remove *.o etc., but leaves *.elf, *.bin
#
#	3. "make clobber" will "clean" and remove *.elf, *.bin etc.
#
#	4. "make flash" will perform:
#	
#	   st-flash write main.bin 0x8000000
#
######################################################################
