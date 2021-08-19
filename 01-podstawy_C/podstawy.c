/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje podstawy języka - preprocesor, zmienne itp.
 *
 */

// komentarzem jest cały tekst od // do końca linii (komentarz C++)
// oraz test zawarty pomiędzy /* i */ (komentarz C)

// na wstępie kilka ogólnych uwag składniowych:
//  * program składa się z ciągu instrukcji, poszczególne instrukcje kończą się ;
//  * bloki instrukcji ujmuje się w klamerki {}
//  * argumenty do funkcji przekazuje się w nawiasach () i rozdziela przecinkami
//      pewnym odstępstwem jest instrukcja for gdzie podajemy 3 bloki instrukcji rozdzielane ;
//  * ciągi znaków umieszcza się wewnątrz cudzysłowów "" i są one tablicą char'ów
//  * białe znaki (spacje, tabulatory, nowe linie) na ogół nie odgrywają roli
//      jednym z wyjątków są nawiasy kwadratowe [] po nazwie tablicy w których umieszcza
//      się indeks elementu tablicy - tam nie powinno być spacji


// polecenie preprocesora włącza wskazany plik jeszcze przed kompilacją
// plik może być podany z ścieżką względną wobec bieżącego katalogu np:
// #include "moja_biblioteczka/hello.c"
// może też być podany standardowy plik nagłówkowy
// (ścieżka względem katalogu zależnego od systemu - np. /usr/include) np:
#include <stdio.h>
#include <stdlib.h>

// poniższe polecenie każe zastąpić w kodzie programu KAŻDE wystąpienie _NAPIS_
// poprzez "Hello World"
#define _NAPIS_ "Hello World"
// w podobny sposób możemy definiować pseudo funkcje
#define _SUMA_(a,b) (a + b)
// poprzedzenie argumentu # powoduje ujęcie go w ""
#define _WYPISZ_(a) printf(#a)
// ## powoduje sklejenie ...
// w zasadzie zaleca się ograniczanie #define (w powyższych zastosowaniach)
// na rzecz stałych i funkcji inline

// możemy też warunkować włączanie fragmentów kodu
#define PL

#ifdef PL
	// ten fragment wykona się tylko gdy zdefiniowana PL
	#undef _NAPIS_
	// oddefiniowalismy _NAPIS_
	#define _NAPIS_ "Witaj World"
	// i zdefiniowaliśmy go inaczej
#endif

// stała napisowa
const char* napis = "Witaj świecie\n";

// funkcja inline - wstawiana literalnie w miejscu wywołania (nie powoduje
// narzutu związanego z normalnymi funkcjami) zwiększa natomiast rozmiar programu
// dostępne w C++ i w współczesnych C (dodane w C99)
// w C++ wystarczy "inline" zamiast "static inline"
// kompilator może zdecydować o nie wstawianiu takiej funkcji jako inline,
// to słowo kluczowe jest tylko sugestią dla kompilatora
static inline void wypisz(const char* napis) {
	puts(napis);
}

// zmienne globalna - dostępna w każdej funkcji zdefiniowanej w tym pliku
// uwaga niekiedy może zajść konieczność przypisania wartości dopiero w main()
int kontrolna = 9;
double tablica_kontrolna [2] = {1, 2};

// deklaracja funkcji, funkcje zdefiniujemy po main(), mogłaby też być
// dołączona na etapie linkowania ...
int funkcja (int x, int y);

