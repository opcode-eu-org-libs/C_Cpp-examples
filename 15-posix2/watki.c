/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje tworzenie wątków i uruchamianie w nich funkcji ...
 *
 * więcej o wątkach: http://rainbow.mimuw.edu.pl/SO/Linux/Temat02/opis_watki.html
 *
 * kompilacja: gcc -lpthread -o watki watki.c
 *
 */

#define _GNU_SOURCE
// to potrzebne aby flagi zachowywały się  zgodnie z POSIX

#include <pthread.h>
#include <sched.h> 
#include <unistd.h> /* for sleep */
#include <stdio.h>  /* fo printf */
#include <stdlib.h> /* for malloc */
#include <errno.h>  /* for ENOTSUP */

int globalna; // zmienna ta wykorzystana jest do zademonstowania możliwosci korzystania
// ze zmiennych globalnych do komunikacji między wątkami
// uwaga: zmienne globalne są alokowane w obszarze pamięci procesu a nie na stosie
//        jak ma to miejsce ze zmiennymi lokalnymi

int wypisz(char * napis) {
	char i;
	globalna = 1;
	for(i=0; i<4; i++) {
		printf("To watek: %s\n", napis);
		sleep(1);
}
	globalna = 0;
	sleep(5);
	pthread_exit(0); // kończy działanie wątku
}

void wypisz2(char * napis) {
	printf("ten wątek umiera ... %s\n", napis);
}

int wypisz3(char * napis) {
	int i;
	printf("To watek: %s\n", napis);
	
	// zabraniamy przerwania wątku w trakcie poniższego wypisywania
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	for(i=0; i<10; i++) {
		printf(".", napis);
		fflush(stdout);
		sleep(1);
	}
	printf("\n", napis);
	// a teraz już może być anulowany
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	
	// ale od teraz tylko w wyznaczonych momentach
	// na funkcjach takich jak sleep, system, ...
	// czy tez pthread_testcancel()
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	
	sleep(50);
}

pthread_mutex_t mut;
pthread_cond_t war;
int licz;

int wypisz4(char * napis) {
	int i;
	printf("To watek: %s\n", napis);
	
	sleep(2);
	
	// blokujemy wejście na ten kawałek kodu
	if (pthread_mutex_trylock(&mut)) {
		printf("nie mogłem zrobić lock więc czekam do skutku\n");
		pthread_mutex_lock(&mut);
}
	
	printf("jestem w sekcji krytycznej - przygotowuje dane\n");
	sleep(5);
	licz++;
	
	// zezwalam na odbiór
	if (licz>0) pthread_cond_signal(&war);
	pthread_mutex_unlock(&mut);
}


