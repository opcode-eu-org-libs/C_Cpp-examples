/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje metodę odczytu katalogów
 *
 */

// będziemy coś wypisywać ...
#include <stdio.h>

// funkcja exit
#include <stdlib.h>

// katalogi
#include <dirent.h>

int main() {
	DIR* katalog = opendir( "/tmp" ); // otwieramy katalog
	// obsługa błędów - np nie istniejącego katalogu
	if (! katalog) {
		perror( "próba otwarcia katalogu" );
	}
	
	printf("typ\tnazwa\n");
	struct dirent* pozycja = 0; // tworzymy strukturę odpowiedzialną za przechowywanie informacji o pozycji katalogu
	while (pozycja = readdir(katalog)) { // pętla po kolejnych pozycjach katalogu
		printf("%d\t%s\n", (*pozycja).d_type, (*pozycja).d_name);
	}
	
	closedir(katalog); // zamknięcie katalogu
	
	// Warto także wspomnieć o POSIXowych funkcjach do operacji na plikach i katalogach
	// jako całych "obiektach" (takich jak ich usuwanie, przenoszenie itd.).
	//
	// Z poziomu C możemy wykonywać takie operacje m.in. przy pomocy funkcji:
	//  * rename - zmienia nazwę i/lub położenie pliku (odpowiednik komendy mv) – patrz `man 2 rename`
	//  * unlink - usuwa plik (odpowiednik komendy rm) – patrz `man 2 unlink`
	//  * mkdir - tworzy katalog – patrz `man 2 mkdir`
	//  * rmdir - usuwa pusty katalog – patrz `man 2 rmdir`
	//  * chmod - zmiana uprawnienia dostępu do pliku – patrz `man 2 chmod`
	//  * chown - zmiana użytkownika i grupy – patrz `man 2 chown`
	//  * chdir - zmiana aktualnego katalogu (odpowiednik komendy cd ...) – patrz `man 2 chdir`
	//  * getcwd - zwraca aktualny katalog, tak jak getenv("PWD") – patrz `man 2 getcwd`
	//
	// Należy zaznaczyć tutaj iż nie ma funkcji systemowej kopiującej plik
	//    - kopiowanie odbywa się przez otwarcie odczyt w pętli i zapis pod nową nazwą.
	
	exit (1);
}
