# Inspired by 
# http://www.electropepper.org/blog/item/linux-terminal-only-pic-programming

# Declare tools.
SHELL = /bin/sh
CC = xc16-gcc

# CHANGE HERE FOR YOUR PROJECT
MCU = 33fj128gp802
ARCH = 
SOURCES = test1.c
PROGRAMMER = ./prog.txt # This is the file with the commands for the PICKIT3


CFLAGS  = -m$(ARCH) -p$(MCU) --use-non-free -L /usr/local/share/sdcc/lib/src/pic14
LDFLAGS =

EXECUTABLE = t1

OBJECTS = $(SOURCES:.c=.o)
CLEANFILES = test1.o test1.asm test1.map test1.lst test1.cod



.SUFFIXES: .c .o
.PHONY: clean

# Compile
all: clean $(EXECUTABLE)

.c.o:
    $(CC) $(CFLAGS) $<

$(EXECUTABLE): $(OBJECTS)

clean:
    rm -rf $(CLEANFILES)
    
flash:
    /opt/microchip/mplabx/mplab_ide/bin/mdb.sh $(PROGRAMMER)
    rm MPLABXLog*