int main () {
	int i;
	
	// tworzymy nowy watek uruchamiając w nim funkcje wypisz
	// i przekazując jako pierwszy argument tej funkcji "Ala ma kota"
	// - to generuje ostrzeżenie ale działa ...
	globalna = -1;
	
	pthread_t watek1;
	pthread_attr_t atrybuty;
	pthread_create( &watek1, 0, (void* (*)(void *))wypisz, "Ala ma kota" );
	
	// pobranie atrybutu wątku i sprawdzenie czy jest wątkiem procesu czy systemu
	pthread_getattr_np(watek1, &atrybuty);
	pthread_attr_getscope(&atrybuty, &i);
	printf("PTHREAD_SCOPE: %d %d %d\n", i, PTHREAD_SCOPE_SYSTEM, PTHREAD_SCOPE_PROCESS);
	// jeżeli typ == PTHREAD_SCOPE_SYSTEM to watek jest systemowy czyli będzie widoczny w ps -m
	system("ps -m");
	
	for(i=0; i<7; i++) {
		printf("To program główny (%d)\n", globalna);
		sleep(1);
}
	
	// możemy oczekiwać na zakończenie wątku poprzez wywołanie w innym funkcji pthread_join ,
	// umożliwia ona także uzyskanie kodu z jakim zakończył się wątek
	printf("Program główny czeka na koniec wątku\n");
	pthread_join(watek1, NULL);
	printf("Wątek zakończył się\n");
	// jej wywołanie jest konieczne aby wątek po zakończeniu był w całości usunięty
	// chyba ze został określony jako taki do którego nie można się dołączyć
	// przez wywołanie pthread_detach() lub podanie przy tworzeniu flagi PTHREAD_CREATE_DETACHED
	
	// przygotowujemy atrybuty dla wątku
	if (pthread_attr_init(&atrybuty))
		perror("pthread_attr_init");
	// ustawiamy że ma być to wątek procesu
	i = pthread_attr_setscope(&atrybuty, PTHREAD_SCOPE_PROCESS);
	if (i != 0) {
		fprintf(stderr, "pthread_attr_setscope ERROR %d\n", i);
		if (i == 95) // ENOTSUP
			fprintf(stderr, "  wątki procesu nie sa wspierane\n");
	}
	// możemy ustawiać takie rzeczy jak to czy wątek jest typu odłączonego
	// (czy będziemy mogli czekać na niego przy pomocy join czy nie) - pthread_attr_setdetachstate
	// parametry schedulera, adres stosu wątku, itd - więcej info w man 3 pthread_attr_init
	
	// określamy funkcje wykonywana w momencie śmierci watka
	pthread_cleanup_push((void (*)(void *))wypisz2, "byłem wypisz3");
	
		// tworzymy inny watek
	pthread_create(&watek1, &atrybuty, (void* (*)(void *))wypisz3, "do anulowania");
	
	pthread_getattr_np(watek1, &atrybuty);
	pthread_attr_getscope(&atrybuty, &i);
	printf("PTHREAD_SCOPE: %d %d %d\n", i, PTHREAD_SCOPE_SYSTEM, PTHREAD_SCOPE_PROCESS);
	system("ps -m");
		
	sleep(5);
	puts("postanowilem zabic ten watek");
		
		// i go kończymy z zewnatrz
		// w zależności od ustawień wątku anulowanie wątku dzieje się natychmiastowo
		// albo w jednym z punktów anulowania - zobacz funkcje to ustawiające w wypisz3()
	pthread_cancel(watek1);
		// wypada też odebrać jego kod powrotu
	pthread_join(watek1, NULL);
	puts("a teraz watek juz niezyje, ale może jeszcze po sobie sprzątać ...");
	
	// odefiniowujemy funkcje wykonywana w momencie śmierci watka
	pthread_cleanup_pop(1);
	
	// biblioteka obsługująca wątki zapewnia także mechanizm zapewniający pojedyncze wykonanie
	// jakiejś czynności (tylko jeden z wątków (pierwszy) przetwarzających wspólny kod
	// wykona ta czynność - zobacz man pthread_once
	//
	// a także implementuje funkcje umożliwiające wątkom korzystanie z semaforów oraz sygnałów - zobacz
	// man pthread_mutex_init , man pthread_sigmask , man pthread_cond_init
	// możemy korzystać także z semaforów biblioteki standardowej C - zobacz man 3 sem_init
	
	pthread_mutexattr_t atrybuty_mutexu;
	pthread_mutexattr_settype(&atrybuty_mutexu, PTHREAD_MUTEX_ERRORCHECK);
	// możemy ustawić PTHREAD_MUTEX_ERRORCHECK aby mechanizm reagował błędem na unlockowanie
	// zablokowanego przez inny wątek mutexu itp, PTHREAD_MUTEX_RECURSIVE aby blokada nałożona
	// przez watek A nie obowiazywala wątku A (umożliwia to korzystanie z mutexów w funkcjach
	// rekurencyjnych), albo PTHREAD_MUTEX_NORMAL lub PTHREAD_MUTEX_DEFAULT ...
	// flagi te mogą się nieco różnić w zależności od stosowanej biblioteki i użytych #define
	// szczegółów należy szukać w plikach features.h pthread.h oraz dokumentacji man
	
	pthread_mutexattr_setprotocol(&atrybuty_mutexu, PTHREAD_PRIO_NONE);
	// poza nic nie zmieniającym PTHREAD_PRIO_NONE mamy także PTHREAD_PRIO_INHERIT zabezpieczający
	// wątki przed inwersją priorytetów (priorytet po wejściu do sekcji krytycznej jest podwyższany
	// do najwyższego z priorytetów procesów czekających oraz PTHREAD_PRIO_PROTECT zabezpieczający
	// również przed zakleszczeniem dzięki podnoszeniu tego priorytetu powyżej najwyższego z
	// priorytetów procesów czekających
	// do zapobiegania inwersji priorytetów przydatne jest też: pthread_mutexattr_setprioceiling();
	
	// inicjalizujemy mutex
	// dla domyślnych ustawień można podać NULL jako drugi argument
	pthread_mutex_init(&mut, &atrybuty_mutexu);
	
	pthread_create(&watek1, &atrybuty, (void* (*)(void *))wypisz4, "z mutexem");
	
	// blokujemy mutex
	pthread_mutex_lock(&mut);
	
	printf("jestem w sekcji krytycznej - czekam na dane\n");
	
	// czekamy ...
	while (licz == 0) pthread_cond_wait(&war, &mut);
	// pthread_cond_wait zwalnia tak na prawdę na chwile podany do niego mutex a po odebraniu 
	// pthread_cond_signal na dozorowanym przez niego warunku ponownie go zamyka
	
	printf("jestem w sekcji krytycznej - odbieram dane\n");
	sleep(5);
	
	licz--;
	// odblokowujemy mutex
	pthread_mutex_unlock(&mut);
	
	// niszczymy mutex
	pthread_mutex_destroy(&mut);
	
	
	// wiecej w man 7 pthreads
	return 0;
}
