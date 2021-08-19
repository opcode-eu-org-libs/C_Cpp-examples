/*
 * Program nasłuchujący na interfejsie szeregowym i zrzucający komunikację Modbus RTU w postaci hexalnej.
 * 
 * kompilacja: gcc modbus_rtu.c modbus_listener.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/time.h>

#include "error_reporting.h"
#include "modbus_rtu.h"

int modbus_listen(int tty_fd, int cnt_max, int speed) {
	struct timeval timeout;
	fd_set tty_fd_set;
	int cnt=0, ret;
	uint8_t ctrl=0, buf;
	
	while (cnt_max == 0 || cnt < cnt_max) {
		FD_ZERO(&tty_fd_set);
		FD_SET(tty_fd, &tty_fd_set);
		
		timeout.tv_sec = 0;
		timeout.tv_usec = (1000000 * 8 * 2.5 / speed);
		
		ret = select(tty_fd+1, &tty_fd_set, NULL, NULL, &timeout);
		if (ret > 0) {
			ret = read(tty_fd, &buf, 1);
			if (ret < 1) {
				perror("Error read data from device - read");
				return -2;
			}
			printf("0x%02x ", buf);
			cnt++;
			ctrl = 0;
		} else if (ret == 0) {
			if (ctrl == 0) {
				ctrl=1;
				printf("\n");
			}
		} else {
			perror("Error read data from device - select");
			return -3;
		}
	}
	printf("\n");
	return 0;
}

int main(int argc, char **argv) {
	// initialize RS485
	int serial = init_tty("/dev/ttyS3", B19200);
	if ( serial < 0 ) {
		LOG_PRINT_CRIT("Open TTY ERROR");
		exit(-1);
	}
	
	puts("LISTENING MODE");
	modbus_listen(serial, 0, 19200);
	close(serial);
	return 0;
}
