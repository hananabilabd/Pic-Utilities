#include <xc.h>
#include <stdint.h>
#include "dsp.h"
#include "adcdacDrv.h"

static int buffer_a[NUMSAMP] __attribute__((space(dma))); // Buffer for data
static int buffer_b[NUMSAMP] __attribute__((space(dma))); // Buffer for data
static int *buffer[] = {buffer_a, buffer_b}; // Index buffer as buffer[a_or_b][sample]

#define TOGGLE_LED do {\
    static int __macro_led = 0;\
    LATBbits.LATB3 = __macro_led;\
    __macro_led ^= 1; } while (0);

/**
 * Initialize Analog-to-Digial converter.
 */
void initAdc(void)
{
    AD1CON1bits.FORM = 3;  // Signed format, Q15
    AD1CON1bits.SSRC = 2;  // Timer 3 starts conversion
    AD1CON1bits.ASAM = 1;  // Auto-start after convertion is finished
    AD1CON1bits.AD12B = 1; // 12-bit operation

    AD1CON2bits.CHPS = 0; // Converts CH0

    AD1CON3bits.ADRC = 0; // ADC clock is derived from system clock
    AD1CON3bits.ADCS = 3; // ADC clock period: TAD = (ADCS+1)*TCY = 100ns

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
void initDac(void)
{
    /* Initiate DAC Clock */
    // Set up DAC clock to be syncronous with Timer 3
    ACLKCONbits.SELACLK = 0; // FRC + PLL as clock
    ACLKCONbits.AOSCMD = 0; // Disable ACLK (PLL used instead)
    ACLKCONbits.ASRCSEL = 0; // Use auxillary oscillator (not used)
    ACLKCONbits.APSTSCLR = 7; // ACLK = FVCO/1 = 147.456 MHz

    DAC1STATbits.ROEN = 1; // Right Channel DAC Output Enabled
    DAC1DFLT = 0x8000; // DAC Default value is the midpoint

    // Sampling Rate Fs = DACCLK/256 = 48000
    DAC1CONbits.DACFDIV = 12; // ACLK / (Fs*256) = 12

    DAC1CONbits.FORM = 1; // Data Format is signed integer
    DAC1CONbits.AMPON = 0; // Analog Output Amplifier is enabled during Sleep Mode/Stop-in Idle mode

    DAC1STATbits.RITYPE = 1; // Interrupt when FIFO is empty
    IFS4bits.DAC1RIF = 0; // Clear Right Channel Interrupt Flag
    IEC4bits.DAC1RIE = 1; // Right Channel Interrupt Enabled

    DAC1CONbits.DACEN = 1; // DAC1 Module Enabled
}

/**
 * Timer 3: Used for triggering ADC conversion.
 */
void initTmr3()
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
void initDma0(void)
{
    DMA0CONbits.AMODE = 0; // Configure DMA for Register indirect with post increment
    DMA0CONbits.MODE = 2; // Configure DMA for Continuous, Ping-Pong mode

    DMA0PAD = (int) &ADC1BUF0; // Peripheral Address Register: ADC buffer
    DMA0CNT = (NUMSAMP - 1); // DMA Transfer Count is (NUMSAMP-1)

    DMA0REQ = 13; // ADC interrupt selected for DMA channel IRQ

    DMA0STA = __builtin_dmaoffset(buffer_a); // DMA RAM start address A
    DMA0STB = __builtin_dmaoffset(buffer_b); // DMA RAM start address B

    IFS0bits.DMA0IF = 0; // Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1; // Set the DMA interrupt enable bit

    DMA0CONbits.CHEN = 1; // Enable DMA channel
}

static int dmabuffer = 0;
static int sample = 0;

/**
 * Interrupt for each full buffer.
 */
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
    dmabuffer ^= 1;
    sample = 0;
    TOGGLE_LED;
    IFS0bits.DMA0IF = 0; // Clear the DMA0 Interrupt Flag
}

int samplemax = 0;
/**
 * Interrupt for each output sample, Ts.
 */
void __attribute__((interrupt, no_auto_psv)) _DAC1RInterrupt(void)
{
    DAC1RDAT = buffer[dmabuffer][sample];
    if (++sample >= NUMSAMP)
        sample = NUMSAMP-1;

    if (samplemax < sample)
        samplemax = sample;
    IFS4bits.DAC1RIF = 0;
}
