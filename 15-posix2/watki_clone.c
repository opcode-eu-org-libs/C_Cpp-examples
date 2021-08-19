/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje tworzenie wątków (poprzez clone) i uruchamianie w nich funkcji ...
 *
 * więcej o wątkach: http://rainbow.mimuw.edu.pl/SO/Linux/Temat02/opis_watki.html
 *
 */

#define _GNU_SOURCE

#include <sched.h>
#include <unistd.h> /* for sleep */
#include <stdio.h>  /* for printf */
#include <stdlib.h> /* for malloc */

int globalna; // zmienna ta wykorzystana jest do zademonstrowania możliwości korzystania
// ze zmiennych globalnych do komunikacji między wątkami

int wypisz(void* napis) {
	usleep(500000);
	char i;
	globalna = 1;
	for(i=0; i<4; i++) {
		printf("To watek: %s\n", (char*)napis);
		sleep(1);
	}
	globalna = 0;
	return 0; // kończę działanie wątku
}

int main () {
	char i;
	
	globalna = -1;

	// jest to metoda rozgałęziania procesu (zbliżona do funkcji fork())
	// charakterystyczna dla Linuxa, w odróżnieniu od pthread oba watki będą
	// widoczne jako osobne procesy, można to zmienić podając flagi
	// CLONE_SIGHAND | CLONE_THREAD,
	// podana tutaj flaga CLONE_VM oznacza współdzielenie pamięci,
	// przy pomocy tych flag można jeszcze więcej ...
	// więcej o flagach można przeczytać w man 2 clone
	
	// jest także funkcja sys_clone która kontynuuje program
	// od miejsca wywołania a nie od zadanej funkcji
	
	void *stos;
	stos = (void*) malloc(32000);
	// na większości architektur stos rośnie w dół,
	// wiec podajemy koniec przydzielonego zakresu pamięci
	clone(wypisz, stos+32000-1, CLONE_VM, "Ala juz nie ma kota");
	
	for(i=0; i<7; i++) {
		printf("To program główny (%d)\n", globalna);
		sleep(1);
	}
	return 0;
} 
