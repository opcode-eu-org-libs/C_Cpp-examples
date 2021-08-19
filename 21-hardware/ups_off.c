// program przeznaczony do wywoływania ze skryptu reboot (na runlevel 6) ...
// program sprawdza stan zasilania ups'a Fideltronic Ares 1000
// jeżeli system działa na zasilaniu z UPS (praca awaryjna)
// nastąpi wyłączenie UPSa oraz wykonanie polecenia halt,
// w przeciwnym razie program zakończy się ... pozwalając na dokonanie reboot'u

// gdy program wywołany nie jest w runlevelu 6 poinformuje o stanie zasilania i zakończy działanie

#include <stdio.h>
#include <sys/io.h>
#include <unistd.h>

#define RS_PORT 0x3f8

int main(int argv, char *argc[]) {
	// zmieniamy poziom uprzywilejowania I/O ...
	// potrzebne do funkcji inb / outb czytających / ustawiających stan portu
	// wymaga praw root'a
	iopl(3);
	
	// ustawiamy DTR na stan wysoki
	outb(1, RS_PORT + 4);
	
	// odczytujemy CTS - informacja o stanie zasilania
	if ( inb(RS_PORT + 6) & (1 <<4) ) {
		puts("Normalny stan zasilania - nic nie robię");
		return 0;
	} else {
		puts("Awaryjny stan zasilania");
		// jeżeli runlvel !=6 nie kontynuuję ...
		if ( 0 != system("test `runlevel | cut -f2 -d' '` = 6")) {
			puts("Runlevel != 6 => kończę");
			return 2;
		} else {
			puts("Czekam na powrót zasilania ...");
			sleep(5);
			// ponownie sprawdzam stan zasilania
			if ( ! ( inb(RS_PORT + 6) & (1 <<4) ) ) {
				puts("Nadal awaryjny stan zasilania => wyłączam");
				
				// ustawiamy stan wysoki na RTS - wyłączenie UPS za 5s
				outb(2, RS_PORT + 4);
				
				// wykonanie halt
				system("halt -d -f -i");
				return 3;
			} else {
				puts("Powrót do normalnego stanu zasilania ...");
				return 1;
			}
		}
	}
}
