/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje standardowe wejście i wyjście, równierz odczyt i zapis plików
 *
 */


#include <stdio.h>

int main() {
	// najprostsze wypisywanie napisu
	fputs( "Hello World !!!\n", stdout );
	puts( "Hello World !!!\n" );
	// oba wyżej zaprezentowane wywołania są równoważne,
	// przy czym pierwsze można wykorzystać do zapisu do pliku - zamiast stdout
	// jako drugi argument podajemy identyfikator otwartego do zapisu pliku
	// (o tym za chwilę ...), tak samo jest w wszystkich innych funkcjach gdzie
	// podaje jako argument stdout lub stdin
	
	// najprostsze wypisywanie napisu
	char c = 'q';
		// definiujemy zmienną całkowitą jedno bajtową z przypisaniem
		// numeru ASCII (patrz man ascii) znaku q
	fputc( c, stdout );
	putc( c, stdout ); // starszy wariant ...
	putchar( c );
	putchar( '\n' ); // znak nowej linii wprowadzamy przez \n
	
	// najprostsze czytanie znaku
	puts( "Podaj znak i naciśnij ENTER" );
	int d = fgetc (stdin); // jest to równoważne: int d = getchar();
	getchar();
		// wczytujemy tamten enter
		// ... bo inaczej dalsze czytanie będzie mogło mieć problemy
	if (d != EOF)
		c = (char) d;
	
	// najprostrze czytanie napisu (linii)
	puts( "wpisz coś i naciśnij ENTER" );
	char napis [20]; // 20 elementowa tablica znaków
	fgets( napis, 20, stdin );
	// warto zauważyć że wczytany napis zawiera znak końca linii, jeżeli się
	// zmieścił, a jeżeli nie to dalsze wczytywanie będzie mieć problemy ... istnieje
	// też funkcja gets wymagająca podania tylko nazwy tablicy do której ma czytać
	
	//gets( napis );
	// jednak stosowanie jej jest wyraźnie odradzane:
	// "warning: the `gets' function is dangerous and should not be used."
	// gdyż niczym nie jest ograniczona ilość wczytywanych znaków,
	// co może prowadzić do błędów przepełnienia bufora ...


	// wyjście formatowane
	fprintf(stdout, "%% Podałeś znak %c oraz napis %s\n", c, napis);
		// funkcja ta podstawia w miejsce znaczników %x odpowiednie zmienne
		// - po kolei w miejsce pierwszego %x pierwszy argument po napisie
		// formatującym (u nas za %c zmienna c za %s zmienna napis);
		// znaki występujące po % informują jak ma być traktowana dana zmienna,
		// aby uzyskać znak % należy wpisać %% (jak w powyższym przykładzie),
		// więcej o napisach formatujących - man 3 printf
		// uwaga: ilość i typy argumentów muszą się zgadzać !!!
		// a w większości kompilatory tego nie sprawdzają
	snprintf( napis, 20, "Podałeś znak %c", c );
		// tak jk wyżej tyle że do wskazanej tablicy
		// rozszerzania GNU dodają również funkcje asprintf, która dostaje
		// wskaźnik, a pamięć potrzebą do stworzenia takiej tablicy
		// przydziela sama ... więcej w napisy.cpp
	printf("napis ma teraz postać: %s\n", napis);
		// tak jak fprintf tyle że zawsze na stdout
	
	// wejście formatowane
	puts( "wpisz liczbę całkowitą\n" );
	fscanf (stdin, "%i", &d);
		// funkcja czyta pierwszy argument z pliku (u nas stdin - czyli
		// standardowego wejścia) i rozkłada go według napisu formatującego,
		// więcej o napisach formatujących - man 3 scanf
		//
		// funkcję tę można wykorzystać np. do parsowania konfigów "zmienna = wartosc"
		// while ( fscanf(fp, "%10s = %10s\n", name, val) != EOF ) {
		//   if ( strlen(name) == 0 || strlen(val) == 0 ) continue;
		//   ...
		// }
	printf("wczytaliśmy liczbę: %i\n", d);
		// oczywiście jest scanf, które nie ma pierwszego argumentu i domyślnie
		// czyta z sdtin oraz parę innych, w tym sscanf:
	double liczba;
	sscanf("3.6", "%lf", &liczba);
	printf("a to była liczba: %f\n", liczba);
		// uwaga: w *scanf należy dodatkowo pamiętać o tym iż należy przekazywać
		// adresy zmiennych do których funkcja ma czytać !!!


	// czytanie i pisanie do pliku
	FILE *plik;
	plik=fopen("/tmp/plik1.txt", "w+");
		// otwieramy plik określony w pierwszym argumencie, w trybie określonym
		// w drugim argumencie (r - odczyt, w - zapis, a - dopisywanie,
		// + - dwukierunkowy (używane po r, w albo a)
	fputs("Hello World !!!\n", plik); // wpisujemy do pliku
	printf("aktualna pozycja w pliku to %ld\n", ftell(plik));
	
	fflush(plik);
		// jako że są to operacje buforowane to aby mieć pewność że to jest już
		// w pliku należy wykonać fflush(), nie jest to konieczne gdy zamykamy
		// plik (wtedy wykonywane jest z automatu)
	fseek(plik, 0, SEEK_SET);
		// przewijamy do początku jest to równoważne rewind(plik);
	fread(&napis, 20, 1, plik);
		// czytamy z pliku 20 elementów o wielkości 1 bajta do tablicy napis
	
	freopen("/tmp/plik2.txt", "a", plik);
		// zmieniamy nazwę pliku dla naszej struktury FILE
		// oprócz fopen, freopen jest też fdopen, która otwiera plik
		// identyfikowany numerycznym deskryptorem a nie ścieżką i popen,
		// która otwiera potok w wybraną stronę do zadanej komendy
		// przykłady użycia w fork.c
	fwrite(&napis, 20, 1, plik);
		// zapisujemy do pliku 20 elementów o wielkości 1 bajta z tablicy napis
	fclose(plik);
		// zamykamy plik ...
	
	// oczywiście funkcje fwrite i fread można stosować do czytania sdtin i pisania na stdout
	// funkcje te są szczególnie użyteczne przy operacji danymi binarnymi:
	printf("DANE BINARNE:\n");
	fwrite(&liczba, 1, sizeof(double), stdout);
	printf("\nKONIEC DANYCH BINARNYCH:\n");
	
	// mamy też funkcje zmieniające nazwę pliku:
	//  rename("sciezka1", "sciezka2");
	// kasujące plik
	//  remove("sciezka");
	// oraz tworzące unikalną nazwę (nie plik) pliku (uważaną za niebezpieczną): tmpnam()
	// albo zwracającą od razu obiekt typu FILE pliku tymczasowego (usuwanego po
	// zakończeniu programu) - tmpfile(): plik = tmpfile();
}
