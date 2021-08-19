/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje użyteczne funkcje charakterystyczne dla systemów zgodnych  z Unix'em
 *
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

// funkcje obsługi sygnału
void sig_usr(int sig) {
	printf("Otrzymałem sygnał numer: %i\n", sig);
}

void akcja_sygnalu(int sig, siginfo_t *info, void *context) {
	printf("Otrzymałem sygnał numer: %i z kodem %d i wartością %d\n",
		sig, info->si_code, info->si_value.sival_int);
	// si_code określa powód wysłania sygnału
}

int main() {
	printf("Mój PID to %d\n", getpid());
	
	
/** OBSŁUGA SYGNAŁÓW **/
	
	// do zdefiniowania obsługi sygnału - jego przechwycenia służy funkcja
	//signal( numer_sygnału, sposób_obsługi );
	// użytkownik ma do dyspozycji dwa nie zdefiniowane przez system sygnały: SIGUSR1, SIGUSR2
	// może też zmieniać obsługę innych (poza nieprzechwytywalnymi) ...
	
	// aby wysłać sygnał do samego siebie korzystamy z raise( sygnał );
	// co jest równoważne kill( getpid(), sygnał );
	// parę słów o sygnałach zawarłem też w fork.c
	
	// ustawiam przechwycenie sygnału SIGUSR1 (pełną ich listę wypisuje kill -l)
	// lista z opisami dostępna na http://en.wikipedia.org/wiki/Signal_(computing)#List_of_signals
	signal(SIGUSR1, &sig_usr);
	signal(SIGUSR2, &sig_usr);
	// zamiast wskaźnika do funkcji możemy podać także SIG_IGN albo SIG_DFL
	// co oznacza odpowiednio ignorowanie tego sygnału lub jego domyślną obsługę
	
	
	// możliwe jest też maskowanie sygnałów
	sigset_t maska_sygnalow;
	if (sigemptyset(&maska_sygnalow)) // inicjalizujemy na pustą, na pełną byłoby sigfillset()
		puts("sigemptyset error");
	if (sigaddset(&maska_sygnalow, SIGINT)) // dodajemy sygnał do maski
		puts("sigaddset error");
	// można także usuwać - sigdelset() i sprawdzać czy jest obecny - sigismember()
	if (sigprocmask(SIG_SETMASK, &maska_sygnalow, NULL))
		puts("sigprocmask error");
	// w skutek tego program nie będzie reagował na Ctrl+C
	//
	// oprócz tego są funkcje API sygnałowgo BSD:
	//  sigvec, sigblock, sigsetmask, siggetmask, sigmask
	
	// dostępny jest także mechanizm kolejkowanej obsługi sygnałów
	// w którym także kolejne sygnały tego samego typu w oczekiwaniu
	// na odebranie gromadzone są w kolejce
	struct sigaction akcja_obslugi_sygnalu;
	akcja_obslugi_sygnalu.sa_flags = SA_SIGINFO;
	sigfillset(&akcja_obslugi_sygnalu.sa_mask);
	akcja_obslugi_sygnalu.sa_sigaction = &akcja_sygnalu;
	sigaction(SIGALRM, &akcja_obslugi_sygnalu, NULL);
	
	sigqueue(getpid(), SIGALRM, (const union sigval)123); 
	
	
	// możliwe jest także oczekiwanie na dowolny sygnał:
	puts("czekam na sygnal");
	pause();
	
	// lub na sygnał z zestawu określonego w taki sposób jak dla 
	// poprze sigwait(&zestaw, &sygnal);
	sigemptyset(&maska_sygnalow);
	sigaddset(&maska_sygnalow, SIGQUIT);
	int signum;
	puts("czekam na sygnal SIGQUIT");
	sigwait(&maska_sygnalow, &signum);
	if (signum == SIGQUIT)
		puts("dostalem SIGQUIT");
	
	// możemy sobie zażyczyć także przesłania sygnału SIGALRM
	// za określoną ilość sekund (lub trochę później):
	alarm(4);
	
	
/** DESKRYPTORY PLIKÓW **/
	
	// funkcja open służy do otwierania dostępu do pliku przez deskryptor, patrz: man 2 open
	int deskryptor = open( "/tmp/plik", O_WRONLY );
	
	write(deskryptor, "Ala ma Kota", 12 ); // wpisuje 12 znaków z podanego ciągu
	// do czegoś określonego deskryptorem (w tym przykładzie jest to plik)
	
	// mamy też bliźniaczą funkcję read(deskryptor, bufor, ilosc) czytająca z deskryptora
	// do bufora zadana ilość znaków oraz funkcję lseek(deskryptor, przesuniecie, tryb)
	// - przesuwającą się w pliku; tryb to SEEK_SET (nowa pozycja = przesuniecie),
	// SEEK_CUR (nowa pozycja = aktualna pozycja + przesuniecie),
	// SEEK_END (nowa pozycja = rozmiar_pliku + przesuniecie)
	
	close(deskryptor); // zamykamy deskryptor
	
	
	system("rm /tmp/fifo1 /tmp/fifo2; mkfifo /tmp/fifo1 /tmp/fifo2"); // funkcja system wykonuje podane w niej polecenie shellowe ... więcej w fork.c
	// w tym wypadku utworzy łącza nazwane /tmp/fifo1 i /tmp/fifo2
	// możnaby zrobić to też funkcją mkfifo bezpośrednio z poziomu C
	
	
/** OCZEKIWANIE NA DANE **/
	
	int d1 = open( "/tmp/fifo1", O_RDWR );
	int d2 = open( "/tmp/fifo2", O_RDWR );
	int maxfd = (d1 > d2) ? d1 : d2;
	
	printf("Oczekiwanie na dane w /tmp/fifo1 oraz /tmp/fifo2\n");
	printf("Możesz użyć `echo 12345678901234567890 > /tmp/fifo1` aby wprowadzić jakieś dane\n");
	fflush(stdout); // wymuszamy wypisanie bufora stdout na ekran ... jest to niekiedy potrzebne gdy chcemy żeby komunikat pojawił się bez zbędnej zwłoki
	
	for (int i=0; i<4; ++i) { // normalnie byłaby tu nieskończona pętla (przerywana tylko zależnie od wczytanych danych) ... ale to jest demo i chcemy żeby kiedyś się skończyło
		fd_set oczekujaceNaOdczyt;
		FD_ZERO(&oczekujaceNaOdczyt);
		FD_SET(d1, &oczekujaceNaOdczyt);
		FD_SET(d2, &oczekujaceNaOdczyt);
		
		struct timeval timeout; // limit czasu oczekiwania na dane:
		timeout.tv_sec  = 4;    //  sekundy
		timeout.tv_usec = 0;    //  mikrosekundy
		
		int ret = select(maxfd + 1, &oczekujaceNaOdczyt, 0, 0, &timeout); // czekamy na dane w deskryptorach gniazd
		
		if (ret == -1) {
			printf("select: %m, niezużyta wartość timeoutu: %d %d\n", timeout.tv_sec, timeout.tv_usec);
		} else if (ret == 0) {
			printf("no data - timeout %d\n", i);
		} else {
			char bufor[10]; // używamy małego bufor aby łatwo było pokazać wczytywanie w kawałkach
			if (FD_ISSET (d1, &oczekujaceNaOdczyt)) {
				printf("nowe dane w /tmp/fifo1\n");
				int ileOdczytano = read(d1, bufor, 9);
				if (ileOdczytano) {
					bufor[ileOdczytano] = 0; // konwersja na NULL-end string (przekazanie mniejszej wartości do read, niż rzeczywisty rozmiar bufora pozwala to zrobić bezpiecznie)
					printf("* %s\n", bufor);
				}
			}
			if (FD_ISSET (d2, &oczekujaceNaOdczyt)) {
				printf("nowe dane w /tmp/fifo2\n");
				int ileOdczytano = read(d2, bufor, 9);
				if (ileOdczytano) {
					bufor[ileOdczytano] = 0; // konwersja na NULL-end string (przekazanie mniejszej wartości do read, niż rzeczywisty rozmiar bufora pozwala to zrobić bezpiecznie)
					printf("* %s\n", bufor);
				}
			}
			i=0; /// pomimo że to demo to nie chcemy żeby skończyło się przedwcześnie
		}
		fflush(stdout);
	}
	printf("Koniec oczekiwania na dane\n");
	
	// funkcja select jest szczególnie przydatna przy oczekiwaniu na dane wprowadzane przez użytkownika na standardowym wejściu lub odbierane z połączenia sieciowego
	// inną funkcją oferującą takie czekanie jest poll (w odróznieniu od select obsługuje ona deskryptory o numerach większych niż FD_SETSIZE (1024),
	// co może być przydatne jeżeli mamy dużo otwartych plików (lub podobnych obiektów) w naszym programie)
	
	
	while(sleep(60) > 0); // aby móc pobawić się sygnałami ...
		// while konieczny bo sygnal przerywa czekanie i nie jest ono kontynuowane
		// należy też wspomnieć iż ładniejszą metodą realizacji takiego zabronienia
		// zakończenia programu jest czekanie na jakiś sygnał niż robienie sleep()
		// ale tu nam chodzi aby ten sleep się kiedyś zakończył ...
	
		// sleep(ilosc_sekund); - proces robi sobie przerwę przez zadaną liczbę sekund lub dłużej 
		//  (nie zajmuje czasu procesora w odróżnieniu od rozwiązań typu pętle itp.,
		//  otrzymanie sygnału skraca przerwę)
}
