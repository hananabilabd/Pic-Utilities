#ifndef __ADCDRV1_H__
#define __ADCDRV1_H__ 

// Sampling Control
#define FOSC 73728000 // Hz
#define FCY (FOSC/2) // Hz
#define FVCO 2*FOSC // Hz

#define Fs 48000 // Hz
#define SAMPPRD (FCY/Fs)-1 // cycles per sample
#define NUMSAMP 256

// Functions
void adc_init(void);
void dac_init(void);
void timer3_init(void);
void dma0_init(void);

// Peripheral ISRs
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _DAC1RInterrupt(void);
#endif

