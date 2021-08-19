/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje rozgałęzianie programu i komunikację międzyprocesową
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void zakonczenie() {
	fprintf(stderr ,"Postanowiłem skończyć ... PID %d\n", getpid());
}

void potomek_umarl(int sig) {
	// odbieramy (i ignorujemy) kod powrotu potomka ... dzięki temu przestanie być zombie
	waitpid(-1, NULL, WNOHANG);
}

int main(int argc, char *argv[], char *envp[]) {
	// należy określić funkcje obsługująca sygnał informujący o śmierci potomka
	// i wywołująca wtedy funkcje z waitpid(-1, NULL, WNOHANG); ... inaczej powstawałyby procesy zombie
	signal(SIGCHLD, &potomek_umarl);
	// zobacz też unix.c
	
	pid_t pid, parent_pid;
	parent_pid = getpid();
	
	// ustalamy jaka funkcja ma być wykonywana przed exit(); nie będzie wykonywana przed abort();
	// która brutalniej kończy program
	atexit( zakonczenie );
	
	// rozgałęziamy proces
	switch (pid = fork()) {
		case -1: // funkcja zwróciła -1 co oznacza błąd
			fprintf(stderr, "Error in fork\n");
			return 1;
		
		case 0: // funkcja zwróciła zero co oznacza że jesteśmy w procesie potomnym
		{
			printf("Jestem procesem potomnym. Moj PID = %d, PID mojego rodzica to: %d\n", getpid(), parent_pid);
			
			// mogę wykonać jakiś program i dalej kontynuować
			int retcode = system ("ls -l");
			printf("Nadal żyję !!! A komenda odpalona w system() zwróciła kod powrotu: %d\n", WEXITSTATUS(retcode));
			
			// do uruchomienia zewnętrznego programu można także spróbować użyć posix_spawnp()
			// działa ona na zasadzie połączenia fork() i exec() ... więcej info w man
			
			char* katalog_domowy = getenv("HOME");
				// pobieram zawartość zmiennej środowiskowej HOME (określającej katalog domowy)
			printf("moim katalogiem domowym jest: %s\n", katalog_domowy);
			
			// dostęp do zmiennych środowiskowych możliwy jest także przez tablicę będącą
			// trzecim argumentem funkcji main:
			while (*envp) { // pętla wypisuje wszystkie przekazane do programu zmienne środowiskowe
				printf("%s\n", *envp++);
		}
			
			// można również tworzyć zmienne środowiskowe ...
			//  setenv("nazwa zmiennej", "tresc", nadpisz);
			// nadpisz==0 nie nadpisuję ewentualnej istniejącej takiej zmiennej
			// nadpisz==1 nadpisuje
			setenv("HOME", "/tmp", 1);
			// uwaga: zmianie ulega tylko środowisko aktualnego programu i programów przez niego
			//        wywoływanych a nie systemu (inne procesy nie odczują zmiany)
			// oraz usuwać:
			//  unsetenv("fork_testowa"); // usuwam utworzoną przez siebie zmienną środowiskową
			
			printf("a teraz moim katalogiem domowym jest:\n");
			// mogę też zastąpić siebie jakimś programem
			execlp("printenv", "printenv", "HOME", 0);
			printf("a teraz zostałem zastąpiony innym programem i się nie wypiszę :-(\n");
			
			exit(0);
		}
		
		default: // funkcja zwróciła coś innego od 0 i -1 oznacza to że jesteśmy w pocesie macierzystym
				// i otrzymaliśmy pid naszego dziecka ...
		{
			printf("Jestem procesem macierzystym. Moj PID = %d, PID mojego potomka to: %d\n", getpid(), pid);
			
			// poczekam aż potomek się zakończy ...
			// warto zauważyć że dopóki nie będę czekał wykonuje się współbieżnie z potomkiem ...
			// w szczególności nawet mógłbym się zakończyć ...
			// rodzicem potomka powinien stać się wtedy init (proces o PID==1)
			wait(0);
			printf("potomek już zrobił swoje ...\n");
		}
	}
	
	
	// tworzymy łącze nienazwane - rurę ...
	int deskryptory_rury1 [2];
	if ( pipe(deskryptory_rury1) == -1 ) fprintf(stderr, "Error in pipe\n");
	//deskryptory_rury1[0] - odbiór z rury
	//deskryptory_rury1[1] - wysyłanie przez rurę
	
	// znowu rozgałęziamy proces
	switch (pid = fork()) {
	case -1: // funkcja zwróciła -1 co oznacza błąd
		fprintf(stderr, "Error in fork\n");
		return 1;
	
		case 0:
		{
			dup2(deskryptory_rury1[1], 1); // podmieniamy deskryptor 1 (stdout) na koniec rury do pisania
			// dla stdin, sdtout i stderr mamy domyślnie otwarte deskryptory o numerach ustalonych na stałe
			// nazwa strumienia in/out - numer deskryptora
			// stdin - 0
			// stdout - 1
			// stderr - 2
			close(deskryptory_rury1[1]); // zamykamy bo skopiowaliśmy na stdout
			close(deskryptory_rury1[0]); // zamykamy bo piszemy a nie czytamy
			system ("uname -a");
			
			pause(); // zatrzymuje program w oczekiwaniu na sygnał
			
			while (1); // to i tak się nie wykona bo sygnałem będzie SIGTERM
			
			return 0;
		}
	
		default:
		{
			FILE *rura = fdopen(deskryptory_rury1[0], "r"); // tym razem kojarzymy z rurą obiekt typu FILE ...
			// identyczny efekt można by uzyskać przez
			//FILE *rura =  popen( "uname -a", "r" );
			// jednak ten w odróżnieniu od prezentowanego tutaj nie umożliwia skonstruowania
			//  komunikacji dwukierunkowej
			
			char napis [20];
			fgets( napis, 20, rura ); // pobieram z rury 20 znaków ...
			printf("%s\n", napis); // i wypisuję
			
			kill(pid, SIGTERM); // prosimy potomka by się zakończył ... poprzez wysłanie sygnału (więcej w unix.c)
			return 1;
		}
	}
}
