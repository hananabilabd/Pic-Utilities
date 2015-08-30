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
    //                         Fin = 7.37 MHz
    CLKDIVbits.PLLPRE = 0;  // (1) Divide by (PLLPRE+2)
    PLLFBDbits.PLLDIV = 38; // (2) Multiply by (PLLDIV+2)
    CLKDIVbits.PLLPOST = 0; // (3) Divide by 2*(PLLPOST+1)
    
    // 40 x: 73.7 MHz

    
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
    extern int samplemax;

    while (1) { 
        // Interrupt driven execution.
        // Debug printing done here.
        sprintf(msg, "Sample Max: %d", samplemax);
        serial_writeln(msg);
        __delay_ms(1000);
    }

    return 0;

}

