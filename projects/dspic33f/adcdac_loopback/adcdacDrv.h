#ifndef __ADCDRV1_H__
#define __ADCDRV1_H__ 

// Sampling Control
#define FOSC	 79227500     // Hz
#define FCY	 (FOSC/2)     // Hz
#define Fs   	 44113       // Hz
#define SAMPPRD  (FCY/Fs)-1   // Hz
#define NUMSAMP  256


// Functions
void initAdc(void);
void initDac(void);
void initTmr3(void);
void initDma0(void);

// Peripheral ISRs
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _DAC1RInterrupt(void);
#endif

