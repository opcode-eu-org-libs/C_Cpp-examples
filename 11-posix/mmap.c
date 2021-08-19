/*
 * plik ma na celu prezentację możliwości korzystania z plików mapownych na obszary pamięci
 *
 */

#include <stdio.h>
#include <fcntl.h> /* for open */
#include <sys/mman.h> /* for mmap */

int main (int argc, char *argv[]) {
	/// zamapowanie fragmentu pliku (lub urządzenia) do pamięci
	int deskryptor = open( "/etc/passwd", O_RDONLY ); // zobacz unix.c
	if (deskryptor < 0) {
		perror("open");
		return 1;
	}
	
	/// odczyt informacji o wielkości pliku (aby wiedzieć ile bajtów zamapować)
	struct stat informacje_o_pliku;
	int ret = fstat(deskryptor, &informacje_o_pliku);
	if (err < 0) {
		perror("fstat");
		return 1;
	}
	
	/// mapowanie całości pliku do pamięci
	char* tab = mmap(NULL, informacje_o_pliku.st_size, PROT_READ, MAP_SHARED, deskryptor, 0);
	if (!tab) {
		perror("mmap");
		return 1;
	}
	// mapowanie odbywa się w wielokrotnościach rozmiaru strony (PAGESIZE)
	// (czyli jak będzimey chcieli mieć jeden bajt i tak dostaniemy całą stronę)
	// podobnie przesunięcie (tutaj podane jako zero) musi być wielokrotnością
	// wartości PAGESIZE - zobacz getpagesize())
	//
	// mapowanie wykonujemy jako do odczytu (PROT_READ) i wspólne dla wszystkich
	// korzystających z pliku (MAP_SHARED), dzięki MAP_SHARED jest to wygodny mechanizm
	// współdzielenia pamięci przez kilka procesów, jeżeli chcemy mieć własny bufor należy
	// skorzystać z MAP_PRIVATE
	//
	// w przypadku zapisów ważne jest wykonanie msync (synchronizacji) lub munmap (odmapowania)
	
	/// sprawdzenie
	// warto zauważyć że do print podajemy ile danych ma wypisać – zamapowane dane pliku
	// tekstowego są napisem ale nie zakończonym bajtem zero (nie jest to napis języka C)
	printf("%.*s\n", informacje_o_pliku.st_size, tab);
	
	/// odmapowanie
	munmap(tab, 20);
	tab=NULL;
	
	// na koniec warto wspomnieć także o możliwości zablokowania wyswapowywania
	// pamięci naszego procesu (nie tylko mapowanych plików co przede wszystkim całej reszty)
	// lub jej części przy pomocy funkcji mlock lub mlockall
}
