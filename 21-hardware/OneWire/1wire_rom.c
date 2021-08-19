#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define I2C_DEVICE "/dev/i2c-1"
#define DS2482_ADDR 0x18

#define NO_USE_SYSLOG
#include "error_reporting.h"

#include "i2c-1wire.h"

// wymagany pakiet libi2c-dev, kompilacja:
//   dla libi2c-dev < 4:
//     gcc       1wire_rom.c
//   dla libi2c-dev >= 4:
//     gcc -li2c 1wire_rom.c


int main(int argc, char **argv) {
	int i2c_fd, ret, i;
	
	/// otwarcie i konfiguracja mostka i2c - 1wire
	i2c_fd = open_i2c_device(I2C_DEVICE, DS2482_ADDR);
	if (i2c_fd < 0) {
		printf("ERROR WHILE OPENING I2C (%d)\n", i2c_fd);
		return 5;
	}
	if (ret = i2c_smbus_write_byte(i2c_fd, DS2482_RESET)) {
		printf("ERROR IN I2C COMMUNICATION (%d)\n", ret);
		return 5;
	}
	if (ret = i2c_smbus_write_byte_data(i2c_fd, DS2482_WRITE_CONFIG, 0xe1) < 0) {
		printf("ERROR IN I2C COMMUNICATION (%d)\n", ret);
		return 5;
	}
	
	/// odczyt ROMu urządzenia 1wire
	uint8_t ow_buf[12];
	if (ret= ds2482_reset(i2c_fd)) {
		printf("ERROR IN RESET (%d)\n", ret);
		return 5;
	}
	
	ow_buf[0]=0x33;
	for (i=1; i <10; i++) {
		ow_buf[i] = 0xff;
	}
	for (i=0; i <12; i++) {
		ret = ds2482_send_and_get(i2c_fd, ow_buf[i]);
		if (ret<0) {
			printf("ERROR IN SET/GET (%d)\n", ret);
			return 6;
		}
		ow_buf[i] = ret;
	}
	printf("1-WIRE ROM ADDRESS: ");
	for (i=1; i <9; i++) {
		printf("0x%X ", ow_buf[i]);
	}
	printf("\n");
	
	return 0;
}
