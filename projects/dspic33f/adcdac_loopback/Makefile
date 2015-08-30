# PROJECT SETUP ################################################################

# Project name
# Example: serial
PROJECT = adcdac_loopback

# Microcontroller name (for xc16)
# Example: 33FJ128GP802
MCU = 33FJ128GP802

# Architecture (for xc16)
# Example: dsPIC33F
ARCH = dsPIC33F

# Microcontroller name (for mdb.sh)
# Example: dsPIC33FJ128GP802 or dsPIC$(MCU)
PROGRAMMCU = dsPIC$(MCU)

# Debugging tool (for mdb.sh)
# Example: pickit3 -p
HWTOOL = pickit3 -p

# Source files
# Example: serial.c
SOURCES = main.c adcdac.c serial.c

# Tool paths
# TODO: DEBUGGER is not used!
INSTALLPATH  = /opt/microchip/xc16/v1.24
CC           = $(INSTALLPATH)/bin/xc16-gcc
LINKERSCRIPT = $(INSTALLPATH)/support/$(ARCH)/gld/p$(MCU).gld
BIN2HEX      = $(INSTALLPATH)/bin/xc16-bin2hex
DEBUGGEER    = /opt/microchip/mplabx/v3.00/mplab_ide/bin/mdb.sh

# PROJECT INDEPENDENT SETTINGS #################################################

# Compiler and linker settings:w
CFLAGS = -mcpu=$(MCU) -Wl,--script=$(LINKERSCRIPT)
LDFLAGS =

# Executables
ELFFILE = $(PROJECT).elf
HEXFILE = $(PROJECT).hex

# Programming file -- mdb commands will be stored here
PROGRAMMER = ./prog.txt

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Files to clean when running "make clean"
CLEANFILES = *.o *.elf *.hex $(PROGRAMMER)

# MAKE ROUTINES ################################################################
.SUFFIXES: .c .o
.PHONY: clean

all: $(PROGRAMMER) $(HEXFILE)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROGRAMMER): 
	echo "Device $(PROGRAMMCU)"                  > $(PROGRAMMER)
	echo "set system.disableerrormsg true"      >> $(PROGRAMMER)
	echo "set poweroptions.powerenable true"    >> $(PROGRAMMER)
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

