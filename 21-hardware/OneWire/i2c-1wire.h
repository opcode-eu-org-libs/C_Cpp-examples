#ifndef I2C_1WIRE_H_
#define I2C_1WIRE_H_

// wymagamy biblioteki "libi2c-dev" z http://www.lm-sensors.org
#include <linux/i2c-dev.h>
#ifndef I2C_FUNC_I2C
#include <i2c/smbus.h>
#endif

#include <sys/ioctl.h>

#define DS2482_RESET              0xF0
#define DS2482_SET_READ_PTR       0xE1
#define DS2482_WRITE_CONFIG       0xD2
#define DS2482_1WIRE_RESET        0xB4
#define DS2482_1WIRE_WRITE_BYTE   0xA5
#define DS2482_DATA               0xE1

int ds2482_reset(int fd) {
	int i, status;
	if (i2c_smbus_write_byte(fd, DS2482_1WIRE_RESET))
		return -1;
	
	for (i=0; i<10; i++) {
		usleep(2);
		status = i2c_smbus_read_byte(fd);

		if (status < 0) {
			return -2;
		} else if ((status & 0x01) == 0) { // 1-wire NOT busy
			return 0;
		}
	}
		
	return -9;
}

int ds2482_send_and_get(int fd, char wr) {
	int i, status;
	if ( i2c_smbus_write_byte_data(fd, DS2482_1WIRE_WRITE_BYTE, wr) < 0 )
		return -1;
	
	for (i=0; i<10; i++) {
		usleep(2);
		status = i2c_smbus_read_byte(fd);

		if (status < 0) {
			return -2;
		} else if ((status & 0x01) == 0) { // 1-wire NOT busy
			if (i2c_smbus_write_byte_data(fd, DS2482_SET_READ_PTR, DS2482_DATA) < 0)
				return -3;
			
			return i2c_smbus_read_byte(fd);
		}
	}

	return -9;
}

int open_i2c_device(const char *dev_file, int addr) {
	// open i2c bus interface
	int fd = open(dev_file, O_RDWR);
	if (fd < 0)
		return -1;
	
	// set slave addres
	if (ioctl(fd, I2C_SLAVE, addr) < 0)
		return -2;
	
	return fd;
}

#endif
