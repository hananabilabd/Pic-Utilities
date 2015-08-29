#include "p33FJ128GP802.h"
#include "dsp.h"
#include "adcdacDrv.h"

//Macros for Configuration Fuse Registers:
//Invoke macros to set up  device configuration fuse registers.
//The fuses will select the oscillator source, power-up timers, watch-dog
//timers etc. The macros are defined within the device
//header files. The configuration fuse registers reside in Flash memory.



// Internal FRC Oscillator
_FOSCSEL(FNOSC_FRC); // FRC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
// OSC2 Pin Function: OSC2 is Clock Output
// Primary Oscillator Mode: Disabled

_FWDT(FWDTEN_OFF); // Watchdog Timer Enabled/disabled by user software
// (LPRC can be disabled by clearing SWDTEN bit in RCON register

int main(void)
{
    // Configure Oscillator to operate the device at 40MIPS
    // Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
    // Fosc= 7.37M*43/(2*2)=79.22Mhz for ~40MIPS input clock
    PLLFBD = 41; // M=43
    CLKDIVbits.PLLPOST = 0; // N1=2
    CLKDIVbits.PLLPRE = 0; // N2=2
    OSCTUN = 0; // Tune FRC oscillator, if FRC is used

    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;

    // Clock switch to incorporate PLL
    __builtin_write_OSCCONH(0x01); // Initiate Clock Switch to
    // FRC with PLL (NOSC=0b001)
    __builtin_write_OSCCONL(0x01); // Start clock switching
    while (OSCCONbits.COSC != 0b001); // Wait for Clock switch to occur

    // Wait for PLL to lock
    while (OSCCONbits.LOCK != 1) {
    };

    initAdc(); // Initialize the A/D converter to convert Channel 4
    initDac(); // Initialize the D/A converter
    initDma0(); // Initialize the DMA controller to buffer ADC data in conversion order
    initTmr3(); // Initialize the Timer to generate sampling event for ADC

    TRISBbits.TRISB3 = 0; // Debug pin

    extern fractional buffer[2][NUMSAMP]; // Ping pong buffers
    extern int flag; // When a buffer has been filled...
    extern int dmabuffer; // Buffer toggler
    int i;

    while (1)
    {
        while (!flag);
        for (i = 0; i < NUMSAMP; i++) {
            while (DAC1STATbits.REMPTY != 1); // Wait for D/A conversion
            /* DAC1RDAT = buffer[dmabuffer][i]; // Load the DAC buffer with data */
            DAC1RDAT = buffer[0][i]; // Load the DAC buffer with data
        }
        flag = 0;
    }

    return 0;

}

