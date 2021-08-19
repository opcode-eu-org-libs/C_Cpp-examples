/*
 * w napisy.cpp przedstawiłem już podstawy korzystania z <string> i <sstream>
 * w tym programie przedstawię kilka kolejnych elementów biblioteki standardowej C++
 *
 * zobacz także:
 *  http://pl.wikipedia.org/wiki/Standard_Template_Library
 *  http://pl.wikibooks.org/wiki/C++#Dodatek_A:_Biblioteka_STL
 */

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include <string>
#include <functional>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;

void wypisz(pair<string, string> p) {
	printf("YY %s : %s\n", p.first.c_str(), p.second.c_str());
}

void wypisz2(int a, int b) {
	printf("QQ %d : %d\n", a, b);
}

int sum(int a, int b) {
	return a+b;
}

struct Porownania {
	int a;
	int b;
	bool operator<(const Porownania &p) { return a < p.a; };
	bool operator>(const Porownania &p) { return a > p.a; };
};

int main() {
	int i=0;
	
	// uwaga: wszystkie kontenery STL operują na kopiach obiektów do nich wstawianych
	// wszystkie konternery mogą zawierać dowolny typ - tak wbudowany jak i definiowany
	// przez użytkownika, dla działania funkcji sortujących konieczne może być zapewnienie
	// operatora większości dla takiego typu
	
	/// dynamiczna tablica czyli "vector"
	// uwaga: powiększanie vectora może doprowadzić do konieczności jego przeniesienia w pamięci
	// operacja ta związana jest z kopiowaniem wszystkich elementów wektora w nowe miejsce
	// operacja taka jest kosztowana czasowo
	// dlatego lepiej aby vector przechowywał wskaźniki na duże elementy a nie duże elementy
	// warto także przed rozpoczęciem wypełniania vectora określić rozmiar
	// jaki chcemy na niego zarezerwować przy pomocy metody reserve
	
	// wskaźnik na element wektora może stracić aktualność na sklutek usunięcia elementów
	// wcześniejszych niż wskazywany lub wspomnianej wyżej realokacji wektora
	// podobnie rzecz ma się z innymi kontenerami bazującymi na tablicach
	
	// tablica 7 liczb całkowitych wypełniona jedynkami
	vector<int> tablica(7,1);
	
	// iterator (służy do wskazywania miejsca w tablicy)
	vector<int>::iterator iter;
	
	// wstawiamy przed 3 (pierwszy +2) elementem wartość 33
	iter = tablica.begin();
	iter += 2;
	tablica.insert(iter, 1, 33);
	// uwaga powoduje to przekopiowanie wszystkich elementów od iter włącznie
	
	// zmieniamy wartość 6 elementu na 7
	tablica[5]=7;
	// zwiększamy wartość 8 (w chwili obecnej ostatniej ostatniego, bo 7 początkowych + 1)
	// i wypisujemy ten element
	printf("XX: %d\n", ++tablica[7]);
	
	// wstawiamy na końcu
	tablica.push_back(15);
	
	// usuwamy element 7
	iter = tablica.begin();
	tablica.erase(iter + 6);
	// uwaga powoduje to przekopiowanie wszystkich elementów od iter włącznie
	
	// wypisanie całej tablicy
	for (; iter != tablica.end(); ++iter) {
		printf("%d : %d\n",i++,*iter);
	}
	
	// wskaźnik na początkowy i końcowy element tablicy
	// uwaga wartości wskaźników do elementów mogą być nietrwałe - przekopiowywanie elementów
	int *ref_f = &(tablica.front());
	int *ref_b = &(tablica.back());
	printf("XX: %p %p %ld\n", ref_f, ref_b, ref_b-ref_f+1);
	
	// gdy nie pusty
	if ( ! tablica.empty() )
		// wypisz aktualny, maksymalny bez przenoszenia w inne miejsce,
		// maksymalny wogole rozmiar
		printf("%ld %ld %ld\n", tablica.size(), tablica.capacity(), tablica.max_size());
	
	// usuwamy ostatni element
	tablica.pop_back();
	
	// usuwamy wszystkie elementy
	tablica.clear();
	
	// oczywiście są także inne przydatne metody tego obiektu ...
	
	/// kolejka o dwóch końcach - deque
	// jest to kontener umożliwiający wydaje wstawianie elementów na koniec i na początek
	// implementowany jest jako zbiór tablic, dzięki czemu dodanie elementu nigdy nie powoduje
	// przepisywania całości, poza tym bardzo przypomina vector
	
	
	
	/// listy czyli "list"
	list<int> lista;
	
	// wstawiamy coś na listę (można wstawiać także na wybraną pozycję - jak w vector)
	lista.push_back(30);
	lista.push_back(20);
	lista.push_back(29);
	lista.push_back(40);
	lista.push_back(-50);
	lista.push_back(20);
	// można też z przodu (a także gdzieś w środku - insert jak w vector)
	lista.push_front(29);
	
	// usuwamy z listy elementy równe 29
	lista.remove(29);
	
	// sortujemy naszą listę (można także określić funkcję zadającą porządek sortowania)
	lista.sort();
	
	// usuwamy występujące koło siebie duplikaty
	lista.unique();
	
	// odwracamy listę
	lista.reverse();
	
	// wypisujemy naszą listę
	// do elementów listy możemy odwoływać się tylko przez iteratory (nie da się przez indeks)
	for(list<int>::iterator iter=lista.begin(); iter != lista.end(); ++iter)
		printf("%d ",*iter);
	puts(" << normalna");
	
	for(list<int>::reverse_iterator iter=lista.rbegin(); iter != lista.rend(); ++iter)
		printf("%d ",*iter);
	puts(" << odwrocona");
	// zastosowanie rbegin i rend jest wygodniejsze niż próba użycia w tym celu pętli
	// od end do begin gdyż begin wskazuje na pierwszy element, a end na pierwszy
	// za ostatnim (są one niesymetryczne co utrudnia takie proste odwrócenie)
	
	
	// oczywiście są także inne przydatne metody tego obiektu ...
	// możemy jeszcze m.in. usuwać ciągi elementów bądź przenosić je w ramach listy ...
	
	/// listy jednokierunkowe "slist"
	// odmienny wariant list (możemy tylko zwiększać iteratory)
	// zajmują mniej pamięci i są szybsze za wyjątkiem  insert, erese i splice
	// (bo działają na elemencie przed podanym,
	//   dlatego dostępne dla tych list są szybsze odpowiedniki - metody *_after)
	
	
	
	/// tablice haszujące (asocjacyjna) - STL map
	map<string,string> tab_asoc;
	tab_asoc["Ala"]="ma kota";
	tab_asoc["kot"]="ma Ale";
	
	printf("Ala %s\n", tab_asoc["Ala"].c_str());
	
	
	
	/// inne kontenery
	// w ramach STL dostępne są też inne obiekty - mniej lub bardziej podobne do przedstawionych powyżej, np:
	//  * unordered_map – mapy nie sortujące
	//  * set – zbiór unikalnych elementów (taka mapa bez wartości)
	//  * array – adapter dla tablic z C dostarczający metod typowych dla kolekcji STL
	//  * tuple – "rozszerzone pair" umożliwiające przechowywanie zestawu wielu elementów, a nie tylko pary
	
	
	/// algorytmy
	// dostępne są także często stosowane wzorce funkcji operujące na kontenerach zwane "algorytmami STL"
	// należy jednak pamiętać że jeżeli możemy skorzystać z metody kontenera lub z algorytmu
	// należy wybrać to pierwsze, gdyż ta implementacja ma duże szanse być sporo wydajniejsza
	
	printf("%d %d\n", min(10, 30), min(30, 10));
	
	// inną przydatną funkcją jest for_each() powodujący wykonywanie podanej
	// funkcji na elementach pomiędzy dwoma iteratorami
	for_each(tab_asoc.begin(), tab_asoc.end(), wypisz);
	
	// warto wspomnieć też o samym std:pair użytym w funkcji wypisz:
	wypisz( pair<string,string>("AA", "BB") );
	// jest to szablon łączący dwa obiekty zadanych typów w parę ... (taki jednoelementowy map)
	
	// przy korzystaniu z algorymów często zachodzi potrzeba dostosowania wywołania istniejącej funkcji
	// lub metody do wymaganego przez algorytm, zadanie to ułatwia użycie std::bind
	bind(wypisz2, placeholders::_2, placeholders::_1)(1, 3);
	bind(wypisz2, placeholders::_1, placeholders::_1)(1, 3);
	(bind(wypisz2, 10, placeholders::_1))(1, 3);
	// wewnątrz możemy podstawiać kolejne funkcje przy pomocy bind'a ...
	// ponadto zawsze możemy wskazać jawnie typy argumentów funkcji o jaką nam chodzi ...
	bind(wypisz2, placeholders::_3, bind(sum, placeholders::_1, placeholders::_2))(1, 3, 8);
	
	// a teraz praktyczny przykład z sortowaniem wektora
	vector<Porownania> pors(2);
	pors[0].a=4; pors[0].b=10;
	pors[1].a=13; pors[1].b=2;
	
	//sort(pors.begin(), pors.end());
	printf("posortowane po a: %d %d\n", pors[0].a, pors[1].a);
	
	sort(pors.begin(), pors.end(), bind(std::less<int>(), bind(&Porownania::b, placeholders::_1), bind(&Porownania::b, placeholders::_2)) );
	// zamiast opratora < klasy Porownania (który operuje na polach a)
	// używamy tutaj szablonu funkcji less dostarczanego przez bibliotekę standardową,
	// który jako argumenty otrzymuje pola b obiektów klasy Porownania
	// (dzięki zbindowaniu &Porownania::b zwracającego pole b z placeholders::_1 / placeholders::_2,
	//   pod który podstawiane są porównywane obiekty z wektora pors (obiekty klasy Porownania))
	printf("posortowane po b: %d %d\n", pors[0].a, pors[1].a);
	
	// nieco podobnym do bind rozwiązaniem jest lambda
	// umożliwia on tworzenie nienazwanych obiektów funkcyjnych
	// poprzez ich definiowanie "w miejscu użycia"
	cout << "Lista: ";
	for_each(lista.begin(), lista.end(), [](int x) {cout << x << ' ';} );
	cout << endl;
	
	// powyższe sortowanie po polu b możemy zapisać także z użyciem lambdy:
	sort(pors.begin(), pors.end(), [](const Porownania& x, const Porownania& y) { return x.b > y.b; } );
	// użyłem sortowani w odwrotnej kolejności, > zamiast <, aby pokazać że to działa)
	printf("posortowane po b (malejąco): %d %d\n", pors[0].a, pors[1].a);
	
	
	// z innych ciekawych tego typu rzeczy warto wspomnieć o std::function implementującym wzorzec
	// komendy (przechowywanie wskaźnika na funkcję w formie obiektu) dzięki niemu możliwe jest
	// przechowywanie w kolekcjach STL ciągów wywołań funkcji (np. dla funkcji "undo")
	
	// należy także wspomnieć o bibliotece boost (która zawiera kandydatów na przyszłe elementy biblioteki standardowej)
	// boost dostarcza m.in. kontenery takie jak:
	//  * variant - "unia dla obiektów" (klasyczna unia z C nie może przechowywać obiektów klas) [od C++17 w std::]
	//  * optional - oprócz wartości może mieć też stan "niezainicjalizowany" [od C++17 w std::]
	//  * any - taki odpowiednik dla void* [od C++17 w std::]
	//  * tribool - logika trójwartościowa
	//  * multi_array - tablice wielowymiarowe
	// jest także biblioteka boost do obsługi grafów - Boost Graph Library
	
	
	// wracając do algorytmów należy jeszcze wspomnieć przynajmniej o:
	// - find – zwraca iterator do pierwszego elementu spełniającego warunek)
	// - count – zwraca liczbę elementów spełniających warunek
	// - remove – usuwa wskazane elementy (często w istocie przenosi pozostałe elementy na początek)
	// - partition – dzieli zbiór na spełniające i niespełniające warunek
	// wszystkie funkcje sortujące bez static_ na początku mogą zmieniać kolejność elementów
	// o tym samym porządku sortowania - spełniających ten sam warunek (np. elementów równych)
	
	// w ramach STL zaimplementowane są różnego typu algorytmy
	// odsyłam do: https://en.cppreference.com/w/cpp/algorithm
	// lub do: http://pl.wikibooks.org/wiki/C++/Algorytmy_w_STL
}
