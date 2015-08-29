#ifndef SERIAL_H
#define SERIAL_H
void serial_init();
void serial_writebyte(uint8_t byte);
void serial_writeln(uint8_t *str);
#endif
