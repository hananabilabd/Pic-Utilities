# References:
# [1] http://www.electropepper.org/blog/item/linux-terminal-only-pic-programming
# [2] https://batchloaf.wordpress.com/2014/03/20/installing-microchip-xc16-in-crunchbang-linux/
# [3] http://ww1.microchip.com/downloads/en/DeviceDoc/50002071E.pdf

# CHANGE HERE FOR YOUR PROJECT
PROJECT = serial
MCU = 33FJ128GP802
ARCH = dsPIC33F
PROGRAMMCU = dsPIC$(MCU)
SOURCES = serial.c

# Declare tools.
SHELL = /bin/sh
INSTALLPATH = /opt/microchip/xc16/v1.24
CC = $(INSTALLPATH)/bin/xc16-gcc
LINKERSCRIPT = $(INSTALLPATH)/support/$(ARCH)/gld/p$(MCU).gld
BIN2HEX = $(INSTALLPATH)/bin/xc16-bin2hex
DEBUGGEER = /opt/microchip/mplabx/v3.00/mplab_ide/bin/mdb.sh

CFLAGS = -mcpu=$(MCU) -Wl,--script=$(LINKERSCRIPT)
LDFLAGS =

ELFFILE = $(PROJECT).elf
HEXFILE = $(PROJECT).hex
PROGRAMMER = ./prog.txt # This is the file with the commands for the PICKIT3

OBJECTS = $(SOURCES:.c=.o)
CLEANFILES = *.o *.elf *.hex $(PROGRAMMER)

.SUFFIXES: .c .o
.PHONY: clean

# Compile
all: $(PROGRAMMER) $(HEXFILE)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROGRAMMER): 
	echo "Device $(PROGRAMMCU)" > $(PROGRAMMER)
	echo "set system.disableerrormsg true" >> $(PROGRAMMER)
	echo "Hwtool pickit3 -p" >> $(PROGRAMMER)
	echo "Program \"$(HEXFILE)\"" >> $(PROGRAMMER)
	echo "Reset MCLR" >> $(PROGRAMMER)
	echo "Quit" >> $(PROGRAMMER)

$(ELFFILE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(ELFFILE) $(OBJECTS)

$(HEXFILE): $(ELFFILE)
	$(BIN2HEX) $(ELFFILE)

flash: $(HEXFILE)
	# $(DEBUGGER) $(PROGRAMMER)
	mdb.sh $(PROGRAMMER)
	rm -rf MPLABXLog.xml*

clean:
	rm -rf $(CLEANFILES)
    
