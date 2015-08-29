#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "dsp.h"
#include "adcdacDrv.h"
#include "serial.h"

#include <libpic30.h>
_FOSCSEL(FNOSC_FRCPLL); // Use PLL with oscillator

/**
 * Set up PLL for 80 MHz operation.
 */
void initPll()
{
    CLKDIVbits.PLLPRE = 0;
    PLLFBDbits.PLLDIV = 42 - 2; // (divisor is 2 more than the value)
    CLKDIVbits.PLLPOST = 0;
    
    // 40 x: 73.700000 MHz = 36.850000 MIPS
    // 42 x: 77.385 MHz = 38.6925 MIPS
    // 43 x: 79.2275 MHz = 39.61375 MIPS
    
    while (OSCCONbits.LOCK != 1); // Wait for PLL to lock
    RCONbits.SWDTEN = 0; // Disable Watch Dog Timer
}

int main(void)
{
    initPll();
    initAdc();
    initDac();
    initDma0();
    initTmr3();

    TRISBbits.TRISB7 = 0; // TX = RP7
    RPOR3bits.RP7R = 0b00011;
    TRISBbits.TRISB6 = 1; // RX = RP6
    RPINR18bits.U1RXR = 6;
    serial_init();

    TRISBbits.TRISB3 = 0; // Debug pin

    serial_writeln("Starting...");

    char msg[81];

    while (1) { 
        // Interrupt driven execution.
        // Debug printing done here.
    }

    return 0;

}

