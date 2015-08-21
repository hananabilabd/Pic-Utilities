# PROJECT SETUP ################################################################

# Project name
# Example: serial
PROJECT = midi_synth_driver

# Microcontroller name (for xc16)
# Example: 33FJ128GP802
MCU = 16F1704

# Architecture (for xc16)
# Example: dsPIC33F
ARCH = PIC16F

# Microcontroller name (for mdb.sh)
# Example: dsPIC33FJ128GP802 or dsPIC$(MCU)
PROGRAMMCU = PIC$(MCU)

# Source files
# Example: serial.c
SOURCES = serial.c

# Debugging tool (for mdb.sh)
# Example: pickit3 -p
HWTOOL = pickit3 -p

# Tool paths
# TODO: DEBUGGER is not used!
INSTALLPATH  = /opt/microchip/xc8/v1.34
CC           = $(INSTALLPATH)/bin/xc8-gcc
LINKERSCRIPT = $(INSTALLPATH)/support/$(ARCH)/gld/p$(MCU).gld
BIN2HEX      = $(INSTALLPATH)/bin/xc8-bin2hex
DEBUGGEER    = /opt/microchip/mplabx/v3.00/mplab_ide/bin/mdb.sh

# PROJECT INDEPENDENT SETTINGS #################################################

# Compiler and linker settings:w
CFLAGS = --chip=$(MCU) -Wl,--script=$(LINKERSCRIPT)
LDFLAGS =

# Executables
ELFFILE = $(PROJECT).elf
HEXFILE = $(PROJECT).hex

# Programming file -- mdb commands will be stored here
PROGRAMMER = ./prog.txt

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Files to clean when running "make clean"
CLEANFILES = *.o *.elf *.hex *.obj *.rlf *.as *.lst *.obj *.d *.cmf\
	    *.cof *hxl *.p1 *.pre *.sdb *.sym $(PROGRAMMER)

# MAKE ROUTINES ################################################################
.SUFFIXES: .c .o
.PHONY: clean

all: $(PROGRAMMER) $(HEXFILE)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROGRAMMER): 
	echo "Device $(PROGRAMMCU)"                  > $(PROGRAMMER)
	echo "set system.disableerrormsg true"      >> $(PROGRAMMER)
	echo "Hwtool $(HWTOOL)"                     >> $(PROGRAMMER)
	echo "Program \"$(HEXFILE)\""               >> $(PROGRAMMER)
	echo "Reset MCLR"                           >> $(PROGRAMMER)
	echo "Quit"                                 >> $(PROGRAMMER)

$(ELFFILE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(ELFFILE) $(OBJECTS)

$(HEXFILE): $(ELFFILE)
	$(BIN2HEX) $(ELFFILE)

# TODO: Make this independent of "mdb.sh" path.
flash: $(PROGRAMMER) $(HEXFILE)
	#$(DEBUGGER) $(PROGRAMMER)
	mdb.sh $(PROGRAMMER)
	rm -rf MPLABXLog.xml*

clean:
	rm -rf $(CLEANFILES)
    
# REFERENCES ###################################################################
# [1] http://www.electropepper.org/blog/item/linux-terminal-only-pic-programming
# [2] https://batchloaf.wordpress.com/2014/03/20/installing-microchip-xc16-in-crunchbang-linux/
# [3] http://ww1.microchip.com/downloads/en/DeviceDoc/50002071E.pdf

