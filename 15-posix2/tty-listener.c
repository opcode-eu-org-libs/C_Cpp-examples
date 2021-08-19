/*
	Prosta implementacja programu podsłuchującego wybrany terminal tty.
	
	Działająca wg poniższego schematu:
	width=`od -j1 -N1 -d /dev/vcsa1 | cut -b8-`
	fold -w $width /dev/vcs1 > /tmp/new
	if ! diff /tmp/new /tmp/old; then
		mv /tmp/new /tmp/old
		render /tmp/old
	fi
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/vt.h>

#define DEVICE_SURFIX 1  // podsłuchiwany będzie terminal tty o podanym tutaj numerze

int getConsole(char devSurfix, uint8_t *w, uint8_t *h) {
	int fd;
	char tmpBuf[20];
	
	snprintf(tmpBuf, 20, "/dev/vcsa%d", devSurfix);
	printf("Console on %s ", tmpBuf);
	
	fd = open(tmpBuf, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s: %m\n", tmpBuf);
		exit(1);
	}
	if (read(fd, tmpBuf, 4) != 4) {
		fprintf(stderr, "read %s: %m\n", tmpBuf);
		exit(2);
	}
	close(fd);
	
	*h = tmpBuf[0];
	*w = tmpBuf[1];
	printf(" has %d rows and %d cols\n", *h, *w);
	
	
	snprintf(tmpBuf, 20, "/dev/vcs%d", devSurfix);
	printf("Openning console on %s \n", tmpBuf);

	fd = open(tmpBuf, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open %s: %m\n", tmpBuf);
		exit(3);
	}
	
	return fd;
}

int main() {
	int vcsFD, myFD;
	struct vt_stat vtstat;
	
	uint8_t width, height, y;
	ssize_t sizeIN, sizeOUT;
	char    *bufIN_1, *bufIN_2, *bufOUT, *bufTMP, charTMP;
	
	vcsFD = getConsole(DEVICE_SURFIX, &width, &height);
	myFD = open("/dev/tty0", O_RDONLY);
	if (myFD < 0) {
		fprintf(stderr, "read /dev/tty0: %m\n");
		exit(1);
	}
	
	sizeIN  = width * height;
	bufIN_1 = malloc(sizeIN);
	bufIN_2 = malloc(sizeIN);
	sizeOUT = (width+1) * height;
	bufOUT  = malloc(sizeOUT);
	
	*bufIN_2 = '\0';
	
	while (1) {
		ioctl(myFD, VT_GETSTATE, &vtstat);
		if (vtstat.v_active == DEVICE_SURFIX) { // run only when DEVICE_SURFIX is active VT
			lseek(vcsFD, 0, SEEK_SET);
			if (read(vcsFD, bufIN_1, sizeIN) == sizeIN) {
				if ( strncmp(bufIN_1, bufIN_2, sizeIN) != 0 ) { // redraw only when changed VT output
					bufTMP = bufIN_1;
					for (y=0; y<height; y++) {
						// save first char of next line and put in its place "end of string"
						charTMP = bufTMP[width];
						bufTMP[width] = '\0';
						
						// show line
						puts(bufTMP);
						
						// set bufTMP to start of next line and restore first char of next line
						bufTMP = bufTMP + width;
						*bufTMP = charTMP;
					}
					
					// swap bufIN_2 and bufIN_1
					bufTMP  = bufIN_2;
					bufIN_2 = bufIN_1;
					bufIN_1 = bufTMP;
				}
			}
		}
		usleep(100000);
	}
}
