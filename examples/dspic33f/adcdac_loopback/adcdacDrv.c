
/////////////////////////////////////////////////////////////////////////////////////////////////
// � 2012 Microchip Technology Inc.
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

#include "p33FJ128GP802.h"
#include "dsp.h"
#include "adcdacDrv.h"

fractional BufferA[NUMSAMP] __attribute__((space(dma))); // Ping-pong buffer A
fractional BufferB[NUMSAMP] __attribute__((space(dma))); // Ping-pong buffer B

/*=============================================================================
initAdc() is used to configure A/D to convert channel 4 on Timer event. 
It generates event to DMA on every sample/convert sequence.  
=============================================================================*/
void initAdc(void)
{
    AD1CON1bits.FORM = 3; // Data Output Format: Signed Fraction (Q15 format)
    AD1CON1bits.SSRC = 2; // Sample Clock Source: GP Timer starts conversion
    AD1CON1bits.ASAM = 1; // ADC Sample Control: Sampling begins immediately after conversion
    AD1CON1bits.AD12B = 1; // 12-bit ADC operation

    AD1CON2bits.CHPS = 0; // Converts CH0

    AD1CON3bits.ADRC = 0; // ADC Clock is derived from Systems Clock
    AD1CON3bits.ADCS = 3; // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*4 = 100ns
    // ADC Conversion Time for 12-bit Tc=14*Tad = 1.4us

    AD1CON1bits.ADDMABM = 1; // DMA buffers are built in conversion order mode
    AD1CON2bits.SMPI = 0; // SMPI must be 0


    //AD1CHS0: A/D Input Select Register
    AD1CHS0bits.CH0SA = 0; // MUXA +ve input selection (AN0) for CH0
    AD1CHS0bits.CH0NA = 0; // MUXA -ve input selection (Vref-) for CH0

    //AD1PCFGH/AD1PCFGL: Port Configuration Register
    AD1PCFGL = 0xFFFF;
    AD1PCFGLbits.PCFG0 = 0; // AN0 as Analog Input


    IFS0bits.AD1IF = 0; // Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 0; // Do Not Enable A/D interrupt
    AD1CON1bits.ADON = 1; // Turn on the A/D converter	
}

/*=============================================================================
initDac() is used to configure D/A. 
=============================================================================*/
void initDac(void)
{
    /* Initiate DAC Clock */
    ACLKCONbits.SELACLK = 0; // FRC w/ Pll as Clock Source
    ACLKCONbits.AOSCMD = 0; // Auxiliary Oscillator Disabled
    ACLKCONbits.ASRCSEL = 0; // Auxiliary Oscillator is the Clock Source
    ACLKCONbits.APSTSCLR = 7; // Fvco/1 = 158.2 MHz/1 = 158.2 MHz

    DAC1STATbits.ROEN = 1; // Right Channel DAC Output Enabled
    DAC1DFLT = 0x8000; // DAC Default value is the midpoint

    // Sampling Rate Fs = DACCLK/256 = 103 kHz
    DAC1CONbits.DACFDIV = 7; // DACCLK = ACLK/(DACFDIV - 1): 158.2 MHz/6 = 26.4 MHz

    DAC1CONbits.FORM = 1; // Data Format is signed integer
    DAC1CONbits.AMPON = 0; // Analog Output Amplifier is enabled during Sleep Mode/Stop-in Idle mode

    DAC1CONbits.DACEN = 1; // DAC1 Module Enabled
}

/*=======================================================================================  
Timer 3 is setup to time-out every Ts secs. As a result, the module 
will stop sampling and trigger a conversion on every Timer3 time-out Ts. 
At that time, the conversion process starts and completes Tc=12*Tad periods later.
When the conversion completes, the module starts sampling again. However, since Timer3 
is already on and counting, about (Ts-Tc)us later, Timer3 will expire again and trigger 
next conversion. 
=======================================================================================*/
void initTmr3()
{
    TMR3 = 0x0000; // Clear TMR3
    PR3 = SAMPPRD; // Load period value in PR3
    IFS0bits.T3IF = 0; // Clear Timer 3 Interrupt Flag
    IEC0bits.T3IE = 0; // Clear Timer 3 interrupt enable bit

    T3CONbits.TON = 1; // Enable Timer 3
}

/*=============================================================================  
DMA0 configuration
 Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM 
 AMODE: Register indirect with post increment
 MODE: Continuous, Ping-Pong Mode
 IRQ: ADC Interrupt
 ADC stores results stored alternatively between BufferA[] and BufferB[]
=============================================================================*/
void initDma0(void)
{
    DMA0CONbits.AMODE = 0; // Configure DMA for Register indirect with post increment
    DMA0CONbits.MODE = 2; // Configure DMA for Continuous Ping-Pong mode

    DMA0PAD = (int) &ADC1BUF0; // Peripheral Address Register: ADC buffer
    DMA0CNT = (NUMSAMP - 1); // DMA Transfer Count is (NUMSAMP-1)

    DMA0REQ = 13; // ADC interrupt selected for DMA channel IRQ

    DMA0STA = __builtin_dmaoffset(BufferA); // DMA RAM Start Address A
    DMA0STB = __builtin_dmaoffset(BufferB); // DMA RAM Start Address B

    IFS0bits.DMA0IF = 0; // Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1; // Set the DMA interrupt enable bit

    DMA0CONbits.CHEN = 1; // Enable DMA channel
}

/*=============================================================================
_DMA0Interrupt(): ISR name is chosen from the device linker script.
=============================================================================*/
unsigned int DmaBuffer = 0;
int flag = 0;

void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
    DmaBuffer ^= 1; // Ping-pong buffer select flag
    flag = 1; // Ping-pong buffer full flag
    IFS0bits.DMA0IF = 0; // Clear the DMA0 Interrupt Flag
}

/*=============================================================================
_DAC1RInterrupt(): ISR name is chosen from the device linker script.
=============================================================================*/
void __attribute__((interrupt, no_auto_psv)) _DAC1RInterrupt(void)
{
    IFS4bits.DAC1RIF = 0; // Clear Right Channel Interrupt Flag
}






