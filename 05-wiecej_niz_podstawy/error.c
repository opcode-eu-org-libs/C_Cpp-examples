/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje metody korzystania z komunikatów o błędach errno
 *
 */

#include <stdio.h>
#include <stdlib.h>

// obsługa błędów
#include <errno.h>

// obsługa błędów - funkcja strerror
#include <string.h>

int main() {
	FILE *plik;
	plik=fopen("/nie_ma_takiego_pliku.txt", "r");
	if (!plik) {
		// nie udało się - zapamiętujemy kod błędu
		int kod_bledu=errno;
		
		// mamy parę metod wypisania ...
		perror( "próba otwarcia katalogu" );
		fprintf(stderr, "błąd (kod %d): \"%s\" (%s : %d)\n", kod_bledu, strerror(kod_bledu), __FILE__, __LINE__);
		
		// warto także zapoznać się z funkcjami z rodziny: err(), warn(), a także error() i syslog()
		// w szczególności użyteczny jest w nich output formatowany printf z obsługą %m (wstawia komunikat z ernno)
		//
		// error(-1, 0, "Ala ma %d kotów: %m", 5); lub errx("Ala ma %d kotow: %m", 5);
		//    wypisze komunikat podstawiając za %m aktualny błąd errno i zakonczy progam z kodem -1
		// error(0, 0, "Ala ma %d kotów: %m", 5); lub warnx("ooo %m ppp %i", 5);
		//    wypisze komunikat podstawiając za %m aktualny błąd errno ale nie zakończy programu
		// error(0, 1, "Ala ma %d kotów: %m", 5);
		//   wypisze komunikat podstawiając za %m błąd odpowiadający errno=1
		
		// kończymy zwracając informację o niepowodzeniu (1) do rodzica
		exit (1);
	}
}
