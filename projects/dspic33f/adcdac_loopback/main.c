/////////////////////////////////////////////////////////////////////////////////////////////////
// © 2012 Microchip Technology Inc.
//
// MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:  You may use this software, and any
// derivatives created by any person or entity by or on your behalf, exclusively with
// Microchip?s products.  Microchip and its licensors retain all ownership and intellectual
// property rights in the accompanying software and in all derivatives here to.
//
// This software and any accompanying information is for suggestion only.  It does not
// modify Microchip?s standard warranty for its products.  You agree that you are solely
// responsible for testing the software and determining its suitability.  Microchip has
// no obligation to modify, test, certify, or support the software.
//
// THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION
// WITH MICROCHIP?S PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
//
// IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
// (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY, INDEMNITY,
// CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE, EXEMPLARY, INCIDENTAL
// OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
// SOFTWARE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR
// THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL
// LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES,
// IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
//
// MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.
//
// ****************************************************************************
// ****************************************************************************/
//
// ADDITIONAL NOTES:
// Code Tested on:
// 16-Bit 28-Pin Starter board with dsPIC33FJ128GP802 device
// **********************************************************************/

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

    extern fractional BufferA[NUMSAMP]; // Ping pong buffer A
    extern fractional BufferB[NUMSAMP]; // Ping pong buffer B
    extern unsigned int DmaBuffer; // DMA flag
    extern int flag; // Flag
    int i;

    while (1) // Loop Endlessly - Execution is interrupt driven
    {
        if (flag) {
            for (i = 0; i < NUMSAMP; i++) {
                while (DAC1STATbits.REMPTY != 1); // Wait for D/A conversion
                if (DmaBuffer == 0)
                    DAC1RDAT = BufferA[i]; // Load the DAC buffer with data
                else
                    DAC1RDAT = BufferB[i]; // Load the DAC buffer with data
            }
            flag = 0;
        }
    }

    return 0;

}

