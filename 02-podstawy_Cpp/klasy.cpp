/** plik prezentuje podstawy programowania obiektowego w C++ :
  *  deklaracje klasy
  *  "zasiegowość zmiennych"
  *  dziedziczenie
  *  deklarowanie konstruktorów i destruktorów
  *  przeciążanie funkcji (na przykładzie konstruktora)
  *  funkcje wirtualne
  **/

#include <stdio.h>
#include <iostream>
#include <memory>

using namespace std;

/// definicja klasy
class klasa1 {
	private:
		char prywatna; // ta zmienna nie będzie dostępna z zęwnątrz klasy
		// (chyba że z klasy lub funkcji zadeklarowanej w tej cześci jako zaprzyjaźniona:
		//  friend class NAZWA_KLASY )
	public:
		/// konstruktory i destruktor
		klasa1() {
			prywatna = 1;
			publiczna = 6;
		} // od razu tutaj tez można umieszczać ciało funkcji
		klasa1(char t);
		// możemy definiować kilka konstruktorów różniących się argumentami
		// (podobnie jak z zwykłymi funkcjami, także poza klasami - przeciążanie funkcji ...)
		~klasa1(){
			cout << "to byl destruktor" << endl;
		}
		
		// klasa zawsze posiada (niezależnie od tego czy jawnie go zdeklarujemy czy nie):
		//  - konstruktor bez argumentowy
		//  - konstruktor kopiujący (przyjmujący obiekt tej klasy jako argument)
		//  - destruktor
		//  - operator przypisania
		
		/// zmienne
		int publiczna; // tutaj nie można przypisać wartości dlatego robi się to w konstruktorach
		static int wspolna; // ta zmienna będzie wspólna dla wszystkich obiektów tej klasy
		static const int stala = 7; // to będzie stała,
			// aby nie tworzyć jej kopi w każdej instancji klasy deklarujemy ją jako static
		
		/// funkcje
		int funkcja1(int a, int b);
		double funkcja2(double x);
		// możemy deklarować funkcję o tej samej nazwie różniące się typami / liczbą argumentów
		int funkcja2() {
			return 0;
		};
		virtual int funkcja3() {
			return 1;
		};
		
		// poniższa funkcja4 jest metodą która może być wywołana dla obiektów tej klasy będących "const"
		const int& funkcja4() const {
			cout << "to było dla const" << endl;
			return publiczna;
		};
		// a to jej odpowiednik dla obiektów nie będących const
		int& funkcja4() {
			cout << "to było nie dla const" << endl;
			
			// zamiast przepisywać kod możemy tutaj wywołać metodą dla obiektów const
			return const_cast<int&>( static_cast<const klasa1*>(this)->funkcja4() );
			
			// przy okazji warto wspomnieć o operatorach rzutowania w stylu C++
			// w C++ mamy 4 operatory rzutowania:
			//  - static_cast - pomiędzy typami spokrewnionymi
			//  - dynamic_cast - w dół hierarchii klas
			//  - const_cast - zniesienie const (chyba jedyny racjonalny przypadek pokazany wyżej)
			//  - reinterpreter_cast - dowolna konwersja
			//      (dzięki niemu możemy tak jak w C rzutować dowolny obiekt na dowolny)
			// używa się ich operator<typ_na_kory_zutujemy>(obiekt_ktory_rzutujemy)
		};
};

/// deklaracja zmiennej statycznej z nadaniem jej wartości - jest to niezbędne aby została ona utworzona
/// (o ile definicja klasy taka jak powyżej często znajduje się w pliku nagłówkowym włączanym w wielu plikach .cpp to
///  deklaracja takiej zmiennej statycznej powinna być tylko w jednym z tych plików .cpp)
int klasa1::wspolna = 0;

/// deklaracja funkcji znajdujących się w naszej klasie (metod)
/// jest to funkcja z wartościami domyślnymi (coś takiego nie przejdzie w C !!!)
int klasa1::funkcja1(int a = 0, int b = 0) {
	return a + b;
}
double klasa1::funkcja2(double x) {
	return x/prywatna;
}
// konstruktor
klasa1::klasa1(char t) {
	prywatna = t;
	publiczna = 6;
}



/// definicja klasy potomnej dla klasa1 (dziedziczącej z klasa1)
///
/// klasa ta dziedziczy publicznie czyli publiczne metody klasa1 będą publicznymi w klasa2
/// możliwe jest dziedziczenie private i protected (dostępne tylko dla dziedziczących)
///
/// poza tym warto wspomnieć o przydatnym gdy chcemy korzystać z dziedziczenia wielobazowego
/// po kilku klasach mających wspólnego przodka dziedziczeniu wirtualnym (gdy tak będziemy
/// definiowali dziedziczenie po wspólnym przodku to w klasie dziedziczącej wielokrotnie
/// po jego potomkach będzie on włączony jednokrotnie)
class klasa2 : public klasa1 {
	public:
		static int funkcja1(int a = 1, int b = 1) {
			return a * b;
		}
		virtual int funkcja3() {
			return 2;
		}
		int funkcja2() {
			return 3;
		}
		// aby nie nadpisało to "przeciążonych" funkcji o tej nazwie nalezy dać
		using klasa1::funkcja2;
		
		klasa2(char t);
		klasa2();
		
		// deklarowanie funktora
		void operator()(int i, int j) {
			printf("%d\n", i + j);
		}
};

/// dziedziczymy konstruktory
klasa2::klasa2(char t):klasa1(t) {}
klasa2::klasa2():klasa1() {}


