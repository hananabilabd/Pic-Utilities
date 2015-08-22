# PROJECT SETUP ################################################################

# Source files
# Example: serial.c
SOURCES = main.c eusart.c

# Hex file name (same basename as the first file in SOURCES!)
# Example serial.hex
HEXFILE = main.hex

# Microcontroller name (for xc8)
# Example: 33FJ128GP802
MCU = 16F1704

# Microcontroller name (for mdb.sh)
# Example: dsPIC33FJ128GP802 or dsPIC$(MCU)
PROGRAMMCU = PIC$(MCU)

# Debugging tool (for mdb.sh)
# Example: pickit3 -p
HWTOOL = pickit3 -p

# Voltage for target
# Example: 3.3
VOLTAGE = 5.0

# Tool paths
# TODO: DEBUGGER is not used!
INSTALLPATH  = /opt/microchip/xc8/v1.34
CC           = $(INSTALLPATH)/bin/xc8
DEBUGGEER    = /opt/microchip/mplabx/v3.00/mplab_ide/bin/mdb.sh

# PROJECT INDEPENDENT SETTINGS #################################################

# Compiler and linker settings:w
CFLAGS = --chip=$(MCU)
LDFLAGS =

# Programming file -- mdb commands will be stored here
PROGRAMMER = ./prog.txt

# Files to clean when running "make clean"
CLEANFILES = *.o *.elf *.obj *.rlf *.as *.lst *.obj *.d *.cmf\
	    *.cof *hxl *.p1 *.pre *.sdb *.sym $(PROGRAMMER)

# MAKE ROUTINES ################################################################
.SUFFIXES: .c .o
.PHONY: clean

all: $(HEXFILE)

$(PROGRAMMER): 
	echo "Device $(PROGRAMMCU)"                  > $(PROGRAMMER)
	echo "set poweroptions.powerenable true"    >> $(PROGRAMMER)
	echo "set voltagevalue $(VOLTAGE)"          >> $(PROGRAMMER)
	echo "Hwtool $(HWTOOL)"                     >> $(PROGRAMMER)
	echo "Program $(HEXFILE)"                   >> $(PROGRAMMER)
	echo "Reset MCLR"                           >> $(PROGRAMMER)
	echo "Quit"                                 >> $(PROGRAMMER)

$(HEXFILE): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES)

# TODO: Make this independent of "mdb.sh" path.
flash: $(PROGRAMMER) $(HEXFILE)
	#$(DEBUGGER) $(PROGRAMMER)
	mdb.sh $(PROGRAMMER)
	rm -rf MPLABXLog.xml*

clean:
	rm -rf $(CLEANFILES)

cleanall:
	rm -rf $(CLEANFILES) $(HEXFILE)
    
# REFERENCES ###################################################################
# [1] http://www.electropepper.org/blog/item/linux-terminal-only-pic-programming
# [2] https://batchloaf.wordpress.com/2014/03/20/installing-microchip-xc16-in-crunchbang-linux/
# [3] http://ww1.microchip.com/downloads/en/DeviceDoc/50002071E.pdf

