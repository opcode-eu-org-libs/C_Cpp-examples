#define _GNU_SOURCE // dla rozszerzeń GNU (asprintf)

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

// C null-terminated strings
// const char*  char*  ... nie gwarantuje terminowania NULLem

int main() {
	// stała napisowa
	const char* text1 = "Ala";
	printf("%s", text1);
	
	// bufor napisowy o określonej długości
	// - coś krótszego możemy przypisać przy incicjalizacji
	// możemy też pominąć 40 wtedy tablica ta będzie miała długość
	// odpowiadająca naszemu napisowi ...
	char text2 [40] = " ma kota\n";
	printf("%s", text2);
	
	// z użyciem snprintf() możemy swobodnie operować buform napisowym
	// (nadpisywać jego zawartość, dopisywać coś do niego, także wypisywać wartości zmiennych)
	int pos = snprintf(text2, 40, "%s ma %d koty", text1, 3);
	if (pos < 40) // uwaga: wartość zwrócona przez snprintf może być większa niż podany rozmiar bufora
		pos += snprintf(text2+pos, 40-pos, " i %d psy", 2);
	if (pos < 40)
		pos += snprintf(text2+pos, 40-pos, "\n");
	printf("%s", text2);
	
	printf("długość: %d == %ld\n", pos, strlen(text2));
	// funkcja strlen() oblicza długość napisu w oparciu o wyszukanie w nim
	// bajtu o wartości 0 (NULL) oznaczającego koniec ciągu napisowego
	
	// rozszerzania GNU dodają również funkcje asprintf,
	// która dostaje wskaźnik, a pamięć potrzebą do stworzenia takiej
	// tablicy przydziela sama ...
	char *wsk = 0;
	asprintf(&wsk, "bla bla bla\n");
	printf("%s", wsk);
	// ale to my musimy ją zwolnić gdy nie jest już potrzebna
	free(wsk);
	
	// natomiast nie pójdzie:
	// text2 = "test";

	// można też z char* zrobić const char*
	const char* text3 = text2;
	printf("%s", text3);
	
	const char* n1 = "abc";
	const char* n2 = "ABC";
	const char* n3 = "ABCDE";
	
	// porównywanie napisów: strcmp(), strncmp(), strcasecmp(), strncasecmp(), ...
	// wynik zero z tych funkcji oznacza równość napisów
	printf("%s =?= %s: %d\n", n1, n1, strcmp(n1, n1));
	printf("%s =?= %s: %d\n", n1, n2, strcmp(n1, n2));
	printf("%.2s =?= %.2s: %d\n", n3, n2, strncmp(n3, n2, 2)); // porównujemy tylko dwa bajty
	printf("%s =?= %s: %d\n", n1, n2, strcasecmp(n1, n2)); // porównujemy bez uwzględniania wielkości liter
	
	// wyszukiwanie pozycji podnnapisu:
	char* found = strstr(text2, "kot");
	if (found) {
		printf("kota występuje na pozycji %ld\n", found-text2);
		
		// przecięcie napisu w wyszukanym miejscu
		*found = 0;
		printf("napis przed '%s', napis po '%s'\n", text2, found+1);
		
		// jeżeli nie możemy / nie chcemy modyfikować oryginalnego napisu
		// wstawiając \0 aby obciąć początek w znalezionej pozycji to
		// możemy użyć np. strncpy aby utworzyć kopię części napisu
	} else {
		printf("nie znaleziono kot w %s", text2);
	}
	
	// wyszukiwanie pozycji znaku z podanego zbioru:
	char* n5 = "Ala ma kota, psa, papugę. Nie ma królika.";
	found = n5;
	do {
		found = strpbrk(found, ",.;");
		if (found) {
			printf("znak przystankowy (%c) na pozycji %ld\n", *found, found-n5);
			++found; // następne wyszukanie musimy zrobić od znaku + 1
		}
	} while(found);
	
	// więcej przydatnych funkcji operujących na takich napisach opisanych jest w: man 3 string
}