/// przeciążenie operatora
int operator+ (klasa2 & kl, int liczba) {
	// tutaj argumenty pobrane zostały przez referncje ...
	// działa to podobnie do wskaźników przy czym adres przypisujemy tylko raz
	// a korzystamy bez wyłuskiwania wartości (jak z zmiennej a nie wskaźnika)
	// operujemy jednak na oryginalnej zmiennej a nie jej kopii (!!!)
	return liczba + kl.publiczna;
}

/// funkcja glowna programu
int main() {
	// funkcja statyczna jest dostępna nawet gdy nie ma żadnego obiektu danej klasy
	int wynik = klasa2::funkcja1(4,8);
	printf("WYNIK: %d\n", wynik);
	
	// deklaracje obiektów naszej klasy klasa1
	klasa1 k11;
	klasa1 k12 = * new klasa1(2);
	// k12 zostanie utworzona na stercie (a k11 na stosie)
	// w tym miejscu należy także wspomnieć o operatorach new i delete dla tablic,
	// które powinny być stosowane w C++ zamiast malloc'a:
	// char* tabliczka = new char[20]
	// w ten sposób zaalokowalibyśmy na stercie 20 bajtów
	// należy zaznaczyć że obiekty takie należy zwalniać przez
	// delete [] tabliczka;
	// operatory te mogą być stosowane też do bardziej złożonych obiektów
	// - wołają one odpowiednio konstruktory i destruktory tworzonych obiektów
	
	// k11.prywatna=3; - tak nie można :-) bo prywatna
	
	cout << "funkcja2: " << k11.funkcja2(31) << " " << k12.funkcja2(31) << endl;
	
	k11.publiczna = 7;
	cout << "publiczna: " << k11.publiczna << " " << k12.publiczna << endl;
	
	k11.wspolna = 3;
	cout << "wspolna: " << k11.wspolna << " " << k12.wspolna << endl;
	k12.wspolna++;
	cout << "wspolna: " << k11.wspolna << " " << k12.wspolna << endl;
	
	
	/// wskaźnik do klasy
	klasa1 * k1_wsk = &k11;
	cout << (*k1_wsk).funkcja1(11,56) << endl;
	
	/// sprytne wskaźniki
	auto_ptr<klasa1> k1_auto_wsk(new klasa1(22));
	cout << "funkcja2: " << k1_auto_wsk->funkcja2(100) << endl;
	// jak widać używa się ich jak normalne wskaźniki
	// różnica polega na tym iż utworzony obiekt typu klasa1 jest skojarzony
	// z tym wskaźnikiem i zostanie automatycznie usunięty przy usunięciu wskaźnika
	// dzięki czemu obiekt zostanie automatycznie usunięty gdy wyjdziemy z zasięgu w którym dostępny był wskaźnik
	//
	// wskaźniki auto_ptr ze względu na swoje zachowanie przy kopiowaniu
	// są niebezpieczne gdy przekazujemy je do funkcji poprzez wartość lub przechowujemy w kolekcji ...
	// rozwiązaniem tego problemu jest stosowanie shared_ptr i week_ptr
	// shared_ptr posiada licznik "kopiowań" dzięki czemu usunięty zostanie
	// dopiero gdy przestaniemy korzystać z ostatniej jego kopii
	
	/// dziedziczenie
	klasa2 k21;
	cout << "funkcja1: " << k21.funkcja1() << endl;
	
	klasa2 * k22 = new klasa2(10);
	cout << "funkcja2: " << k22->funkcja2(100) << endl;
	// tak tez można mieć wskaźnik do klasy
	// warto także zauważyć że zamiast (*k22). mozemy uzywac k22->
	
	cout << "jak widzimy funkcja2 zostala odziedziczona a funkcja 1 nadpisana ..." << endl;
	
	// wywołanie obiektu jak funkcji - dzięki zdefiniowaniu operatora ()
	k21(3, 4);
	
	
	/// wskaźnik typu klasa1 może wskazywać na obiekty pochodne od tej klasy
	k1_wsk = k22;
	// ale w takim wypadku nie działa zwyczajne nadpisywanie funkcji
	cout << "funkcja1: " << k1_wsk->funkcja1() << " zachowuje sie jak z klasa1" << endl;
	// jednak dziala nadpisywanie gdy funkcje są deklarowane jako virtual
	cout << "funkcja3: " << k1_wsk->funkcja3() << " zachowuje sie jak z klasa2" << endl;
	
	
	cout << "przeciazony operator: " << k21 + 12 << endl;
}

/* na koniec wspomnę o paru ciekawszych rozwiązaniach związanych z
   wykorzystaniem programowania obiektowego:
   * mechanizm klasy możemy użyć celem opakowania wywołania jakiejś
     funkcji wraz przekazywanymi do niej argumentami
     (w tym celu w klasie umieszczamy pole identyfikatora akcji,
     pola argumentów wywoływanej funkcji oraz metodę związaną z
     jej wykonaniem; dzięki temu że możemy tworzyć kolekcje obiektów
     takiej klasy możemy m.in. kolejkować zadania do wykonania,
     jak również zapamiętywać historię wykonanych poleceń - opcja undo),
     patrz wzorzec projektowy komendy
   * mechanizm dziedziczenia możemy wykorzystać do próby dynamizacji typów -
     operując klasa bazowa możemy w jakiś sposób wytworzyć (zależnie od
     potrzeb) jakiś obiekt którejś z klas pochodnych (o typie tworzonego
     obiektu mogą decydować czynniki nieznane w trakcie kompilacji - np.
     wartość odczytana z pliku), do wyboru mamy kilka sposobów
     implementacji tego typu mechanizmu - patrz wzorce projektowe fabryk
     obiektów oraz http://pl.wikipedia.org/wiki/RTTI
*/
