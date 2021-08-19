/**
 * Plik stanowi zestaw funkcji do obsługi interfejsu 1-wire
 * do wykorzystania zarówno w mikrokontrolerach jak i sprzęcie PC
 * konieczne jest wstawienie odpowiedniej wersji definiowania funkcji:
 *   onewire_send_zero() onewire_send_jeden() onewire_read() - patrz poniżej
 *
 * a także włączenie "util/delay.h"
 * lub odpowiednika (np. dla PC - patrz onewire_get_rom.c)
**/

/* PRZYKŁADY DEFINICJI FUNKCJI ZALEŻNYCH OD IMPLEMENTACJI:
1) dla portu LPT w komputerze PC:
	#define onewire_send_zero() outb(0x00, 0x37a)
	#define onewire_send_jeden() outb(0x04, 0x37a)
	#define onewire_read() (inb(0x37a) & 0x04) >> 2

2) dla mikrokontrolera ATmega8 z pojedynczym interfejsem na PB6
	#define onewire_send_zero() DDRB |= 0x40; PORTB &= 0xbf
	#define onewire_send_jeden() PORTB |= 0x40; DDRB &= 0xbf
	#define onewire_read() (PINB & 0x40) >> 6

3) dla mikrokontrolera ATmega8 z 8-mioma interfejsami na PORTB
	#define MULTI_BIT 6
	#define onewire_send(byte) DDRB = ~byte; PORTB = byte
	#define onewire_read()     PINB
*/

#include <stdint.h>

#ifdef MULTI_BIT
#	define onewire_send_zero()  onewire_send(0x00)
#	define onewire_send_jeden() onewire_send(0xff)
#endif

/// funkcja resetująca szynę onewire
void onewire_reset() {
	cli();
	
	onewire_send_zero();
	_delay_ms( 0.5 ); // 500 us
	onewire_send_jeden();
	sti();
	_delay_ms( 10 );
}

/// funkcja przesyłająca pojedynczy bit poprzez onewire
///  dla trybu jednoliniowego wysyła bit 1 gdy b!=0
///  dla trybu wieloliniowego wysyła na kolejnych liniach kolejne bity b
uint8_t onewire_bit(uint8_t b) {
	cli();
	
	onewire_send_zero();
#ifdef MULTI_BIT
	onewire_onewire_send(b);
#else
	if (b) onewire_send_jeden();
#endif
	_delay_us( 20 );
	b = onewire_read();
	_delay_us( 50 );
	onewire_send_jeden();
	
	sti();
	return b;
}

/// funkcja przesyłająca pojedynczy bajt poprzez onewire
///  dla trybu jednoliniowego zwraca otrzymany bajt odpowiedzi
uint8_t onewire_byte(uint8_t b_in) {
#ifdef MULTI_BIT
	uint8_t i, j, tmp;
	for (j=0; j<8; j++) {
		// powielenie bierzącego bitu na cały bajt
		if (b_in & (1<<j))
			tmp=0xff;
		else
			tmp=0x00;
		
		// obsługa interfeju
		onewire_bit(tmp);
	}
#else
	uint8_t c;
	for (c=8; c; c--)
		b_in = ( onewire_bit( b_in & 0x01 ) << 7 ) | ( b_in >> 1 );
		// w najstarszy bit zapisuje zczytana wartość
		// wykonuje binarne lub z
		// poprzednia wartością b przesunięta o 1 w prawo
#endif
	return b_in;
}

/// funkcja przesyła bajt w trybie liniowym,
/// ale wartości dla poszczególnych bitów brane są z osobnych komórek
/// tabeli a nie poprzez powielenie bitu pojedynczego bajtu
/// jak ma to miejsce w onewire_byte()
#ifdef MULTI_BIT
void onewire_byte_array(const uint8_t *b_in) {
	uint8_t i, j, tmp1, tmp2;
	for (j=0; j<8; j++) {
		// konstrukcja bajtu (tmp1) złożonego z aktualnego bitu (j <=> tmp2) każdej linii
		tmp1=0x00;
		tmp2=(1<<j);
		for (i=0; i<MULTI_BIT; i++) {
			tmp1 |= (b_in[i] & tmp2) << i;
		}
		
		// obsługa interfeju
		tmp1 = onewire_bit(tmp1);
	}
}
#endif

/// funkcja wysyłając same jedynki odbiera odpowiedz z onewire_get_rom
///  dla trybu jednoliniowego jest to alias do onewire_byte(0xff)
///    i wynik zwracany jest przez return
///  dla trybu wieloliniowego wynik zwracany jest w podanej jako argument
///    tablicy o wielkości określonej przez MULTI_BIT
#ifdef MULTI_BIT
void onewire_reply_array(uint8_t *b_out) {
	uint8_t i, j, tmp;
	for (j=0; j<8; j++) {
		// obsługa interfejsu
		tmp = onewire_bit(0xff);
		
		// analiza otrzymanej odpowiedzi
		// (podział bajtu na bity do osobnych komórek tabeli)
		for (i=0; i<MULTI_BIT; i++) {
			b_out[i] |= (tmp & (1<<i)) << j;
		}
	}
}
#else
#	define onewire_reply() onewire_byte(0xff)
#endif

/// funkcja przesyła komendę onewire
void onewire_command(uint8_t command, const uint8_t *adres) {
	onewire_reset();
	
	if (adres == NULL) {
		onewire_byte( 0xcc ); // skip room
	} else {
		char c;
		onewire_byte( 0x55 ); // match rom
		for (c=8; c; c--) {
			onewire_byte(*adres);
			adres++;
		}
	}
	onewire_byte(command);
}

#ifdef MULTI_BIT
/// funkcja przesyła komendę poprzez wieloliniowy onewire, gdzie
/// na poszczególnych liniach wystawiane są niezależne adresy urządzeń
/// adresy podawane są jako tablica 8*MULTI_BIT w postaci:
/// adr0[7], ..., adr0[0], adr1[7], ..., adr1[0], ...
void onewire_command_array(uint8_t command, const uint8_t *adres) {
	uint8_t i, j, addr_tmp[8];
	onewire_reset();
	
	onewire_byte( 0x55 ); // match rom
	for (i=0; i<8; i++) {
		for (j=0; j<MULTI_BIT; j++) {
			addr_tmp[j]= adres[j*8+i];
		}
		onewire_byte_array(addr_tmp);
	}
	
	onewire_byte(command);
}
#endif
