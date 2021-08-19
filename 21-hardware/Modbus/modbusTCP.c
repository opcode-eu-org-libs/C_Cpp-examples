/**
 * Plik prezentuje prosty przykład użycia libmodbus jako mastera Modbus TCP.
 * Biblioteka ta może być także używana w trybie RTU.
 * 
 * Kompilacja: gcc -lmodbus modbusTCP.c
 */

#include <modbus/modbus-tcp.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define HEX     0
#define INT16   1
#define SINT16  2
#define BITMASK 3
#define SINT32  10
#define INT32   11
#define FLOAT   12
#define INT32L  13
#define INT64   14

/**
 * inicjalizuje libmodbus jako master TCP
 */
modbus_t* begin(const char* host, const char* port, int addr) {
	modbus_t* mb = modbus_new_tcp_pi(host, port);
	if (mb == NULL) {
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		exit(1);
	}

	if (addr > 0)
		modbus_set_slave(mb, addr);

	#ifdef DEBUG
	modbus_set_debug(mb, 1);
	#endif

	if (modbus_connect(mb) == -1) {
		fprintf(stderr, "Connection failed for %s: %s\n", host, modbus_strerror(errno));
		modbus_free(mb);
		exit(1);
	}

	return mb;
}

/**
 * pomocnicza funkcja wypisująca rejest danego typu
 */
void printRaw(int type, uint16_t *data, int *dataIdx, int start) {
	int i = *dataIdx;
	int j;
	switch(type) {
		case INT16:
			printf("%d  %f\n", start + i, (float)(data[i]));
			*dataIdx = i + 1;
			break;
		case SINT16:
			printf("%d  %f\n", start + i, (int16_t)((data[i])));
			*dataIdx = i + 1;
			break;
		case INT32:
			printf("%d  %f\n", start + i, (float)(MODBUS_GET_INT32_FROM_INT16(data, i)));
			*dataIdx = i + 2;
			break;
		case BITMASK:
			for (j=0; j<16; ++j)
				printf("%d [%d]  %d\n", start + i, j, (data[i] & (1<<j)) != 0);
			*dataIdx = i + 1;
			break;
		case FLOAT:
			printf("%d  %f\n", start + i, modbus_get_float_dcba(data+i));
			*dataIdx = i + 2;
			break;
		case HEX:
		default:
			printf("%d  0x%04x\n", start + i, data[i]);
			*dataIdx = i + 1;
			break;
	}
}

/**
 * odczytuje i wypisuje zestaw rejestrów modbusowych pojedynczego typu
 *
 * @param start   numer pierwszego rejestru do odczytania
 * @param count   ilość rejestrów do odczytania
 *                  jeżeli pojedyncza wypisywana wartość budowana jest na podstawie kilu rejestrów (np. typ FLOAT) to większy od liczby wypisanych wartości
 *                  jeżeli pojedyncza wypisywana wartość budowana jest na podstawie fragmentu rejestr (np. typ BITMASK) to mniejszy od liczby wypisanych wartości
 * @param type    sposób interpretacji wartości w rejestrach modbus
 * @param tab_reg pozwala na zachowanie danych z modbus w zewnętrznym buforze, podanie NULL powoduje użycie wewnętrznego bufora
 */
int read(modbus_t *mb, int start, int count, int type, uint16_t *tab_reg) {
	uint16_t buff[128];
	int i=0;
	
	if (tab_reg == NULL) tab_reg = buff;
	
	if (modbus_read_registers(mb, start, count, tab_reg) == -1) {
		fprintf(stderr, "Read failed: %s\n", modbus_strerror(errno));
		return -1;
	}
	
	while (i<count) {
		printRaw(type, tab_reg, &i, start);
	}
	
	return -2;
}


/**
 * deinicjalizuje libmodbus
 */
void end(modbus_t *mb) {
	modbus_close(mb);
	modbus_free(mb);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("USAGE: %s IPaddress\n", argv[0]);
		exit(2);
	}
	
	modbus_t* mb = begin(argv[1], "502", -1);
	
	read(mb, 149, 16, INT16, NULL);
	
	end(mb);
}
