#include <xc.h>
#include <stdint.h>
#include "dsp.h"
#include "adcdac.h"

static int buffer_a[NUMSAMP] __attribute__((space(dma))); // Buffer for data
static int buffer_b[NUMSAMP] __attribute__((space(dma))); // Buffer for data
static int *buffer[] = {buffer_a, buffer_b}; // Index buffer as buffer[a_or_b][sample]

/**
 * Initialize Analog-to-Digial converter.
 */
void adc_init(void)
{
    AD1CON1bits.FORM = 3;  // Signed format, Q15
    AD1CON1bits.SSRC = 2;  // Timer 3 starts conversion
    AD1CON1bits.ASAM = 1;  // Auto-start after convertion is finished
    AD1CON1bits.AD12B = 1; // 12-bit operation

    AD1CON2bits.CHPS = 0; // Converts CH0

    AD1CON3bits.ADRC = 0; // ADC clock is derived from system clock
    AD1CON3bits.ADCS = 3; // ADC clock period: TAD = (ADCS+1)*TCY = ~100ns

    AD1CON1bits.ADDMABM = 1; // DMA buffer build mode
    AD1CON2bits.SMPI = 0; // Increment DMA address after each sampling

    // Select input
    AD1CHS0bits.CH0SA = 0; // Vin+ = AN0
    AD1CHS0bits.CH0NA = 0; // Vin- = Vref

    AD1PCFGL = 0xFFFF;
    AD1PCFGLbits.PCFG0 = 0; // AN0 as analog input

    IFS0bits.AD1IF = 0;   // Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 0;   // Do Not Enable A/D interrupt

    AD1CON1bits.ADON = 1; // Turn on the A/D converter
}

/**
 * Initialize Audio DAC.
 */
void dac_init(void)
{
    /*
     * Set up DAC clock to be syncronous with Timer 3
     */
    ACLKCONbits.SELACLK = 0;  // FRC + PLL as clock
    ACLKCONbits.AOSCMD = 0;   // Disable ACLK (PLL used instead)
    ACLKCONbits.ASRCSEL = 0;  // Use auxillary oscillator (not used)
    ACLKCONbits.APSTSCLR = 7; // ACLK = FVCO/1 = 147.456 MHz

    DAC1STATbits.ROEN = 1; // Enable right DAC output
    DAC1DFLT = 0x8000;     // Default to mid-point

    /* 
     * Sampling Rate Fs = DACCLK/256 = 48000
     * Set DAC clock divider to resolve the above equation.
     */
    DAC1CONbits.DACFDIV = 11; // ACLK/(Fs*256) - 1 = 11

    DAC1CONbits.FORM = 1;  // Signed integer format (Q15)
    DAC1CONbits.AMPON = 0; // Amplifier setup @ sleep

    DAC1STATbits.RITYPE = 1; // Interrupt when FIFO is empty
    IFS4bits.DAC1RIF = 0;    // Clear Right Channel Interrupt Flag
    IEC4bits.DAC1RIE = 1;    // Right Channel Interrupt Enabled

    DAC1CONbits.DACEN = 1; // DAC1 Module Enabled
}

/**
 * Timer 3: Used for triggering ADC conversion.
 */
void timer3_init()
{
    TMR3 = 0x0000; // Clear TMR3
    PR3 = SAMPPRD; // Load period value in PR3
    IFS0bits.T3IF = 0; // Clear Timer 3 Interrupt Flag
    IEC0bits.T3IE = 0; // Clear Timer 3 interrupt enable bit

    T3CONbits.TON = 1; // Enable Timer 3
}

/**
 * DMA0: Move data from ADC0 to buffer.
 */
void dma0_init(void)
{
    DMA0CONbits.AMODE = 0; // Indirect addressing + post increment
    DMA0CONbits.MODE = 2;  // Continuous, Ping-Pong mode

    DMA0PAD = (int) &ADC1BUF0; // Read from: ADC
    DMA0CNT = (NUMSAMP - 1);   // "Number of samples per buffer" minus one

    /*
     * Both the ADC-sample-finished and DAC-output can be used as DMA trigger
     * as these are syncronized.
     */
    DMA0REQ = 13; // "ADC finished" as IRQ
    /* DMA0REQ = 78; // "DAC Right Output" as IRQ */

    DMA0STA = __builtin_dmaoffset(buffer_a); // DMA RAM start address A
    DMA0STB = __builtin_dmaoffset(buffer_b); // DMA RAM start address B

    IFS0bits.DMA0IF = 0; // Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1; // Set the DMA interrupt enable bit

    DMA0CONbits.CHEN = 1; // Enable DMA channel
}

static int dmabuffer = 0;
static int sample = 0;
int samplecnt = 0;
int samplemax = 0;

/**
 * Interrupt for each full buffer.
 */
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
    dmabuffer ^= 1;
    sample = 0;
    samplecnt = 0;

    IFS0bits.DMA0IF = 0; // Clear the DMA0 Interrupt Flag
}

/**
 * Interrupt for each output sample, Ts.
 */
void __attribute__((interrupt, no_auto_psv)) _DAC1RInterrupt(void)
{
    DAC1RDAT = buffer[dmabuffer][sample]; 

    // Simple Moving Average filter
    /* static int x[] = {0,0,0}; // Previous samples */
    /* DAC1RDAT = (buffer[dmabuffer][sample]>>2)  */
    /*     + (x[0]>>2) */
    /*     + (x[1]>>2) */
    /*     + (x[2]>>2); */
    /* x[2] = x[1]; */
    /* x[1] = x[0]; */
    /* x[0] = buffer[dmabuffer][sample]; */

    if (++sample >= NUMSAMP) {
        sample = NUMSAMP-1;
    }

    // Count samples per buffer
    if (samplemax < samplecnt++)
        samplemax = samplecnt;

    IFS4bits.DAC1RIF = 0;
}
