#ifndef __ADCDRV1_H__
#define __ADCDRV1_H__ 

// Sampling Control
#define FOSC 73728000 // Hz
#define FCY (FOSC/2) // Hz
#define FVCO 2*FOSC // Hz

#define Fs 48000 // Hz
#define SAMPPRD (FCY/Fs) // Hz
#define NUMSAMP 256

// Functions
void initAdc(void);
void initDac(void);
void initTmr3(void);
void initDma0(void);

// Peripheral ISRs
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _DAC1RInterrupt(void);
#endif

