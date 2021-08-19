/*
 * prosta implementacja komunikacji Modbus RTU obejmująca:
 *  - konfigurację portu szeregowego
 *  - wysłanie zapytania do urządzenia slave i odbiór odpowiedzi
 * implementacja używa uproszczonego sprawdzania poprawności czasu odpowiedzi
 * (nie weryfikuje przerw między znakowych, nie oblicza dokładnych timeoutów)
 */

#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <netinet/in.h>

#include "error_reporting.h"

#include "crc.c"
inline static uint16_t modbus_check_sum(uint8_t *data, int buf_pos) {
	return crcSlow((unsigned char* const)data, (int)buf_pos);
}

#define TTY_READ_TIMEOUT_1 100000
#define TTY_READ_TIMEOUT_2  40000

int init_tty(const char *tty_device, int tty_baud_flag) {
	struct termios term;
	int tty_fd;
	
	tty_fd = open(tty_device, O_RDWR | O_NOCTTY | O_SYNC);
	if (tty_fd < 0) {
		LOG_PRINT_ERROR("Opening serial port: %m");
		return tty_fd;
	} else {
		tcflush(tty_fd, TCIFLUSH);
		//fcntl(tty_fd, F_SETFL, FASYNC);
		
		memset(&term, 0, sizeof(term));
		
		term.c_iflag = IGNBRK | IGNPAR /* bez parzystości */;
		term.c_oflag = term.c_lflag = term.c_line = 0;
		term.c_cflag = CREAD |  CLOCAL | tty_baud_flag | CS8 /* 8 bitow */;
		term.c_cc[VMIN]=1;
		term.c_cc[VTIME]=0;
		tcsetattr (tty_fd, TCSAFLUSH, &term);
		
		tcflush(tty_fd, TCIOFLUSH);
	}
	
	return tty_fd;
}

int modbus_write_read(int tty_fd, uint8_t addr, uint8_t func, uint16_t start_reg, uint8_t num_reg,
                      void *data, uint16_t data_len, uint8_t verbose) {
	uint8_t buft[260];
	uint8_t* buf = buft;
	int buf_pos; // pozycja w buforze (W BAJTACH)
	uint16_t check_sum, i;
	int ret;
	struct timeval timeout;
	fd_set tty_fd_set;
	
	
	/// prepare modbus frame
	buf_pos = 0;
	buf[buf_pos++] = addr;
	buf[buf_pos++] = func;
	*(uint16_t*)(buf + buf_pos) = htons(start_reg);
	buf_pos += 2;
	
	if (func == 0x03 || func == 0x04) { /// read
		buf[buf_pos++] = 0;
		buf[buf_pos++] = num_reg;
	} else if (func == 0x10) {          /// write multiple
		uint16_t *reg_value = (uint16_t *)data;
		buf[buf_pos++] = 0;
		buf[buf_pos++] = num_reg;
		for (i=0; i<num_reg; i++) {
			*(uint16_t*)(buf + buf_pos) = htons(reg_value[i]);
			buf_pos += 2;
		}
	} else if (func == 0x06) {          /// write single
		*(uint16_t*)(buf + buf_pos) = htons( *(uint16_t *)data );
		buf_pos += 2;
	} else {
		return -10;
	}
	
	*(uint16_t*)(buf + buf_pos) = modbus_check_sum(buf, buf_pos);
	buf_pos += 2;
	
	
	/// sending modbus frame
#ifdef DEBUG
	uint8_t tmpbuf[1310];
	for (i=0; i < buf_pos; i++)
		sprintf(tmpbuf + i*5, "0x%02x ", buf[i]);
	LOG_PRINT_DEBUG("REQUEST: %s", tmpbuf);
#endif
	
	ret = write(tty_fd, buf, buf_pos);
	if (ret != buf_pos) {
		LOG_PRINT_WARN("Error write data from device: %m");
		return -11;
	}
	
	
	/// receive modbus response
	FD_ZERO(&tty_fd_set);
	FD_SET(tty_fd, &tty_fd_set);
	timeout.tv_sec = 0;
	timeout.tv_usec = TTY_READ_TIMEOUT_1;
	ret = select(tty_fd+1, &tty_fd_set, NULL, NULL, &timeout);
#ifdef DEBUG
	LOG_PRINT_DEBUG("SELECT RETURN: %d", ret);
#endif
	if (ret > 0) {
		buf_pos = read(tty_fd, buf, 260);
		if (buf_pos < 0) {
			LOG_PRINT_WARN("Error read data from device - read: %m");
			return -21;
		}
		// doczytanie reszty pakietu
		do {
			FD_ZERO(&tty_fd_set);
			FD_SET(tty_fd, &tty_fd_set);
			timeout.tv_sec = 0;
			timeout.tv_usec = TTY_READ_TIMEOUT_2;
			ret = select(tty_fd+1, &tty_fd_set, NULL, NULL, &timeout);
			if (ret>0) {
				buf_pos += read(tty_fd, buf+buf_pos, 260-buf_pos);
				if (buf_pos < 0) {
					LOG_PRINT_WARN("Error read data from device - read (2): %m");
					return -22;
				}
			}
		} while (ret>0);
	} else if (ret == 0) {
		LOG_PRINT_WARN("Timeout on read from device: %d, start_reg: %d", addr, start_reg);
		return -20;
	} else {
		LOG_PRINT_WARN("Error read data from device - select: %m");
		return -23;
	}
	
	while (buf[0] == 0) {
		buf++;
		buf_pos--;
	}
	
	/// checking modbus response
	if (buf_pos < 3) {
		LOG_PRINT_WARN("Too short response from device: %d, start_reg: %d", addr, start_reg);
		return -24;
	}
	check_sum = modbus_check_sum(buf, buf_pos-2);
	
#ifdef DEBUG
	for (i=0; i < buf_pos; i++)
		sprintf(tmpbuf + i*5, "0x%02x ", buf[i]);
	LOG_PRINT_DEBUG("RESPONSE: %s", tmpbuf);
	LOG_PRINT_DEBUG("RESPONSE CHECK SUM: 0x%04x\n", check_sum);
#endif

	if ( *(uint16_t*)(buf+buf_pos-2) != check_sum) {
		LOG_PRINT_WARN("Error read data (reg=%d) from device %d - check_sum", start_reg, addr);
		return -30;
	}
	
	if ( buf[1] != func ) {
		LOG_PRINT_WARN("Error read data (reg=%d) from device %d - modbus error code: %d", start_reg, addr, buf[2]);
		return -31;
	}
	
	if (func == 0x03 || func == 0x04) {
		for (i=0; i < buf[2]; i++) {
			if (i == data_len) {
				LOG_PRINT_WARN("Output Buffer Overflow");
				return -32;
			} else {
				*(uint8_t *)(data+i) = buf[i+3];
			}
		}
		return buf[2];
	}
	
	return 0;
}
