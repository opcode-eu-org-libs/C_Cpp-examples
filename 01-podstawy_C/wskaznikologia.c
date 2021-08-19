/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody bazujące na wykorzystaniu wskaźników
 *
 * czytanie należy zaczęć od funkcji main
 * a w miarę kolejnych wywołań wczytywać się w wcześniej zdeklarowane procedury
 *
 */

#include <stdio.h>

/* ta funkcja nic nie zwraca (void), natomiast odbiera wskaźnik do zmiennej i operuje na niej
   w odróżnieniu od funkcji przyjmującej po prostu zmienna x zmiana wartości tej zmiennej
   dokonywana w tej funkcji będzie widoczna po zakończaniu wykonywania tej funkcji */
void funkcja_1 ( int *x ) {
	*x=(*x) + 1;	/* pod wartość pod wskaźnikiem x podstawiam wartość z pod wskaźnika x powiększoną o 1 */
}

/* funkcja ta przyjmuje tablice i modyfikuje jej drugi element */
void funkcja_2 ( int tab_x[] ) {
	tab_x[2] = 55;

	/* moglibysmy rowniez zwracać (przez return) wskaźnik do tablicy
	   nie byłoby natomiast sensu zwracać w funkcji wskaźnika do zmiennej zadeklarowanej w niej
	   gdyż po zakończeniu funkcji pamięć w niej rezerwowana jest zwalniana
	   i zwracalibyśmy adres do pamięci w której mogą być już zupełnie inne dane */
}


/* funkcja pomocnicza - dodaje dwie liczby i konwertuje zwracana wartość do double */
double suma (int a, int b) {
	return (double)(a+b);
}

/* zmienna globalna która zostanie zainicjalizowana przed uruchomieniem main (o tym mówi static) */
static double mnoznik = 3;
// uwaga: zmienne globalne są alokowane w obszarze pamięci procesu a nie na stosie
//        jak ma to miejsce ze zmiennymi lokalnymi

/* ta funkcja przyjmuje jako argument wskaźnik do funkcji przyjmującej dwa argumenty int i zwracającej double */
typedef double (*pomocnicza)(int, int);
double funkcja_3 (double (*f)(int, int)) {
	/* możemy definiować także po prostu wskaźniki do funkcji,
	   co więcej możemy do tego wykorzystać definiowane samodzielnie typy */
	pomocnicza f2 = f;
	return mnoznik * f(2, 5) + f2(1,1);
}

/* przykładowa strukturka ... */
typedef struct struct_dane dane;
struct struct_dane {
	int liczba;
	char tablica[2];
};

/* funkcja przyjmująca wskaźnik do struktury */
void funkcja_4 (dane *a) {
	/* to jest to samo co: double funkcja_3 (struct struct_dane *a) i wtedy nie potzrzebujemy typedef */
	printf( "mamy dwa sposoby odwołania się do elementów struktury przekazanej przez wskaźnik:");
	printf( " %d == %d\n", a->liczba, (*a).liczba );
	printf( "i dwa sposoby odwołania się do adresów elementów tej struktury:");
	printf( " %p == %p\n", &(a->liczba), &((*a).liczba) );
}



/* funkcja startowa przyjmująca argumenty wywołania programu (tablica argv)
   ich liczbę (argc) a także zmienne środowiskowe (tablica envp)
   oraz zwracającą kod powrotu 0 - sukces, inne - porażka */
