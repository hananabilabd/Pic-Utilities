// vim: noexpandtab ts=8 sw=8
/**
 * \todo The code
\verbatim
while (1) {
	set_resistance(E1);
	__delay_ms(1000);
	set_resistance(A1);
	__delay_ms(1000);
	set_resistance(D2);
	__delay_ms(1000);
}
 \endverbatim
 * does not work. Only two tones are played.
 * Something may be wrong with the delay.
 */

#define _XTAL_FREQ (16000000ULL)

#include <xc.h>
#include <stdint.h>
#include <stddef.h>

#define SS_POTMETER LATCbits.LATC0

void fuse_init()
{
	OSCCON = 0b01111010; // Internal 16 MHz clock, no PLL
}

void pin_init()
{
	/*                          PIC16F1704
	 *        --------------------------------
	 *       | VDD                        VSS |
	 *       | RA5                RA0/ICSPDAT |
	 *       | RA4                RA1/ICSPCLK |
	 *       | VPP/MCLR#/RA3              RA2 |
	 *       | RC5                        RC0 | CS#
	 *    TX | RC4                        RC1 | SCK
	 *    RX | RC3                        RC2 | MOSI
	 *        --------------------------------
	 */


	LATC = 0b00000000; // All pins low
	TRISC = 0x0b00111000; // #SS, SCK, SDO = outputs
	ANSELC = 0b00000000; // Analog selection: No analog input
	WPUC = 0b00000000; // Disable weak pull-ups

	// Disable weak pull-ups
	OPTION_REGbits.nWPUEN = 0x01;

	// Store interrupt state and disable interrupts
	int state = GIE;
	GIE = 0;

	// Unlock sequence for PPS (port peripheral selection)
	PPSLOCK = 0x55;
	PPSLOCK = 0xAA;
	PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

	// Link pins and SPI
	SSPCLKPPS = 0x11; // RC1->MSSP:SCK (input)
	RC1PPS = 0x10; // RC1->MSSP:SCK (output)
	RC2PPS = 0x12; // RC2->MSSP:SDO (output)

	// Lock sequence for PPS
	PPSLOCK = 0x55;
	PPSLOCK = 0xAA;
	PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS

	// Restore interrupt state
	GIE = state;
}

void spi_init()
{
	// Clock source: FOSC/4
	SSP1CON1bits.SSPM = 0x00;

	// SPI mode 0,0
	SSP1STATbits.CKE = 1;
	SSP1CON1bits.CKP = 0;

	// Enable SSP and set chip select high
	SS_POTMETER = 1;
	SSP1CON1bits.SSPEN = 1;
}

uint8_t spi_transfer(uint8_t data)
{
	SSP1CON1bits.WCOL = 0; // Clear write-collision flag

	SSPBUF = data;
	while (!SSP1STATbits.BF); // Wait till buffer full
	return SSPBUF;
}

void set_resistance(uint32_t r)
{
	uint8_t reg;

	/*
	 * Wiper resistance is subtracted first (~70 ohm)
	 * 
	 * reg = R * 256/10000
	 *	 = R * 13>>9 (0.82% error)
	 *
	 * Resolution = 10000/256 = 39 ohm
	 *
	 * Time between transfers = 7.6 us
	 */
	if (70 < r)
		r -= 70;

	reg = 13 * r >> 9;

	SS_POTMETER = 0;
	spi_transfer(0b00010001);
	spi_transfer(reg);
	SS_POTMETER = 1;
}

// Test tones resistances
#define E1 6200
#define A1 8000
#define D2 9250

static uint32_t R = 10000;

int main(void)
{
	uint8_t read;

	fuse_init();
	pin_init();
	spi_init();

	uint16_t arr[6] = {E1,A1,D2,E1,A1,D2};
	
	while (1) {
		for (int i = 0; i < 6; i++) {
			set_resistance(arr[i]);
			__delay_ms(200);
		}
	}
	return 0;
}
