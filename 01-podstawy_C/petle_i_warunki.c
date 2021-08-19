/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje pętle, instrukcje warunkowe itp.
 *
 */


/* podstawowymi instrukcjami wykorzystywanymi w programowaniu są pętle i instrukcje warunkowe;
   podobnie jak podstawowe operacje matematyczne zamieniane są one niemalże bezpośrednio na
   instrukcje procesora (w odróżnieniu od wielu innych instrukcji nie są wywołaniami bibliotecznymi
*/

#include <stdio.h>

int main() {
	int i = 0;


/// instrukcje iteracyjne (pętle) - powodują wielokrotne wykonywanie tego samego fragmentu programu

	/// pętla while
	while ( i<4 ) {
		printf("%d", i++);
		// i++; jest (niemalże) równoważne zapisowi i = i+1; czyli zwiększeniu i o 1
		// można by próbować wykonać to też w taki sposób:
		// printf("%d", i=i+1);
		// jednak wtedy program wypisałby 1234 (zamiast 0123), co oznacza iż najpierw
		// i zostało zwiększone o 1 a potem podstawione; jest to równoważne instrukcji:
		// printf("%d", ++i);
		// mamy też podobne operatory zmniejszania o jeden
		//  i-- (podstaw i zmniejsz o jeden) --i (zmniejsz o jeden i podstaw)
	}
	// jak nietrudno zauważyć pętla ta wykonuje następującą po niej instrukcje
	// (bądź blok instrukcji ujęty w nawiasy klamrowe)
	// dopóki warunek podany na początku jest prawdziwy
	
	printf("\n");
	
	/// pętla do
	do {
		printf("%d", i++);
	} while ( i<4 );
	// pętla ta wykonuje instrukcję również dopóki warunek jest prawdziwy,
	// jednak jest on sprawdzany dopiero po wykonaniu bloku instrukcji - zatem pętla ta
	// wykona się zawsze przynajmniej raz (jak widać powyżej),
	// natomiast gdy startujemy z warunku prawdziwego jest równoważna pętli while
	
	printf("\n");
	
	/// pętla for
	for (i=0; i<4; i++) {
		printf("%d", i);
	}
	// wykonuje się ona bezpośrednio iterując jakiś indeks i jak widać powyższa
	// konstrukcja równoważna jest powyższemu while
	
	printf("\n");


/// instrukcje warunkowe - instrukcje te uzależniają wykonanie fragmentu kodu od zadanego warunku,
/// jednak w odróżnieniu od pętli fragment ten wykonywany jest tylko raz

	/// instrukcja if
	if (i == 4) {
		printf("%d", i++);
	} else {
		printf("!");
	}
	
	if (i == 4) {
		printf("%d", i++);
	} else {
		printf("!");
	}
	// instrukcja ta (printf("%d", i++);) wykona się tylko gdy warunek jest prawdziwy
	// (w tym wypadku i równa się 4), w przeciwnym razie wykona się blok instrukcji
	// zawarty po else (printf("!");), jako że w pierwszym wykonaniu zwiększyliśmy
	// i to za drugim razem zostanie wypisany wykrzyknik
	
	// oczywiście możliwe są różne inne warianty instrukcji if:
	// bez else:
	// if() {}
	// z wieloma if else
	// if() {} else if() {} else if() {}
	// z wieloma if else i jednym else
	// if() {} else if() {} else if() {} else {}
	
	printf("\n");
	i=1;
	
	/// instrukcja switch
	switch (i) {
		case 0:
			printf("wykonałem dla 0\n");
			break; // przerywamy switch
		case 1:
			printf("wykonałem dla 1\n");
			i = 3;
			// tu nie ma break; więc wykonujemy dalszy ciąg instrukcji
			// (bez względu na wartość i)
		case 2:
		{
			printf("wykonałem dla 2\n");
			break;
		}
		// uwaga jeżeli w ramach swich'a chcemy deklarować zmienne należy ująć
		// całość instrukcji wykonywanych w danym case w klamry ...
		// jak uczyniłem to w tym punkcie ...
		case 3:
			printf("wykonałem dla 3\n");
			break;
		default:
			// ta sekcja wykonana byłaby gdyby na wstępie zapodać np. i=5 (zamiast i=1)
			printf("było cos innego niż 0 1 2 3\n");
		//
	}

	/* słów parę o warunkach jakie możemy sprawdzać:
	oczywiście oprócz warunku równości (==) możemy też sprawdzać wiele innych rzeczy:
	a<b - a mniejsze od b
	a>b - a większe od b
	a>=b - a nie mniejsze od b
	a<=b - a nie większe od b
	a!=b - a różne od b
	mamy też OR oraz AND logiczne - odpowiedni || oraz &&
		(warunki te sprawdzane są od lewej do prawej i jeżeli wartość logiczna całości
		jest znana (np. prawda OR coś jest zawsze prawdą) sprawdzanie jest przerywane
		- uwaga na wykonywanie funkcji i przypisań pod if, while, itd.)
		możemy też sprawdzać bezpośrednio równość z zerem
	*/
	if (0) printf("to się nie wypisze\n");
	if (1) printf("a to tak\n");
	if (2) printf("a to ???\n");
	// oczywiście zamiast wpisanego na sztywno zera czy jedynki umieszcza się tam zazwyczaj
	// jakąś funkcję zwracająca takie wartości ...


/// instrukcje skoków - służą do przemieszczania się w programie ...

	/// break
	for (i=0; i<4; i++) {
		printf("%d", i);
		if (i==1) break;
	}
	// wykonanie break; spowoduje opuszczenie pętli (zawsze tylko najbliższej
	// - najbardziej wewnętrznej pętli w której został użyty)
	
	printf("\n");
	
	/// continue
	for (i=0; i<4; i++) {
		if (i==1) continue;
		printf("%d", i);
	}
	// spowoduje rozpoczęcie kolejnej iteracji tej pętli
	// (pominiecie instrukcji pomiędzy nim  końcem bloku instrukcji wykonywanych w pętli
	
	printf("\n");
	
	/// goto - powodująca przeskok do określonej etykiety
	/// (powszechnie zaleca się jak najrzadsze wykorzystywani tej instrukcji)
wypisz_i:	// etykieta wypisz_i
	printf("%d", i);
	
	if (i !=0 ) {
		i=0;
		goto wypisz_i; // spowoduje skok do etykiety wypisz_i
	}
	
	printf("\n");
	
	// return - kończąca działanie funkcji w której została wykonana
	// i zwracająca wartość zadana wartość
	return i;
	// kończymy działanie funkcji main() a więc i całego programu zwracając
	// wartość zmiennej i (u nas będzie to zero - co oznacza że wszystko było OK)
	// to co zwrócił program możemy sprawdzić wpisując w terminalu (na którym działał)
	// po jego wykonaniu: echo $?
	
	// w miejscu tym należy także wspomnieć o możliwości wykonywania tzw. długich skoków
	// polegają one na tym iż z dowolnego miejsca w programie możemy wykonać funkcję 
	// longjmp(bufor, kod) powoduje ona przeskok do miejsca w którym ostatni raz wywołana była
	// setjmp(bufor) i rozpoczęcie wykonywania programu tak jakby zwróciła ona niezerową
	// wartość podaną w kod
	//
	// działanie tego mechanizmu polega na tym iż wywołanie setjmp(bufor) powoduje
	// zapamiętanie aktualnego stanu stosu w buforze i zwrócenie przez tą funkcję zera
	// natomiast longjmp(...) powoduje odtworzenie stanu stosu z bufora i zasymulowanie
	// zwrócenia przez setjmp niezerowej wartości
	//
	// należy pamiętać iż jest to niskopoziomowy mechanizm, który nie usuwa obiektów
	// dynamicznych tworzonych na stercie, anie nie wywołuje destruktorów
}
