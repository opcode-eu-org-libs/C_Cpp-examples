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
//     gcc       1wire_temperatura.c
//   dla libi2c-dev >= 4:
//     gcc -li2c 1wire_temperatura.c


int main(int argc, char **argv) {
	int i2c_fd, ret, i, len;
	uint8_t ds18b20_addr[8];
	
	if (argc != 9) {
		printf("USAGE: %s space sepatred 8 bytyes of ds18b20 thermometer address\n", argv[0]);
		return 1;
	}
	printf("Using 1wire address: ");
	for (i=0; i<8; i++) {
		ds18b20_addr[i]=strtoul(argv[i+1], 0, 0);
		printf("0x%X ", ds18b20_addr[i]);
	}
	printf("ON 0x%X i2c device on %s\n", DS2482_ADDR, I2C_DEVICE);
	
	/// otwarcie i konfiguracja mostka i2c - 1wire
	i2c_fd = open_i2c_device(I2C_DEVICE, DS2482_ADDR);
	if (i2c_fd < 0) {
		printf("ERROR WHILE OPENING I2C (%d)\n", i2c_fd);
		return 3;
	}
	if (ret = i2c_smbus_write_byte(i2c_fd, DS2482_RESET)) {
		printf("ERROR IN I2C COMMUNICATION (%d)\n", ret);
		return 4;
	}
	if (ret = i2c_smbus_write_byte_data(i2c_fd, DS2482_WRITE_CONFIG, 0xe1) < 0) {
		printf("ERROR IN I2C COMMUNICATION (%d)\n", ret);
		return 5;
	}
	
	
	/// polecenie konwersji temperatury dla DS18B20
	uint8_t ow_buf[12];
	if (ret= ds2482_reset(i2c_fd)) {
		printf("ERROR IN RESET (%d)\n", ret);
		return 5;
	}
	
	#ifdef USE_CONVERT_ALL
		ow_buf[0]=0xCC;
		ow_buf[1]=0x44;
		len=1;
	#else
		ow_buf[0]=0x55;
		for(i=0; i<8; i++){
			ow_buf[i+1] = ds18b20_addr[i];
		}
		ow_buf[9]=0x44;
		len=9;
	#endif
	
	for (i=0; i <len; i++) {
		ret = ds2482_send_and_get(i2c_fd, ow_buf[i]);
		if (ret<0) {
			printf("ERROR IN SET/GET (%d)\n", ret);
			return 6;
		}
	}
	
	i2c_smbus_write_byte_data(i2c_fd, DS2482_WRITE_CONFIG, 0xa5); // "power byte"
	
	ret = ds2482_send_and_get(i2c_fd, ow_buf[len]);
	if (ret<0) {
		printf("ERROR IN SET/GET (%d)\n", ret);
		return 6;
	}
	
	
	sleep(1);
	
	
	/// odczyt temperatury z DS18B20
	if (ret= ds2482_reset(i2c_fd)) {
		printf("ERROR IN RESET (%d)\n", ret);
		return 7;
	}
	
	ow_buf[0]=0x55;
	for(i=0; i<8; i++){
		ow_buf[i+1] = ds18b20_addr[i];
	}
	ow_buf[9]=0xbe;
	ow_buf[10]=0xff;
	ow_buf[11]=0xff;
	
	for (i=0; i <12; i++) {
		ret = ds2482_send_and_get(i2c_fd, ow_buf[i]);
		if (ret<0) {
			printf("ERROR IN SET/GET (%d)\n", ret);
			return 8;
		}
		ow_buf[i] = ret;
	}
	
	
	/// parsowanie temperatury
	if (
		(ow_buf[10]==0x50 && ow_buf[11]==0x05) || \
		(ow_buf[10]==0xf8 && ow_buf[11]==0x07) || \
		(ow_buf[10]==0xff && ow_buf[11]==0xff)
	) {
		printf("ERROR in read temperature");
		return 9;
	} else {
		float val = (short)(ow_buf[11] << 8 | ow_buf[10]) / 16.0;
		printf("TEMPERATURE: %f\n", val);
		return 0;
	}
}
