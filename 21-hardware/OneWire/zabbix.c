#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/ip.h>

#include "error_reporting.h"
#include "zabbix.h"

void zabbix_message_reinit(zabbix_msg_t *msg) {
	msg->size = 0;
	msg->is_item = 0;
	msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size, "{\"request\":\"sender data\",\"data\":[\n");
}

void zabbix_message_add_item(zabbix_msg_t *msg, const char *host, const char *item, const char *value) {
	char extra[3];
	if (msg->is_item) {
		extra[0] = ',';
		extra[1] = '\n';
		extra[2] = 0;
	} else {
		extra[0] = 0;
		msg->is_item = 1;
	}
	msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size,
		"%s\t{\"host\":\"%s\",\"key\":\"%s\",\"value\":\"%s\"}", extra, host, item, value);
}

void zabbix_message_add_item_t(zabbix_msg_t *msg, const char *host, const char *item, const char *value, int time) {
	char extra[3];
	if (msg->is_item) {
		extra[0] = ',';
		extra[1] = '\n';
		extra[2] = 0;
	} else {
		extra[0] = 0;
		msg->is_item = 1;
	}
	msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size,
		"%s\t{\"host\":\"%s\",\"key\":\"%s\",\"value\":\"%s\",\"clock\":%d}", extra, host, item, value, time);
}

void zabbix_message_add_item_f(zabbix_msg_t *msg, const char *host, const char *item, float value) {
	char extra[3];
	if (msg->is_item) {
		extra[0] = ',';
		extra[1] = '\n';
		extra[2] = 0;
	} else {
		extra[0] = 0;
		msg->is_item = 1;
	}
	msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size,
		"%s\t{\"host\":\"%s\",\"key\":\"%s\",\"value\":\"%f\"}", extra, host, item, value);
}

void zabbix_message_add_item_tf(zabbix_msg_t *msg, const char *host, const char *item, float value, int time) {
	char extra[3];
	if (msg->is_item) {
		extra[0] = ',';
		extra[1] = '\n';
		extra[2] = 0;
	} else {
		extra[0] = 0;
		msg->is_item = 1;
	}
	msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size,
		"%s\t{\"host\":\"%s\",\"key\":\"%s\",\"value\":\"%f\",\"clock\":%d}", extra, host, item, value, time);
}

int zabbix_message_finish(zabbix_msg_t *msg) {
	msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size, "]}");
	//msg->size += snprintf(msg->data + msg->size, msg->max_size - msg->size, "],\"clock\":%d}", time(0));
	if (msg->size > 0xfff0)
		return -1;
	else
		return 0;
}

int zabbix_write(int fd, zabbix_msg_t *msg) {
	int a;
	char header[13];
	
	/// prepare header
	header[0]='Z'; header[1]='B'; header[2]='X'; header[3]='D'; header[4]=0x01;
	header[5] = msg->size % 256;
	header[6] = msg->size / 256;
	for (a=7; a<13; a++) header[a] = 0;
	
	/// write data
	a = write(fd, header, 13);
	if (a != 13) {
		return -1;
	}
	a = write(fd, msg->data, msg->size);
	if (a != msg->size) {
		return -1;
	}
	
	return 0;
}

int zabbix_listen(int *sh) {
	char buf[100];
	struct timeval timeout;
	fd_set zabbix_fd_set;

	usleep(500000);
	
	FD_ZERO(&zabbix_fd_set);
	FD_SET(sh[0], &zabbix_fd_set);
	
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
		
	int ret = select(sh[0]+1, &zabbix_fd_set, NULL, NULL, &timeout);
	if (ret > 0) {
		ret=read(sh[0], buf, 100);
#ifdef DEBUG
		if (ret > 0) {
			buf[ret]=0;
			int i;
			for (i=13; i<ret; i++)
				if (buf[i]<0x20)
					buf[i]=' ';
			LOG_PRINT_DEBUG("%s", buf+13);
		}
#endif
	} else {
		LOG_PRINT_WARN("zabbix_listen - timeout or error in select()");
	}
	
	close(sh[0]);
	free(sh);
	
	return 0;
}

int zabbix_send(zabbix_msg_t *msg) {
	if ( zabbix_message_finish(msg) < 0 ) {
		LOG_PRINT_WARN("zabbix_send - too big message");
		return -1;
	}
	
	int *sh = malloc(sizeof(int));
	sh[0] = socket(PF_INET, SOCK_STREAM, 0);
	if (*sh<0) {
		LOG_PRINT_WARN("zabbix_send - socket: %m");
		return -2;
	}

	if ( connect(sh[0], (struct sockaddr *) msg->zabbix_server, sizeof(struct sockaddr_in)) ) {
		LOG_PRINT_WARN("zabbix_send - connect: %m");
		close(sh[0]);
		return -3;
	}

	if (zabbix_write(sh[0], msg) < 0){
		LOG_PRINT_WARN("zabbix_send - zabbix_write ERROR");
		close(sh[0]);
		return -4;
	}
	
	pthread_t response_listener;
	pthread_create( &response_listener, 0, (void* (*)(void *))zabbix_listen, (void *)sh );
	pthread_detach( response_listener );
	
	return 0;
}
