#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "dsp.h"
#include "adcdac.h"
#include "serial.h"

#include <libpic30.h>
_FOSCSEL(FNOSC_FRCPLL); // Use PLL with oscillator

/**
 * Set up PLL for 80 MHz operation.
 */
void pll_init()
{
    //                         Fin = 7.37 MHz
    CLKDIVbits.PLLPRE = 0;  // (1) Divide by (PLLPRE+2)
    PLLFBDbits.PLLDIV = 38; // (2) Multiply by (PLLDIV+2)
    CLKDIVbits.PLLPOST = 0; // (3) Divide by 2*(PLLPOST+1)
    
    // 7.37e6 * 40/(2*2): 73.7 MHz

    
    while (OSCCONbits.LOCK != 1); // Wait for PLL to lock
    RCONbits.SWDTEN = 0; // Disable Watch Dog Timer
}

int main(void)
{
    pll_init();
    adc_init();
    dac_init();
    dma0_init();
    timer3_init();
    serial_init();

    serial_writeln("Starting...");

    char msg[81];
    extern int samplemax;

    while (1) { 
        // Interrupt driven execution.
        // Debug printing done here.
        /* sprintf(msg, "Sample Max: %d", samplemax); */
        /* serial_writeln(msg); */
        /* __delay_ms(1000); */
    }

    return 0;

}

