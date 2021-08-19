#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sqlite3.h>

// sqlite3 /tmp/onewire.db 'CREATE TABLE ds18b20 (A INT, B INT, C INT, D INT, E INT, F INT, G INT, H INT, host TEXT, item TEXT);'
// sqlite3 /tmp/onewire.db 'INSERT INTO ds18b20 VALUES (40, 192, 38, 115, 2, 0, 0, 138, "test", "test");'

#include "error_reporting.h"
#include "zabbix.h"

#define I2C_DEVICE "/dev/i2c-0"
#define DS2482_ADDR 0x18
#define DATABESE_FILE "/tmp/onewire.db"

//#define USE_CONVERT_ALL 1

#define ZABBIX_SERWER_IP "192.168.119.13"
#define ZABBIX_SERWER_PORT 10051
#define ZABBIX_MESSAGE_BUF_SIZE 8192
char zabbix_message_buf[ZABBIX_MESSAGE_BUF_SIZE];

#include "i2c-1wire.h"

// wymagany pakiet libi2c-dev oraz libsqlite3-dev, kompilacja:
//   dla libi2c-dev < 4:
//     gcc -lsqlite3       -lpthread zabbix.c 1wire_temperatura-zabbix.c
//   dla libi2c-dev >= 4:
//     gcc -lsqlite3 -li2c -lpthread zabbix.c 1wire_temperatura-zabbix.c



typedef struct sql_callbac_parm_st sql_callbac_parm_t;
struct sql_callbac_parm_st {
	zabbix_msg_t *zabbix_msg;
	int i2c_fd;
};

int temp_conv(sql_callbac_parm_t *param, int argc, char **argv, char **argv_names) {
	char ow_buf[10];
	int i;
	
	ds2482_reset(param->i2c_fd);
	
	ow_buf[0]=0x55;
	for(i=0; i<8; i++){
		ow_buf[i+1] = atoi(argv[i]);
	}
	ow_buf[9]=0x44;
	
	for (i=0; i <9; i++) {
		ds2482_send_and_get(param->i2c_fd, ow_buf[i]);
	}
	i2c_smbus_write_byte_data(param->i2c_fd, DS2482_WRITE_CONFIG, 0xa5); // "power byte"
	ds2482_send_and_get(param->i2c_fd, ow_buf[9]);
	
	return 0;
}

int temp_conv_all(sql_callbac_parm_t *param) {
	ds2482_reset(param->i2c_fd);
	
	ds2482_send_and_get(param->i2c_fd, 0xCC);
	i2c_smbus_write_byte_data(param->i2c_fd, DS2482_WRITE_CONFIG, 0xa5); // "power byte"
	ds2482_send_and_get(param->i2c_fd, 0x44);
	
	return 0;
}

int temp_read(sql_callbac_parm_t *param, int argc, char **argv, char **argv_names) {
	char ow_buf[12];
	int i;
	
	ds2482_reset(param->i2c_fd);
	
	ow_buf[0]=0x55;
	for(i=0; i<8; i++){
		ow_buf[i+1] = atoi(argv[i]);
	}
	ow_buf[9]=0xbe;
	ow_buf[10]=0xff;
	ow_buf[11]=0xff;
	
	for (i=0; i <12; i++) {
		ow_buf[i] = ds2482_send_and_get(param->i2c_fd, ow_buf[i]);
	}
	
	/// parsowanie temperatury
	if (
		(ow_buf[10]==0x50 && ow_buf[11]==0x05) || \
		(ow_buf[10]==0xf8 && ow_buf[11]==0x07) || \
		(ow_buf[10]==0xff && ow_buf[11]==0xff)
	) {
		LOG_PRINT_ERROR("ERROR in read temperature for %s %s", argv[8], argv[9]);
	} else {
		float val = (short)(ow_buf[11] << 8 | ow_buf[10]) / 16.0;
#ifdef ZABBIX_SERWER_IP
		zabbix_message_add_item_f(param->zabbix_msg, argv[8], argv[9], val);
#else
		printf("%s: %s: %f\n", argv[8], argv[9], val);
#endif
	}
	
	return 0;
}



int main(int argc, char **argv) {
	int i2c_fd, ret;
	char *ErrMsg = NULL;
	
	openlog("onewire_reader", LOG_PID, LOG_DAEMON);
	
	/// otwarcie i konfiguracja mosta i2c - 1wire
	i2c_fd = open_i2c_device(I2C_DEVICE, DS2482_ADDR);
	i2c_smbus_write_byte(i2c_fd, DS2482_RESET);
	i2c_smbus_write_byte_data(i2c_fd, DS2482_WRITE_CONFIG, 0xe1);
	
	/// open database
	sqlite3 *db;
	if ( sqlite3_open(DATABESE_FILE, &db) ) {
		LOG_PRINT_CRIT("Can't open database: %s", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(-1);
	}
	
	/// prepare zabbix message structure
	zabbix_msg_t message;
#ifdef ZABBIX_SERWER_IP
	struct sockaddr_in zabbix_ser;
	zabbix_ser.sin_family=AF_INET;
	zabbix_ser.sin_port=htons(ZABBIX_SERWER_PORT);
	zabbix_ser.sin_addr.s_addr=inet_addr(ZABBIX_SERWER_IP);
	
	message.zabbix_server = &zabbix_ser;
	message.max_size = ZABBIX_MESSAGE_BUF_SIZE;
	message.data = zabbix_message_buf;
#endif
	
	/// prepare struct for sqlite3_exec callback function parametrs
	sql_callbac_parm_t param;
	param.zabbix_msg = &message;
	param.i2c_fd = i2c_fd;
	
	while(1) {
#ifdef ZABBIX_SERWER_IP
		zabbix_message_reinit(&message);
#endif
		
		/// konwrsja temperatury
#ifdef USE_CONVERT_ALL
		temp_conv_all((void*)&param);
#else
		ret = sqlite3_exec(db, "SELECT A, B, C, D, E, F, G, H FROM ds18b20;",
		                   (int (*)(void *, int, char **, char **))temp_conv, (void*)&param, &ErrMsg);
		if ( ret != SQLITE_OK ) {
			LOG_PRINT_WARN("temp_conv SQL error: %s", ErrMsg);
			sqlite3_free(ErrMsg);
		}
#endif
		
		sleep(1);
		
		/// odczyt temperatury
		ret = sqlite3_exec(db, "SELECT A, B, C, D, E, F, G, H, host, item FROM ds18b20;",
		                   (int (*)(void *, int, char **, char **))temp_read, (void*)&param, &ErrMsg);
		if ( ret != SQLITE_OK ) {
			LOG_PRINT_WARN("temp_conv SQL error: %s", ErrMsg);
			sqlite3_free(ErrMsg);
		}
		
		// wysłanie danych do zabbix'a
#ifdef ZABBIX_SERWER_IP
		if ( zabbix_send(&message) < 0 )
			LOG_PRINT_ERROR("ERROR in zabbix_send");
#endif
		sleep(30);
	}
	
	sqlite3_close(db);
	closelog();
	return 0;
}
