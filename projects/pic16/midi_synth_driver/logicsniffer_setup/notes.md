1 bit @ 31250 baud = 32 us

MIDI
- 1 start bit = 0
- 8 data bits
- 1 stop bit = 1
- Logic 1: 0 mA = 5V after opto
- Logic 0: 5 mA = 0V after opto

10010000 = 0x90 = Start note
00110000 = 48 = C3
01010111 = 87 = Velocity

00110000 = 48 = 0x30 = C2
