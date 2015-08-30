# PIC Utilities and Projects

This repository contains various utilities for PIC and dsPIC microcontrollers along with different projects.

## Makefiles
The [makefiles](./makefiles) folder contains Makefiles for compiling and flashing code with the PICkit3 debugger.
- For PIC16, the `xc8` compiler is used.
- For dsPIC33, the `xc16` compiler is used.

The code is flashed using the `mdb.sh` utility that comes with MPLAB X IDE.

The Makefiles should be modified to fit 
- The specific processor used 
- The source files used in the project (`.c` files only, atm - I haven't had use for Assembly yet but it should be added easily)
- The paths to the compiler and debugger installation.

Examples of the Makefiles in use can be found in the [projects](projects) folder.

## Projects
The [projects](projects) folder contains various PIC16 and dsPIC33 projects that might be of interrest for someone to get started using these microcontrollers.


