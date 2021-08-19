/** plik prezentuje zagadnienia z programowania w C++ :
 *  szablony
 *  przestrzenie nazw
 *  wyjątki
 **/


#include<iostream>
#include<limits>

using namespace std;

/// deklarujemy szablon klasy
// szablon może mieć kilka parametrów oraz parametry z wartością domyślną (opcjonalne)
template <typename T1, typename T2 = int> class klasa3 {
	T1 val; // gdy korzystamy z class domyslnie wszystko jest prywatne
	public:
		void set(T1 v);
		T1 get() {
			// gdyby parametr szablonu miałbyć jakąś klasą, która zawiera deklarację typu
			// i chielibyśmy odwołać się do tego typu, konieczne byłoby poinformowanie
			// kompilatora że odwołujemy się do typu, a nie składowej z tej klasy poprzez
			//   typename T3::typ_z_t3 var = 0;
			// (samo T3::typ_z_t3 szukałoby składowej typ_z_t3 w T3 ...)
			
			// niekiedy potrzebujemy sprawdzić czy przekazany do szablonu typ posiada
			// jakąś cechę (np. jak jest wartość maksymalna przekazanego typu liczbowego
			// w tym celu korzysta się z mechanizmu trait'ów - są to szablony prostych
			// funkcji zwracających pewną cechę przekazanego jako parametr szablonu typu
			// (np. czy jest to iterator, czy jest to tablica, ...)
			cout << "maksymalny zakres T2 wynosi " << numeric_limits<T2>::max() << endl;
			
			return val;
		}
		
		/// generowanie wyjątku
		void wyj(T2 i) throw (char const *, int) {
			switch(i) { // rzucamy wyjątek w zależnosci od i
				case 0:
					throw "Ala ma kota";
				case 1:
					cout << "wypisze sie (0) ?" << endl;
					throw "Kot ma Ale";
					// oczywiście gdy w definicji funkcji podaliśmy throw od innego obiektu
					// to tutaj po throw tez dajemy obiekt takiego typu
				case 2:
					throw i;
			}
			cout << "wypisze sie (1) ?" << endl;
		}
};

/// deklarujemy szablon funkcji z tej klasy
template <typename T1, typename T2> void klasa3<T1, T2>::set(T1 v) {
	val = v;
}

/// możemy zdeklarować także szablon funkcji nie należącej do klasy ...
// ponadto tutaj kompilator jest w stanie ustalić typ szablonu w oparciu
// o argument funkcji, dzięki czemu nie musimy jawnie podawać typu szablonu
// przy wywołaniu takiej funkcji ...
template <typename T1> T1 funkcja1(T1 x) {
	return 2 * x;
}

/// możemy tworzyć specjalizacje szablonów ...
template <> float funkcja1(float x) {
	return 3 * x;
}
// dla klasy wyglądałoby to tak:
//    template <> class klasa1<float> { ... }
// w przypadku pełnej specjalizacji - dla danego typu
// lub tak:
//    template <typename T1> class klasa1<T1*> { ... }
// w przypadku specjalizacji częściowej - dla dowolnego wskaźnika


/// z pomocą szablonów możemy także wykonywać obliczenia w trakcie kompilacji (np. silni)
template<int N> struct silnia {
	enum {val=N*silnia<N-1>::val};
};
template<> struct silnia <0>{
	enum {val=1};
};
///  parametrem szablonu nie musi być typ ...  może być wartość, a nawet inny szablon
//   należy tutaj zaznaczyć że jeżeli typ szablonu będącego parametrem
//   i typ naszego szablonu są powiązane (powinny być tego samego typu)
//   warto stosować następującą formę deklaracji, która pozwala uniknąć
//   błędu polegającego na podaniu sprzecznych typów
//    template<typename T, template<E, A> class C> class kont { ... }
//   wewnątrz klasy korzystamy z C<T, std::allocator<T>> jak z normalnego szablonu
//   a na zewnątrz wołamy kont<int, std:list> zamiast kont< int, std:list<int> >
//   gdy deklaracja byłaby: template<typename T, class C> class kont { ... }
//   takie przekazywanie szablonu może służyć do realizacji strategii w szablonach,
//   czyli decydowaniu na etapie wywołania jaką akcję podejmuje szablon poprzez
//   podanie do niego odpowiedniego szablonu klasy realizującej taką akcję
//   (np. zapis do pliku, lub na stdout w systemie logowania)

/// szablony mogą posłużyć także do korzystania z mechanizmu funkcji wirtualnych
// realizowanego na etapie kompilacji (bez dodatkowych narzutów w trakcie działania
// programu, jakie są związane z zwykłymi funkcjami wirtualnymi) ...
// realizuje się to poprzez dziedziczenie po szablonie klasy bazowej,
// w którym nasza klasa jest parametrm:
//   class abc : abecadlo_base<abc>
// a klasa bazowa implementuje funkcje wirtualne w formie wywołań:
//   return static_cast<T*>(this)->funkcja_wystepujaca_w_potomklach();
// podobny mechanizm jest często używany do uzupełniania klas potomnych o przeciążone
// operatory relacji realizowane przez negacje jednego operatora z klasy potomnej

/// szablony są niestety dość niewdzięczne w użyciu:
// - kod w nich podany jest kompilowany tylko wtedy gdy jest użyty
// - komunikaty błędów związane z użyciem szablonu na ogół wyglądają dziwnie i
//   wskazują gdzieś w kod szablonu a nie użycie które wywołało błąd
//   problem ten można rozwiązać używając w szablonach asercji czasu kompilacji
//   np. BOOST_STATIC_ASERT(p) z boost/static_asert.hpp
// - rozrost kodu wynikowego, tutaj można próbować ograniczyć to zapewniając
//   współdzielenie kodu przez poszczególne szablony poprzez realizację za ich
//   pomocą adapterów dostosowujących typ do stosowanego przez klasę realizującą
//   obliczenia (np. zamieniających wartości na wskaźniki ...)



/// przestrzenie nazw
namespace przestrzen {
	void tescik() {
		cout << "przestrzen nazw" << endl;
	}
}

int main() {
	/// tworzymy obiekty w oparciu o szablon klasy klasa3 dla typów double i int
	klasa3<double> d;
	klasa3<int> i;
	
	d.set(13.497);
	i.set(14);
	
	cout << d.get() + i.get() << endl;
	
	cout << "funkcja1: " << funkcja1(12) << " " << funkcja1(12.5f) << endl;
	
	/// przechwytujemy wyjątek
	for (int j=0; j<3; j++) {
		cout << endl << "j = " << j << endl;
		try {
			d.wyj(j);
			cout << "wypisze sie (2) ?" << endl;
			i.wyj(1);
		} catch (char const * opis) {
			cout << "WYJATEK: " << opis << endl;
		} catch (...) {
			cout << "inny wyjątek" << endl;
		}
		// aby zaoszczędzić jednego kopiowania należy przechwytywać wyjątek przez referencję
		// ale należy pamiętać iż ze względu na mechanizm działania wyjątków
		// wartość ta i tak będzie kopiowana
	}
	
	
	/// korzystanie z przestrzeni ...
	// przestrzenie stosowane standardowo (bez konieczności ich każdorazowego podawania)
	// można określać przy pomocy dyrektywy "using namespace" (zobacz na początku pliku)
	przestrzen::tescik();
}
