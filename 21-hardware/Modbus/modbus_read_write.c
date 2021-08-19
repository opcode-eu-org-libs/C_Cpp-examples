/*
 * Program wysyłający zapytanie do wskazanego urządzenia Modbus o pojedynczy rejestr i odbierający odpowiedź.
 * 
 * Zapytania typu odczyt realizowane są jako funkcja 3, a zapis jako funkcja 6.
 * 
 * kompilacja: gcc modbus_rtu.c modbus_read_write.c
 */

#include <inttypes.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <netinet/in.h>

#include <signal.h>

#include "modbus_rtu.h"
#include "error_reporting.h"

#define MODBUS_READ_BUF_SIZE 255
uint8_t modbus_read_buf[MODBUS_READ_BUF_SIZE];

int main(int argc, char **argv) {
	if (argc < 5) {
		fprintf(stderr, "USAGE: %s tty_device_path 2400|4800|9600|19200|38400 modbus_dev_address modbus_register [value]\n", argv[0]);
		exit(1);
	}
	int baud;
	switch (strtoul(argv[2], NULL, 0)) {
		case 2400:  baud = B2400; break;
		case 4800:  baud = B4800; break;
		case 9600:  baud = B9600; break;
		case 19200: baud = B19200; break;
		case 38400: baud = B38400; break;
		default:
			fprintf(stderr, "Unsupported baud value\n");
			exit(2);
	}
	int addr=strtoul(argv[3], 0, 0);
	int reg=strtoul(argv[4], 0, 0);

	int fun;
	if (argc > 5) {
		int val = htons( strtoul(argv[5], 0, 0) );
		modbus_read_buf[0] = val >> 8;
		modbus_read_buf[1] = val & 0xff;
		fun = 6;
	} else {
		fun = 3;
	}
	
	// initialize RS485
	int serial = init_tty(argv[1], baud);
	if ( serial < 0 ) {
		LOG_PRINT_CRIT("Open TTY ERROR\n");
		exit(3);
	}
	
	// read / write data from modbus
	int ret = modbus_write_read(serial,            // SERIAL: port_descriptor
	                            addr, fun, reg, 1, // MODBUS: addres, function, start_reg, number_of_regs
	                            modbus_read_buf, MODBUS_READ_BUF_SIZE, 0);
	
	// print output
	if (ret < 0) {
		LOG_PRINT_ERROR("modbus_write_read() in %s:%d for addr=%d, base_reg=%d exit with code %d",
				__FILE__, __LINE__, addr, reg, ret);
		exit(4);
	} else if (fun == 3) {
		printf("OK for addr=%d, base_reg=%d: 0x%X\n", addr, reg, ntohs(*(uint16_t *)(modbus_read_buf)));
		exit(0);
	}
}
