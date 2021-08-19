/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje korzystanie z komunikacji międzyprocesowej poprzez SystemV IPC (svipc):
 * semafory pamięć współdzielona i kolejki komunikatów
 *
 * warto zajrzeć też na: http://rainbow.mimuw.edu.pl/~mengel/jpp/sem/
 *
 */

#define __GNU_LIBRARY__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>

union semun {
	int val;                    /* wartość dla SETVAL */
	struct semid_ds *buf;       /* bufor dla IPC_STAT i IPC_SET */
	unsigned short int *array;  /* tablica dla GETALL i SETALL */
	struct seminfo *__buf;      /* bufor dla IPC_INFO */
};

int main(int argc, char *argv[]) {
	// pid aktualnego procesu
	int pid = getpid();
	
	// ścieżka do pliku wykonywalnego
	char filePath[128];
	int ret = readlink("/proc/self/exe", filePath, 128);
	if (ret < 1 || ret > 127) {
		perror("readlink");
		return 1;
	}
	filePath[ret]=0;
	
	// klucz IPC
	int projID = 1;
	// semafor będzie wspólny dla wszystkich procesów podających taki sam filePath i (projID & 0xff)
	key_t klucz = ftok(filePath, projID);
	
	printf("%d: uzyskałem klucz: %x\n", pid, klucz);
	
	
/** SEMAFORY **/
	
	/// tworzymy zestaw semaforów bądź uzyskujemy dostęp do istniejącego
	// tworzymy semafor gdy nie istnieje
	int identyfikatorZestawuSemaforow = semget (klucz, 1, 0640|IPC_CREAT|IPC_EXCL);
		// pierwszym argumentem jest liczba całkowita pełniąca funkcje nazwy
		// zestawu semaforów (zamiast niej można podać IPC_PRIVATE wtedy jednak
		// zestawu możemy używać tylko w tym i w potomnych procesach)
		// drugim jest liczba semaforów w zestawie
		// trzecim są flagi (określające prawa dostępu do zestawu semaforów
		// (jak do pliku) oraz sposób otwarcia (IPC_CREAT - utworzenie gdy nie ma
		// lub otwarcie gdy jest, IPC_CREAT | IPC_EXCL - utworzenie gdy nie ma,
		// błąd gdy jest)
	if (identyfikatorZestawuSemaforow != -1) {
	// jeżeli się udało nadajemy mu wartość początkową
		printf("%d: utworzyłem zestaw semaforów o identyfikatorze: %i\n", pid, identyfikatorZestawuSemaforow);
		union semun ctl_arg;
		ctl_arg.val=1;
		semctl (identyfikatorZestawuSemaforow, 0, SETVAL, ctl_arg);
			// drugim argumentem jest numer semafora
			// trzecim argumentem jest polecenie
			// (SETVAL = ustaw na wartość z ctl_arg.val, więcej man 2 semctl)
			// trzecim jest union semun w tym wypadku interesuje nas pole val
	} else if (errno == EEXIST) {
	// w przeciwnym razie korzystamy z istniejącego
		identyfikatorZestawuSemaforow = semget (klucz, 1, 0640);
		printf("%d: uzyskałem dostęp do semafora o identyfikatorze: %i\n", pid, identyfikatorZestawuSemaforow);
	} else {
		printf("BŁĄD: %i", errno);
		return -1;
	}

	/// używanie semaforów
	// - sprawdzamy czy jest zezwolenie (ewentualnie czekamy na nie) i ustawiamy blokadę
	
	// tablica struktur opisujących operacje
	struct sembuf operacje [1];
	// pierwsza operacja
	(operacje[0]).sem_num = 0; // pierwszy, czyli zerowy ;-) semafor
	(operacje[0]).sem_op = -1; // gdy wartość semafora >= |-1| dodaj do wartości
	                           // semafora -1 i kontynuuj, inaczej czekaj ...
	(operacje[0]).sem_flg = 0; // możliwe są:
	//   SEM_UNDO   (zmiana stanu semafora cofnięta w momencie zakończenia procesu) i
	//   IPC_NOWAIT (gdy nie może być wykonana natychmiast kończy semop z błędem)
	
	printf("%d: oczekiwanie na semafor\n", pid);
	
	// wykonanie operacji
	semop (identyfikatorZestawuSemaforow, operacje, 1);
		// trzecim argumentem jest liczba operacji do wykonania na zestawie
		// semaforów (ilość elementów tablicy z argumentu drugiego)
		// operacje te wykonane będą w sposób niepodzielny ...
		// zobacz też: man 2 semop
	
	printf("%d: jestem za semaforem\n", pid);
	printf("%d: naciśnij enter aby zwolnic semafor\n", pid);
	while (getchar() != 10);
	
	(operacje[0]).sem_op = 1;  // zwiększamy wartość semafora (zwalniamy)
	(operacje[0]).sem_flg = 0; // nie potrzebujemy cofnięcia tej operacji
	semop (identyfikatorZestawuSemaforow, operacje, 1);
	
	printf("%d: semafor zwolniony\n", pid);
	
	/// usuwanie semafora
	(operacje[0]).sem_op = -1;
	(operacje[0]).sem_flg = IPC_NOWAIT;
	if (semop (identyfikatorZestawuSemaforow, operacje, 1) != -1) {
		semctl (identyfikatorZestawuSemaforow, 0, IPC_RMID, 0);
		printf("%d: usuwam semafor\n", pid);
	} else {
		printf("%d: nie usuwam semafora\n", pid);
	}
	
	
/** PAMIĘĆ WSPÓŁDZIELONA **/
	
	// tworzymy lub podłączamy się do fragmentu pamięci współdzielonej
	int deskryptor_pamieci_dzielonej = shmget(klucz, 100, 0640|IPC_CREAT | SHM_R | SHM_W);
	if (deskryptor_pamieci_dzielonej < 0) {
		printf("BŁĄD shmget: %i (%s)\n", errno, strerror(errno));
		return -1;
	}
	void * adres_pamieci_wspolnej = (void *) shmat(deskryptor_pamieci_dzielonej, NULL, 0);
	if ((int) adres_pamieci_wspolnej == -1) {
		printf("BŁĄD shmat: %i (%s)\n", errno, strerror(errno) );
		return -1;
	}
	
	// czytamy, zwiększamy o 1 i zapisujemy
	int *wartosc, old, new;
	wartosc = adres_pamieci_wspolnej;
	old = *wartosc;
	new = old + 1;
	*wartosc = new;
	printf("odczytałem %i, ustawiłem %i\n", old, new);
	
	// gdy wartość > 5 usuwamy
	if (new > 5) {
		printf("Zlecam usunięcie wspólnego segmentu pamięci\n");
		// odłączamy od naszej przestrzeni adresowej
		shmdt(adres_pamieci_wspolnej);
		// zaznaczamy do usunięcia
		shmctl(deskryptor_pamieci_dzielonej, IPC_RMID, NULL);
	}
	
	
/** KOLEJKI KOMUNIKATÓW **/
	
	// struktura opisująca komunikaty
	struct {
		long typ;
		char tresc[20];
	} komunikat;

	// uzyskujemy identyfikator kolejki komunikatów
	int identyfikator_kolejki_komunikatow = msgget(klucz, 0640|IPC_CREAT);
	if ( identyfikator_kolejki_komunikatow < 0) {
		printf("BŁĄD msgget: %i (%s)\n", errno, strerror(errno) );
		return -1;
	}
	
	
	// odbieramy komunikat ... jeżeli nie ma to nie czekamy
	if ( msgrcv(identyfikator_kolejki_komunikatow, &komunikat, sizeof(komunikat), 2, IPC_NOWAIT) < 0 ) {
		if (errno != ENOMSG) {
			printf("BŁĄD msgrcv: %i (%s)\n", errno, strerror(errno) );
			return -1;
		} else {
			printf("Brak komunikatów\n");
			
			// wysyłamy komunikat
			komunikat.typ=2;
			strcpy(komunikat.tresc, "Hello");
			if ( msgsnd(identyfikator_kolejki_komunikatow, &komunikat, sizeof(komunikat), 0) < 0) {
				printf("BŁĄD msgsnd: %i (%s)\n", errno, strerror(errno) );
				return -1;
			}
			printf("Nadano komunikat: %s\n", komunikat.tresc);
		}
	} else {
		printf("Otrzymano komunikat: %s\n", komunikat.tresc);
		
		printf("Usuwam kolejkę komunikatów\n");
		// usuwamy kolejkę komunikatów
		msgctl(identyfikator_kolejki_komunikatow, IPC_RMID, NULL);
	}
	
	return 0;
} 