// funkcja główna programu ... rozpoczęcie programu oznacza wywołania main,
// zakończenie main oznacza zakończenie programu
int main(int argc, char *argv[], char *envp[]) {

	// zabawy z preprocesorem ...
	printf("%s\n", _NAPIS_);
	printf("%d\n", _SUMA_(5,3));
	_WYPISZ_ (witaj świecie !!! \n);
	
	// wykorzystanie stałych i funkcji inline
	wypisz (napis);
	
	// zmienne globalne
	tablica_kontrolna [1] = 10.5;
	
	// podstawowe operacje matematyczne
	printf("suma: %d\n", 2 + 5);
	printf("różnica: %d\n", 2 - 5);
	printf("mnożenie: %d\n", 2 * 5);
	printf("dzielenie całkowite: %d\n", 2 / 5);
	printf("dzielenie: %f\n", 2.0 / 5.0);
	// uwaga: dzielenie całkowite gdy oba argumenty całkowite
	printf("reszta z dzielenia: %d\n", 2 % 5);
	
	// deklaracja zmiennej
	//  typ nazwa;
	//  typ nazwa2;
	//  typ nazwa = wartosc_poczatkowa;
	//  typ nazwa = wartosc_poczatkowa, nazwa2 = wartosc_poczatkowa2;
	
	// nazwy skladac sie moga z liter, cyfr, znaków _ i zaczynają się od litery
	
	// typy podstawowe - patrz:
	//  http://pl.wikipedia.org/wiki/C_(język_programowania)#Podstawowe_typy
	// warto też zwrócić uwagę na typy stałych np.:
	//  98 to int a 98.0 to double, 98.0F to float,
	//  'c' odpowiada typowi char z wartością równą kodowi ASCII znaku c
	
	// liczby zmiennopozycyjne posiadają wartości oznaczające nieskończoność czy też
	// wynik nie będący liczbą (Not a Number, czyli NaN)
	double a = 0.0/0.0;
	if( !(a==a) )
		printf("a jest NaN\n");
	printf ("%f %f %f\n", 2.0/0.0, -4343.0/0.0, a);
	// uwaga wartości NaN odpowiada bardzo wiele wartości binarnych zmiennej ...
	
	
	// mamy też możliwość definicji bardziej zaawansowanych typów:
	int tablica [2];
	// tablica 2 elementowa liczb całkowitych, tablica indeksowana od zera !!!
	// możemy tworzyć aliasy dla typów
	typedef int calkowita;
	calkowita blabla2; // blabla2 jest zmienną int ...
	// jest jeszcze tego trochę ... ale na początek to wystarczy ...
	// patrz też wskaźnikologia.c
	
	// ilość komórek w tablicy (nie chce działać na wskaźnikach)
	printf("Dlugość tablicy: %ld\n", sizeof(tablica)/sizeof(tablica[0]) );
	
	{
	double liczba2 = 21.789;
	// poniżej prezentuję rzutowanie czyli konwersje typu, zasad jest taka:
	// (nowy_typ) obiekt_do_konwersji
	int liczba = (int) liczba2;
	printf("%d %f\n", liczba, liczba2);
	}
	//printf("%d %f\n", liczba, liczba2);
	// teraz nie istnieje zmienna liczba2 gdyż zasięg zmiennych ograniczają klamerki
	
	// możemy też konwertować napisy na liczby ...
	// zobacz też man 3 strtod i man 3 atoi
	int liczba = atoi( "43" );
	double liczba2 = atof( "35.71" );
	printf("%d %f\n", liczba, liczba2);
	
	// wywołanie funkcji zwracającej wartość może wyglądać tak:
	liczba = funkcja (29, 84);
	// jeżeli zwracana wartość nas nie interesuje możemy ją zignorować:
	// funkcja (29, 84);
	
	// na koniec o funkcjach matematycznych - w pliku <math.h>
	// mamy wiele funkcji matematycznych, takich jak: sin, exp, abs - modół, ...
}

// definicja funkcji
int funkcja (int x, int y) {
	int z; // zmienna lokalna funkcji
	z = x - y;
	// funkcja coś robi i zwraca wynik przez return
	// wynikiem może być prosta zmienna (pojedyncza liczba), wskaźnik lub strukturę
	// dzięki tym dwóm ostatnim metodom możemy zwracać z funkcji kilka wartości
	// podobnie jak możemy wykorzystać do tego argumenty przekazywane przez wskaźniki
	return z;
	// mogłoby też być tak: return x - y;
}
