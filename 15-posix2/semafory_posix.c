/**
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje korzystanie z pamięci współdzielonej SHM oraz semaforów POSIXowych
 *
 * kompilacja gcc -lrt -lpthread semafory_posix.c
**/

#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>

#include <semaphore.h>
#include <time.h>

#include <mqueue.h>

#include <signal.h>

#include <aio.h>

#include <errno.h>
#include <stdio.h>

void sig_usr(int sig) {}

int main(int argc, char *argv[]) {
	
/** PAMIĘĆ WSPÓŁDZIELONA **/
	// z użyciem shm i mmap (więcej `man 7 shm_overview`)
	// można także przez mmap i zwykły plik - patrz alokacja_pamieci.c
	int fd=shm_open("moja_pamiec", O_RDWR|O_CREAT, 0770);
	int size=ftruncate(fd, sizeof(int));
	void *addr=mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	int *licznik = (int*) addr;
	// jeżeli więcej jakiś danych to najlepiej w tablicy lub w strukturze
	// no chyba że ktoś lubi liczyć ręcznie offsety ...
	
	printf("Licznik (1): %d\n", (*licznik)++);
	printf("Licznik (2): %d\n", (*licznik)++);
	// tak na prawdę to należałoby operacje na tym liczniku pozabezpieczać semaforami ...
	
	sleep(5);
	
	munmap(addr,sizeof(int));
	close(fd);
	shm_unlink("moja_pamiec");
	
	
/** SEMAFORY POSIX **/
	// więcej `man 7 sem_overview`
	sem_t *semafor;
	
	// otwieramy semafor nazwany
	// zamiast tego możemy po prostu użyć struktury typy sem_t
	// w takim wypadku przed pierwszym użyciem musielibyśmy wykonać na niej sem_init
	// a po zakończeniu używania przez ostatni proces sem_unlink
	semafor = sem_open("/moj_semafor", O_CREAT, 0770, 1);
	// flaga O_CREAT powoduje ze semafor zostanie utworzony gdy nie istnieje
	// oraz wymusza podanie dwóch dodatkowych argumentow funkcji ...
	// dodanie O_EXCL w flagach spowodowałoby ze funkcja zwróci błąd
	// gdy semafor o takiej nazwie istnieje
	//
	// eleganckim wydaje się używanie
	// sem_open("moj_semaforek", O_CREAT|O_EXCL, ...); w procesie głównym
	//  wraz z ew. obsługa tworzenia innej nazwy gdy dostaniemy EEXIST
	// a w "klientach" sem_open("moj_semaforek", 0);
	
	if (semafor == SEM_FAILED) {
		perror("sem_open");
		return -1;
	}
	
	
	if (sem_trywait(semafor)) {
		puts("Nie udało się opuścić semafora");
		perror("sem_trywait");
		
		struct timespec timeout;
		clock_gettime(CLOCK_REALTIME, &timeout);
		// funkcja o działaniu zbliżonym do gettimeofday ..
		
		// zwiększamy aktualny czas o 5s
		timeout.tv_sec+=5;
		
		if (sem_timedwait(semafor, &timeout)) {
			perror("sem_timedwait");
			// zamykamy semafor - nie będziemy z niego korzystać
			sem_close(semafor);
			goto kolejki;
		}
	}
	// jest tez oczywiście sem_wait która czeka do skutku ...
	
	// śpimy, to nieładnie spać w sekcji krytycznej, ale to tylko demo ...
	puts("jesteśmy w sekcji krytycznej");
	sleep(10);
	
	// podnosimy semafor
	sem_post(semafor);
	
	puts("wyszliśmy z sekcji krytycznej");
	
	// usuwamy semafor nazwany, ma to wpływ na wszystkie procesy z niego korzystające
	sem_unlink("/moj_semafor");
	
	
/** KOLEJKI WIADOMOŚCI POSIX **/
	// więcej w `man 7 mq_overview`
	mqd_t kolejka;
	
	struct mq_attr atrybuty;
kolejki:
	atrybuty.mq_maxmsg=3; // pojemność kolejki - 3 wiadomości
	atrybuty.mq_msgsize=20; // po 20 bajtów każda
	
	// otwarcie kolejki
	// dla kolejki o domyślnych parametrach można podać NULL jako ostatni argument
	// w drugim argumencie można podać
	//  O_NONBLOCK można podać aby kolejka była nie blokująca
	//  O_EXCL - działanie takie samo jak w sem_open()
	// trzeba podać tryb (read, write, read+write)
	kolejka = mq_open("/moja_kolejka", O_CREAT | O_RDWR, 0777, &atrybuty);
	if (kolejka == (mqd_t) -1) {
		perror("mq_open");
		return -1;
	}
	
	char buf[100]; // mogły być 20 bo tyle wynosi mq_msgsize, ale potem się nam przyda ...
	unsigned priorytet;
	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec+=1;
	
	// odbieramy wiadomość z timeoutem ...
	size = mq_timedreceive(kolejka, buf, 100, &priorytet, &timeout);
	// mamy także możliwość odbioru bez timeoutu bądź zażądania powiadomienia
	// o wpisaniu pierwszej wiadomości do niepustej kolejki - mq_notify()
	if (size > 0)
		printf("odebrałem %d bajtów z priorytetem %d: %s\n", size, priorytet, buf);
	else
		perror("mq_timedreceive");
	
	// wysyłamy wiadomość do kolejki
	// funkcja ta zawiesi proces gdy nie ma miejsca w kolejce która nie jest
	// otwarta z O_NONBLOCK, istnieje też wariant z timeoutem
	mq_send(kolejka, "Ala ma kota", 12, 9);
	// kolejka może mieć zarówno wielu producentów jak i odbiorców
	// raz odebrana wiadomość znika z kolejki
	//
	// wiadomości odbierane będą w kolejności priorytetów
	// a w ramach jednego priorytetu w kolejności wysyłania
	//
	// w przypadku braku miejsca w kolejce procesy wysyłające są zawieszane
	
	sleep(5);
	
	mq_unlink("/moja_kolejka");
	
	
	
/** timery POSIX **/
	// więcej w `man timer_settime`
	// przygotowujemy strukturę opisującą zdarzenie związane z tyknięciem timera
	// taki sam mechanizm zdarzeń wykorzystywany jest w wspomnianej wcześniej mq_notify()
	struct sigevent zdarzenie_timera;
	zdarzenie_timera.sigev_notify = SIGEV_SIGNAL; // zostaniemy poinformowani sygnałem
	// może być także SIGEV_THREAD wtedy w sigev_notify_function podajemy funkcje
	// która ma utworzyć nowy watek, w sigev_notify_attributes atrybuty wątku a w
	// sigev_value.sival_ptr argument funkcji wątku ... przykład w man 3 mq_notify
	zdarzenie_timera.sigev_signo = SIGPWR; // będzie to sygnał SIGPWR
	zdarzenie_timera.sigev_value.sival_int = 13; // wartość sygnału
	
	// musimy zgłosić ze jesteśmy zainteresowania obsługą SIGPWR
	signal(SIGPWR, &sig_usr);
	
	// tworzymy timer
	timer_t licznik_czasu;
	timer_create(CLOCK_REALTIME, &zdarzenie_timera, &licznik_czasu);
	// gdyby zamiast zdarzenie_timera podać NULL to jesteśmy budzeni sygnałem SIGALRM
	
	// konfigurujemy i uruchamiamy timer
	struct itimerspec zaleznosci_czasowe;
	// pierwsze wykonanie timera po 100ms
	zaleznosci_czasowe.it_value.tv_sec=0;
	zaleznosci_czasowe.it_value.tv_nsec=100000000;
	// kolejne co 1s
	zaleznosci_czasowe.it_interval.tv_sec=1;
	zaleznosci_czasowe.it_interval.tv_nsec=0;
	timer_settime(licznik_czasu, 0, &zaleznosci_czasowe, NULL);
	// warto zwrócić uwagę na flagę TIMER_ABSTIME umożliwiającą ustawienie timera
	// na czas absolutny (a nie okres czasu)
	// jeżeli it_interval będzie wyzerowane to timer wykona się tylko raz
	char i=0;
	do {
		struct itimerspec pozostalo_do_timera;
		timer_gettime(licznik_czasu, &pozostalo_do_timera);
		printf("do timera posostalo: %dns\n", pozostalo_do_timera.it_value.tv_nsec);
		pause(); // czekamy na timer
		printf("aktualna liczba utraconych tyknięć timera wynosi %d\n",
			timer_getoverrun(licznik_czasu));
	} while(i++<4);
	// powyższa metoda gwarantuje rozpoczynanie w równych odstępach czasu ...
	// w przypadku chęci zastosowania sleep() należałoby obliczać czas który się
	// wykonywaliśmy i odejmować go od czasu który podajemy do sleep()
	
	// robimy sobie drzemkę ...
	struct timespec drzemka, pobudka;
	drzemka.tv_sec=2;
	drzemka.tv_nsec=100000000;
	// chcemy spać 2.1s ale obudzi nas timer
	// to ile niedospaliśmy zostanie zapisane w pobudka
	if (nanosleep(&drzemka, &pobudka)<0 && errno == EINTR)
		printf("niedospaliśmy: %d s i %d ns\n", pobudka.tv_sec, pobudka.tv_nsec);
	
	// usuniecie timera
	timer_delete(licznik_czasu);
	
	
/** ASYNCHRONICZNE IO **/
	struct aiocb opis_aio;
	opis_aio.aio_fildes = open( "/etc/passwd", O_RDONLY); // czytamy plik
	opis_aio.aio_offset = 0; // od początku
	opis_aio.aio_buf = (void *) buf; // do bufora buf
	opis_aio.aio_nbytes = 100; // ile czytamy
	opis_aio.aio_sigevent.sigev_notify = SIGEV_NONE;
	// nie korzystamy z powiadomienia o końcu
	// jest to taki sam mechanizm jak dla timerów ...
	opis_aio.aio_reqprio = 1; // priorytet operacji
	
	if (aio_read(&opis_aio))
		perror("aio_read");
	
	// czekamy na koniec operacji io
	// w ogólności możemy podać cala listę operacji io
	// - funkcja wychodzi gdy którakolwiek się z nich zakończy
	const struct aiocb* const lista_aio[] = { &opis_aio };
	aio_suspend(lista_aio, 1, NULL);
	
	// odbieramy stan zakończenia
	size = aio_error(&opis_aio);
	if (size == 0) {
		size = aio_return(&opis_aio);
		
		buf[99]='\0';
		printf("Wczytałem %d bajtów: %s\n", size, buf);
	} else {
		printf("aio_error return %d\n", size);
	}
}
