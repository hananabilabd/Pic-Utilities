#define _XTAL_FREQ (16000000ULL)

#include <xc.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define SS_POTMETER LATCbits.LATC0

void fuse_init()
{
    OSCCON = 0b01111010; // Internal 16 MHz clock, no PLL
}

// Lock or unlock PPS (pin setup)
// status  = 0: Unlock
// status != 0: Lock
void pps_lock(int status)
{
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    if (status == 0)
        // Unlock sequence for PPS (port peripheral selection)
        PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    else
        // Lock sequence for PPS
        PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS

}

void pin_init()
{
    /*                   PIC16F1704
     *        --------------------------------
     *       | VDD                        VSS |
     *       | RA5                RA0/ICSPDAT |
     *       | RA4                RA1/ICSPCLK |
     *       | VPP/MCLR#/RA3              RA2 | TEST LED
     *       | RC5                        RC0 | CS# (blue)
     *    TX | RC4                        RC1 | SCK (purple)
     *    RX | RC3                        RC2 | MOSI (gray)
     *        --------------------------------
     */

    LATC = 0b00000000; // All pins low
    TRISC = 0b00001000; // #SS, SCK, SDO, TX = outputs
    ANSELC = 0b00000000; // Analog selection: No analog input
    WPUC = 0b00000000; // Disable weak pull-ups

    TRISAbits.TRISA2 = 0; // Output test led
    LATAbits.LATA2 = 0;

    // Disable weak pull-ups
    OPTION_REGbits.nWPUEN = 0x01;

    // Store interrupt state and disable interrupts
    int state = GIE;
    GIE = 0;

    // Unlock PPS
    pps_lock(0);

    // Link pins and SPI
    SSPCLKPPS = 0x11; // RC1->MSSP:SCK (input)
    RC1PPS = 0x10; // RC1->MSSP:SCK (output)
    RC2PPS = 0x12; // RC2->MSSP:SDO (output)

    // Link pins and EUSART
    RXPPS = 0x13; // UART RX
    RC4PPS = 0x14; // UART TX

    // Lock PPS
    pps_lock(1);

    // Restore interrupt state
    GIE = state;
}

#define SERIAL_BAUD 9600
void serial_init()
{
    // Baud rate
    // 1. Initialize the SPBRGH, SPBRGL register pair and the BRGH and BRG16
    // bits to achieve the desired baud rate (see Section 29.4 "EUSART Baud
    // Rate Generator (BRG)").
    SPBRGH = 0;
    SPBRGL = _XTAL_FREQ/SERIAL_BAUD/64 - 1;
    TX1STAbits.BRGH = 0;
    BAUD1CONbits.BRG16 = 0;

    // 2. Enable the asynchronous serial port by clearing the SYNC bit and
    // setting the SPEN bit.
    TX1STAbits.SYNC = 0;
    RC1STAbits.SPEN = 1;

    // 4. If interrupts are desired, set the RCIE bit of the PIE1 register
    // and the GIE and PEIE bits of the INTCON register.
    PIE1bits.RCIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;

    // 5. Enable the transmission by setting the TXEN control bit. This
    // will cause the TXIF interrupt bit to be set.
    TX1STAbits.TXEN = 1;

    // 6. Enable reception by setting the CREN bit.
    RC1STAbits.CREN = 1;

    // 8. Load 8-bit data into the TXREG register. This will start the
    // transmission.
}

void serial_printbyte(uint8_t byte)
{
    while (!PIR1bits.TXIF); // Wait for buffer space...
    TXREG = byte;
}

void serial_print(uint8_t *str)
{
    int i;
    for (i = 0; i < strlen(str); i++) {
        serial_printbyte(str[i]);
    }
}

void serial_println(uint8_t *str)
{
    serial_print(str);
    serial_printbyte('\n');
}

void interrupt ISR()
{
    // UART Receiver
    if (PIR1bits.RCIF) {
        static uint8_t rx[1];
        /* while (!PIR1bits.RCIF); */

        if (RC1STAbits.OERR) {
            // Overflow occured
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
        }
        rx[0] = RC1REG;
        serial_println(rx);

        PIR1bits.RCIF = 0;
    }
}

int main(void)
{
    fuse_init();
    pin_init();
    serial_init();

    while (1) {
        serial_println("Hello");
        __delay_ms(10);
    }
    return 0;
}
