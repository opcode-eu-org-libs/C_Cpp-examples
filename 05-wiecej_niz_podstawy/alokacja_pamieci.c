/*
 * plik ma na celu prezentację metod ręcznego zarządzania pamięcią
 *
 */


#include <stdio.h>
#include <stdlib.h> /* for malloc itp */
#include <string.h> /* for memcpy */

int main (int argc, char *argv[]) {
	/// ręczna alokacja pamięci
	/// na ogół podobny efekt można uzyskać deklarując tablica[jakas_zmienna]
	/// ale to nie działa w starszych implementacjach C
	/// zobacz: http://pl.wikibooks.org/wiki/C/Wskaźniki#Dynamiczna_alokacja_pami.C4.99ci
	/// ponadto metoda tablica[wartosc] narzuca ograniczenie na ich rozmiar związane
	/// z ograniczeniem rozmiaru stosu, dlatego lepiej duże tablice alokować dynamicznie

	
	// wskaźnik na naszą tablicę
	char *tab = NULL, *tab2 = NULL;
	// rozmiar naszej tablicy
	int tab_size = 15;
	// alokacja pamięci dla naszej tablicy + nadanie poprawnej wartości wskaźnikowi na nią
	// podobnie działa calloc z tym że dodatkowo zarezerwowany obszar zapisuje zerami
	tab = malloc(sizeof(*tab) * tab_size );
	
	/// wpisanie czegoś do naszej tablicy
	memcpy(tab, "Ala ma kota", 12); // długość napisu + 1 bo null-end string
	
	// sprawdzenie
	printf("%s - %c\n", tab, tab[4]);
	
	// ustalenie nowego rozmiaru i inne pomocnicze
	// aby zobaczyć że następuje przeniesienie należy program wywołać z argumentem
	// stanowić on będzie rozmiar nowej tablicy oraz spowoduje zajęcie pamięci za naszą tablicą
	// użycie rozmiaru
	if (argc > 1) {
		tab_size = atoi(argv[1]);
		char *tab2 = malloc(sizeof(*tab) * 1000 );
	} else {
		tab_size = 40;
	}
	char *old_tab = tab;
	
	/// zwiększenie rozmiaru naszej tablicy
	// jeżeli nie jest możliwe zwiększenie obecnego rozmiaru (musi to być obszar ciągły)
	// alokowany jest nowy obszar, a dane są kopiowane
	// w związku z powyższym zwrócony wskaźnik może się różnić od podanego !!!
	tab = realloc(tab, sizeof(*tab) * tab_size);
	
	if (old_tab == tab)
		puts("nieprzeniesione");
	else
		puts("przeniesione");
	
	// sprawdzenie
	printf("%s - %c\n", tab, tab[25]);
	memcpy(tab, "Ala ma kota. Kot ma Ale. Czyżby ...", tab_size);
	printf("%s - %c\n", tab, tab[25]);
	
	/// usuwamy naszą tablice
	free(tab); // zwalniamy ten obszar pamięci
	tab = NULL; // kasuję wartość wskaźnika (bo obszar zwolniony)
	
	if (tab2)
		free(tab2);
}
