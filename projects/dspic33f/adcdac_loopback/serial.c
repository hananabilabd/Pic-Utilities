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

/**
 * Set up UART.
 */
void serial_init()
{
    TRISBbits.TRISB7 = 0; // TX = RP7
    RPOR3bits.RP7R = 0b00011;
    TRISBbits.TRISB6 = 1; // RX = RP6
    RPINR18bits.U1RXR = 6;

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
