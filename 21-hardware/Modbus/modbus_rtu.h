#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <inttypes.h>
#include <termios.h>

int init_tty(const char *tty_device, int tty_baud_flag);

int modbus_write_read(int tty_fd, uint8_t addr, uint8_t func, uint16_t start_reg, uint8_t num_reg, void *data, uint16_t data_len, uint8_t verbose);

#endif
