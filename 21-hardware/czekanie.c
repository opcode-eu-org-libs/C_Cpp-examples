/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody wykonywania precyzyjnych przerw w programach
 *
 * uwaga: aby uzyskać naprawdę dokładne przerwy należy prezentowane tu metody wyczekiwania
 * łączyć z metodami powodującymi uprzywilejowanie naszego procesu w dostępie do procesora
 * opisanymi w zawieszacz.c i priorytety.c
 *
 */

#include <stdio.h>

#define CPU_SPEED 3000 // taktowanie CPU w MHz

// funkcja zwracająca wartość rejestru zliczającego takty procesora
unsigned long long czas() {
  asm( "rdtsc" );
}

struct timeval tv_1;
unsigned long long czas2() {
	gettimeofday ( &tv_1, NULL );
	return (unsigned long long)( tv_1.tv_sec * 1000000 + tv_1.tv_usec );
		// zmieści się to nam gdyż 10^6 * 2^32 < 2^64
}

int main (int argv, char *args[]) {
	int ile = atoi(argv[1]); // czas czekania w us
	
	// czekanie aktywne w oparciu o licznik taktów i pomiar czasu czekania ...
	// czekanie to może działać nawet z precyzja kilku nanosekund
	// - zależy to od prędkości taktowania procesora ...
	struct timeval tv_1, tv_2;
	gettimeofday ( &tv_1, NULL );
	
	// czekamy
	tmp1 = czas() + CPU_SPEED*ile;
	while (czas() < tmp1);
	
	gettimeofday ( &tv_2, NULL );
	printf("CZAS [us]: %li\n",
		(tv_2.tv_sec * 1000000 + tv_2.tv_usec) - (tv_1.tv_sec * 1000000 + tv_1.tv_usec) );
	
	// jak widzimy pomiar ten jest dość dokładny (jest systematyczne zawyżenie
	// ale wiąże się ono z czasem wykonywania dodatkowych instrukcji ...
	// dla czasów oczekiwań > 4us można zatem zrealizować z wykorzystaniem gettimeofday
	// należy wtedy korzystać z czas2() zamiast czas() oraz nie podawać mnożnika CPU_SPEED
	// który jest związany z prędkością procesora (w przykładzie 3GHz)
	// mnożnik ten można obliczyć z wykorzystaniem połączenia obu metod
	
	// dla zapewnienia precyzji czekanie (czyli pętla `while (czas() < tmp1)`)
	// powinno odbywać się z wyłączonymi przerwaniami - patrz zawieszacz.c
	
	// na koniec trzeba tez wspomnieć o nanosleep(), która zgodnie z dokumentacja man powinna
	// dla czasów <2ms i polityce szeregowania czasu rzeczywistego używać aktywnego czekania
	
	return 0;
}