int main (int argc, char *argv[], char *envp[]) {

printf("\nWSKAZNIKI\n\n");

	int a=5; /* zmienna całkowita o wartości 5 */
	int *wsk=0; /* wartością wsk jest adres w pamięci ... (w tym wypadku wskazuje na adres zmiennej typu integer),
	               jeżeli void *.... to nie określamy (nie wiemy jeszcze na jakiego typ danych on wskazuje) */

	wsk=&a;
		/* do naszego wskaźnika wpisujemy adres (& oznacza operację pobrania adresu zmiennej) */

	printf("Wartość a wynosi: %d \n", a);
		/* wypisywanie - zobacz: man 3 printf */
	printf("Wartość pod wskaznikiem: %d \n", *wsk);
		/* * oznacza operator wyluskania, pobiera wartość zmiennej z pod podanego adresu */
	printf("Adres zmiennej a %p \n", wsk);

	// STAŁOŚĆ WSKAŹNIKÓW
	const int* wskaznik_na_stala=wsk;
	// takiemu wskaźnikowi możemy zmienić zmienną na którą pokazuje,
	// ale nie możemy zmienić wartości tej zmiennej
	
	int* const wskaznik_o_stalym_adresie=wsk;
	// takiemu wskaźnikowi nie możemy zmienić zmiennej na którą pokazuje
	// ale przy jego pomocy możemy zmieniać wskazywaną zmienną
	
	const int* const wskaznik_o_stalym_adresie_na_stala=wsk;
	// temu wskaźnikowi nie możemy zmienić adresu ani przy jego pomocy zmienić wartości wskazywanej
	

printf("\nTABLICE A WSKAŹNIKI\n\n");

	/* tablica to w rzeczywistości wskaźnik do pierwszego jej elementu ... */
	int tablica[5]; /* zadeklarowaliśmy tablicę o pięciu elementach typu int (indeksowanych 0 ... 4) */
	tablica[0]=50; tablica[1]=51; tablica[2]=52; tablica[3]=53; tablica[4]=54;

	wsk=tablica;
		/* do naszej zmiennej wskaźnikowej wpiszemy adres naszej tablicy (nie potrzebujemy tutaj używać &) */

	for (a=0; a<5; a++) {
		printf("Element numer a=%d, wypisany *(tablica+a) wynosi: %d\n", a, *(tablica+a));
		printf("                    i jest to to samo co tablica[a]: %d\n", tablica[a]);
		printf("                    jak również wypisanie *(wsk++): %d\n", *(wsk++));
		printf("         jego adres to: %p\n", wsk);
	}


printf("\nFUNKCJE I WSKAZNIKI\n\n");

	int z = 0;
	printf("Po zainicjowalizowaniu zmiennej: %d\n", z);

	funkcja_1(&z);
	printf("Po wykonaniu funkcji: %d\n", z);

printf("\nFUNKCJE I TABLICE\n\n");

	funkcja_2(tablica); /* do tej funkcji przekaże tablicę - kompilator zamieni to na przekazanie wskaźnika */
	printf("po naszej kolejnej funkcji wartość tablica[2] wynosi: %d %d\n", tablica[2], 2[tablica]);
	// zaskakująca postać 2[tablica] jest równoważna
	// bo operator a[b] jest tak naprawdę rozumiany jako *(a+b)


printf("\nFUNKCJE I FUNKCJE\n\n");

	double x;
	x = funkcja_3( suma );
		/* jako argument funkcja_3 przekazujemy nazwę funkcji - suma, a zwracaną wartość wpisujemy do x */
	printf("Po wykonaniu naszych funkcji mamy: %f\n", x);


printf("\nTABLICE DWUWYMIAROWE\n\n");

	/* deklarujemy tablicę dwuwymiarową i ją wypełniamy */
	int tablica_dwuwymiarowa[2][3];
	tablica_dwuwymiarowa[0][0]=0; tablica_dwuwymiarowa[0][1]=1; tablica_dwuwymiarowa[0][2]=2;
	tablica_dwuwymiarowa[1][0]=10; tablica_dwuwymiarowa[1][1]=11; tablica_dwuwymiarowa[1][2]=12;

	/* przypisujemy jej adres do naszego wskaźnika
	   tym razem użyliśmy * jest to podyktowane tym iż tak naprawdę tablica dwuwymiarowa to tablica tablic,
	   co zatem idzie pod adresem wskazywanym przez nazwę tablicy jest wskaźnik do tablicy odpowiadającej
	   pierwszemu wierszowi (a zarazem pierwszemu elementowi w tym wierszu)*/
	wsk = *tablica_dwuwymiarowa;

	printf("Wywolanie tablica_dwuwymiarowa[i][j] daje: %d\n", tablica_dwuwymiarowa[1][2]);
	printf("     i to samo daje: wsk[i*SZEROKOSC+j]: %d\n", wsk[1 * 3 + 2]);
		/* taki sposób odwołania przydaje się przy przekazywaniu do funkcji tablic dwuwymiarowych
		   gdzie SZEROKOSC to ilość kolumn - zakres drugiego argumentu 
		
		to działa dlatego że zazwyczaj i tak całość takiej tablicy jest alokowana w jednym kawałku,
		ale w ogólności należy uważać bo kolejne wiersze tablicy wielowymiarowej
		nie muszą składać się na ciągły obszar pamięci ... */


printf("\nTABLICE WSKAZNIKOW i WSKAZNIKI DO WSKAZNIKOW\n\n");

	int *wskazniki[2];
	wskazniki[0] = tablica;
	wskazniki[1] = &a;

	printf("Element trzeci tablicy siedzącej pod wskazniki[0] to: %d\n", wskazniki[0][3]);
	printf("Ale trzeba UWAŻAĆ bo odwołanie do trzeciego elementu wskazniki[1] nie ma sensu: %d\n", wskazniki[1][3]);
	printf("  sens ma natomiast *wskazniki[1]: %d ale nie wskazniki[1]: %p\n", *wskazniki[1], wskazniki[1]);
	printf("  podobnie *wskazniki[0]: %d i wskazniki[0]: %p\n", *wskazniki[0], wskazniki[0]);
	printf("  natomiast sens posiada **wskazniki (czyli wskaźnik do wskaźnika, wyłuskujący");
	printf("    zawartość komórki której adres zapisany jest pod adresem w wskaźniki): %d\n", **wskazniki);

	/* wskaźniki do wskaźników można tez po prostu deklarować */
	int **wskwsk;
	wskwsk = wskazniki + 1;
	printf("Natomiast po podwójnym wyłuskaniu wskwsk = wskazniki + 1;, gdzie int **wskwsk; mamy: %d\n", **wskwsk);
	printf("  czyli to samo co *wskazniki[1]\n");
	

printf("\nSTRUKTURY i WSKAŹNIKI DO NICH\n\n");

	dane moja_struktura;
	moja_struktura.liczba = 23;
	funkcja_4(&moja_struktura);
	printf("Co trochę może dziwić ale sama nazwa struktury nie jest traktowana jak wskaźnik");
	printf(" lecz jak wyłuskanie pierwszego jej elementu: %d\n", moja_struktura);

	/* w tym miejscu należy trochę przyszłościowo wspomnieć o wskaźnikach do klas i metod w C++,
	 jak już wiadomo wskaźnik zna rozmiar elementu na który wskazuje - dzięki temu działa arytmetyka wskaźnikowa,
	 ale mechanizm ten może być zdradliwy przy dziedziczonej hierarchii klas - możemy używać wskaźnika na klasę
	 bazową celem odwoływania się do klas pochodnych, ale taki wskaźnik ma nieprawdziwą wielkość klasy
	 (ma wielkość klasy bazowej a nie pochodnej) poza tym w przypadku dziedziczenia zdradliwe może być
	 też porównywanie wartości wskaźników celem sprawdzenia czy wskazują na ten sam obiekt */


	return 0; /* zwracam kod powrotu i zakańczam wykonywanie programu */
}
