#include <omp.h>
#include <stdio.h>
#include <unistd.h>

// kompilacja: gcc -fopenmp openmp.c -o openmp_demo
// uruchomienie (4 wątki): env OMP_NUM_THREADS=4 ./openmp_demo
int main (int argc, char *argv[]) {
	int ilosc_watkow = -22, numer_watka = -33;
	// od tego miejsca kod będzie zrównoleglany ...
	// default(none) wymusza jawne deklarowanie zasięgu każdej zmiennej
	// zmienna prywatna dla każdego z wątków będzie numer_watka
	// wspólną zmienną będzie ilosc_watkow
	#pragma omp parallel default(none) shared(ilosc_watkow) private(numer_watka)
	// ponadto w dyrektywie parallel możemy:
	// określić kiedy ma mieć miejsce zrównoleglenie - poprzez if (warnunek na jakis zmiennych)
	// ile wątków powinno obsługiwać urównoleglenie - poprzez num_threads(ilosc)
	{
		// poniższy blok wykona się tylko w pojedynczym wątku
		#pragma omp single
		{
			ilosc_watkow = omp_get_num_threads();
			printf("Urównolegliliśmy się na %d watków\n", ilosc_watkow);
		}
		
		// poniższy kod wykona każdy z wątków niezależnie
		numer_watka = omp_get_thread_num();
		printf("Tu wątek numer %d ... ja żyje :-)\n", numer_watka);
		
		// czekamy az wszystkie watki zakończą powyższy kod
		#pragma omp barrier
		if ( numer_watka == 0 ) { // tylko w wątku zerowym
			printf("Tu wątek numer zero ... informuje iż każdy wątek ma już ustawioną swoją zmienną numer_watka\n");
		}
		
		// możemy też w prostszy od pokazanego powyżej if'a przydzielać zadania poszczególnym wątkom:
		#pragma omp sections
		{
			#pragma omp section
			{
				// ten wątek sobie troszkę pośpi
				sleep(1);
				printf("Jakiś wątek AAAA\n");
			}
			#pragma omp section
			{
				printf("Jakiś inny wątek BBBB\n");
			}
		}
	}
	printf("No i nie ma już naszych %d wątków ... ", ilosc_watkow);
	printf("teraz mamy zwykły sekwencyjnie wykonywany program, a numerem wątka jest %d\n", numer_watka);
	// na powyższym widać różnicę między zmienną współdzieloną a prywatną dla wątków
	
	// możemy urównoleglać także pętlę for
	// możemy używać zmiennej do której pisać będzie kilka wątków
	// pod warunkiem że będzie to tablica a watki piszą do różnych komórek
	// lub że będzie to zmienna akumulacyjna (jak w poniższym przykładzie)
	// - musi być ona wtedy podana przez reduction(charakter:zmienna)
	int i, max_i=10, suma=0;
	#pragma omp parallel for default(none) shared(max_i) private(i) reduction(+ : suma)
	for (i = 0; i < max_i; i++) {
		suma += i * omp_get_thread_num();
	}
	printf("SUMA = %d\n", suma);
}
