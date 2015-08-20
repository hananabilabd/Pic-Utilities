/** 
 * Serial library for dsPIC.
 *
 * Wiring:
 * |-------------------------------------------------|
 * | FTDI       dsPIC33                              |
 * |-------------------------------------------------|
 * | GND   --   GND  (pin 8)                         |
 * | VCC   --   VCC  (pin 13) (not needed/for power) |
 * | TX    ->   RX   (pin 15)                        |
 * | RX    <-   TX   (pin 16)                        |
 * |-------------------------------------------------|
 *
 * Inspired by:
 *     http://solar-blogg.blogspot.dk/2009/08/uart-example-for-dspic33.html
 * Thank you very much!
 */

#include <xc.h>
#include <stdint.h>
#include <string.h>

// Enable the use of __delay_ms()
#define FOSC 80000000ULL
#define FCY FOSC/2
#include <libpic30.h>

_FOSCSEL(FNOSC_FRCPLL); // Use PLL with oscillator

/**
 * Set up PLL for 80 MHz operation.
 */
void pll_init()
{
    CLKDIVbits.PLLPRE = 0;
    PLLFBDbits.PLLDIV = 42 - 2; // (divisor is 2 more than the value)
    CLKDIVbits.PLLPOST = 0;
    
    // 40 x: 73.700000 MHz = 36.850000 MIPS
    // 42 x: 77.385 MHz = 38.6925 MIPS
    // 43 x: 79.2275 MHz = 39.61375 MIPS
    
    while (OSCCONbits.LOCK != 1); // Wait for PLL to lock
    RCONbits.SWDTEN = 0; // Disable Watch Dog Timer
}

/**
 * Set up UART.
 */
void serial_init()
{
    U1MODE = 0x0000;
    U1BRG = 239; // Clock divisor
    U1STA = 0x0000;

    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
}

/**
 * Transmit a byte over UART.
 *
 * \param byte Byte to transmit.
 * \todo Read a "finish" flag instead of delay.
 */
void serial_writebyte(uint8_t byte)
{
    while (U1STAbits.UTXBF); // Wait if buffer full.
    U1TXREG = byte;
}

/**
 * Transmit a string, followed by a newline, over UART.
 *
 * \param str String to transmit.
 */
void serial_writeln(uint8_t *str)
{
    int i;
    for (i = 0; i < strlen(str); i++) {
        serial_writebyte(str[i]);
    }
    serial_writebyte('\n');
}

int main()
{
    ADPCFG = 0xFFFF; // make ADC pins all digital	

    pll_init();

    // Set up Serial port
    TRISBbits.TRISB7 = 0; // TX = RP7
    RPOR3bits.RP7R = 0b00011;
    TRISBbits.TRISB6 = 1; // RX = RP6
    RPINR18bits.U1RXR = 6;
    serial_init();

    while (1) {
        serial_writeln("Braller");
        __delay_ms(1000);
    }

    return 0;
}
