/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje standardowe wejście i wyjście, również odczyt i zapis plików
 *
 */

#include <iostream>
#include <iomanip> // dla modyfikatorów w cout / cin
#include <fstream> // dla plików
using namespace std; // aby nie pisać std::cout, std::cin, ... itd.

int main() {
	// standardowe wejście, wyjście, wyjście błędu w C++
	double liczba = 0;
	cout << "Podaj liczbę\n"; // wyjście
	cin >> liczba; // wejście - wczytuje pierwszy argument (do białego znaku)
	               // i próbuje potraktować jako liczbę, 
	cin.clear(); // czyszczenie błędów, aby program działał normalnie, gdy zostało
	             // podane coś co nie może być potraktowane jako liczba
	cerr << "Podałeś " << liczba << "\n"; // wyjście błędu
	clog << "Idziemy dalej\n"; // buforowane wyjście błędu
	
	// odczyt znaku
	int c = cin.get(); // zwraca znak lub EOF
	cout << "Podany został znak o numerze: " << c << " ... podaj kolejne i zakończ Enter" << endl;
	// widać że ze strumienia nie został pobrany Enter, którym zakończyliśmy wprowadzanie liczby ...
	char d;
	while ( ! cin.get(d).eof() ) { // w tym wypadku EOF sprawdzamy cin.get(d).eof()
		if (d == 10) break;
		cout << d;
	}
	cout << " - to są podane znaki" << endl;

	
	// odczyt napisu / linii ...
	char napis [20];
	cout << "Wpisz coś (i naciśnij Enter):" << endl;
	cin.getline(napis, 20);
	cout << "wpisałeś: " << napis << endl;

	cout << "Wpisz coś (tym razem aby zawierało w środku znak 0):" << endl;
	cin.getline(napis, 20, '0');
	cout << "wpisałeś: " << napis << endl;
	// podobnie można skorzystać z get - cin.get(napis, 20); lub cin.get(napis, 20, '0');
	// wtedy do tablicy wynikowej wpisany zostanie odpowiednio znak nowej linii i 0
	// wczytanie naprawdę cłej linii (bez ograniczenia długości) patrz getline w napisy.cpp
	
	// mamy również odczyt binarny - cin.read( wskaznik_gdzie, ile );
	
	// pozostałe ciekawe metody ...
	//  ignore(_ile_, _ogranicznik_); ignoruje _ile_ znaków albo (co nastąpi szybciej)
	//  do _ogranicznik_ domyślnie _ile_=1, _ogranicznik_=EOF
	cin.ignore(1000, '\n'); // ignorujemy wszystko co było dalej
	
	cout << "podaj znak\n";
	cin.get(d);
	cout << "podałeś: " << d << endl;
	
	// mamy też metody:
	//  unget -oddającą ostatnio pobrany znak
	cin.unget();
	//  putback - oddającą zadany znk do strumienia
	// cin.putback(d);
	//  peek - pozwalającą na podejrzenie następnego znaku bez zabierania go ze strumienia
	cout << "ostatnim pobranym znakiem było: " << cin.peek() << endl;
	
	cin.getline(napis, 20);
	cout << "w naszym strumieniu było: " << napis << endl;
	
	
	// formatowanie wyjścia/wejścia ...
	cout.setf(ios::fixed);  // zapewnia że precision() odnosi się do miejsc po kropce
	                        // bez tego precision może okreslać ilość znaczących cyfr w całej liczbie
	cout.setf(ios::showpoint);  // wypisujemy kropkę dziesiętną i nieznaczące zera
	cout.width(10); // minimalna ilość znaków wypisywanej liczby
	cout.fill('0'); // wypełniacz dla znaków dodawanych na początku liczby
	cout.precision(6); // precyzja (patrz uwaga o ios::fixed)
	cout << 20.9863 << endl;
	
	// taki sam efekt można by uzyskać korzystając z modyfikatorów umieszczanych w ciągu << <<:
	cout << setw(6) << setfill('_') << setprecision(2) << 20.9863 << endl;
	// mamy też modyfikatory do zmiany systemu liczbowego
	cout << "10 w systemie ósemkowym: " << setbase(8) << 10 << endl;
	cout << "10 w systemie szesnastkowym: " << setbase(16) << 10 << endl;
	cout << "10 w systemach o podstawie 8, 10 i 16: ";
	cout << oct << 10 << " " << dec << 10 << " " << hex << 10 << endl;
	
	// warto wspomnieć też o flush wyrzucającym zawartość bufora ...
	
	// są też wspomniane już flagi formatujące ustawiane przez cout.setf(ios::flaga):
	//  skipws - pominięcie początkowych spacji (cin, modyfikator wc) 
	//  uppercase - wielkie litery w trybie szesnastkowym i notacji wykładniczej:
	cout.setf(ios::uppercase);
	//  showbase - nagłówki identyfikujące system dla 16-kowego i 8-kowego:
	cout.setf(ios::showbase);
	cout << "10 w systemach o podstawie 8, 10 i 16: ";
	cout << oct << 10 << " " << dec << 10 << " " << hex << 10 << endl;
	//  showpoint - w zmiennoprzecinkowych zawsze wypisz kropkę 
	//  showpos - dodatni poprzedź plusem
	//  unitbuf - wyłącz buferowanie
	
	// ustawiane przez cout.setf(ios::flaga, basefield):
	//  dec - dziesiętny system liczbowy
	//  oct - ósemkowy system liczbowy
	//  hex - szesnastkowy system liczbowy
	
	// ustawiane przez cout.setf(ios::flaga, floatfield):
	//  scientific - notacja naukowa dla zmiennoprzecinkowych
	//  fixed - notacja zwykła dla zmiennoprzecinkowych
	//  0 - automatyczny wybór (domyślna)
	
	// ustawiane przez cout.setf(ios::flaga, adjustfield):
	//  left - wyrównanie do lewej
	//  right - wyrównanie do prawej
	//  internal - wyrównanie do srodka
	
	
	// obsługa plików
	// oprócz predefiniowanych strumieni możemy tworzyć i korzystając z strumieni związanych z plikami
	ofstream plik_w; // tworzymy strumień (typu out) związany z plikiem ...
	ifstream plik_r; // tworzymy strumień (typu in) związany z plikiem ...
	plik_w.open("/tmp/plik_w", ios::out|ios::ate);
		// otwieramy plik w trybie dopisywania z możliwością zmiany dotychczasowej zawartości
	plik_r.open("/tmp/plik_r"); // otwieramy plik do czytania
	// tryby możemy budować z następujących opcji:
	//  ios::in - odczyt (domyślna dla ifstream)
	//  ios::out - zapis (domyślna dla ofstream)
	//  ios::ate - ustawie pozycji na koniec pliku
	//  ios::app - dopisywanie
	//  ios::trunc - nadpisuje plik
	//  ios::noreplace - nie nadpisuje pliku
	//  ios::nocreate - wymaga aby plik istniał
	
	cout << "Aktualna pozycja w pliku OUT: " << plik_w.tellp() << endl;
	cout << "Aktualna pozycja w pliku IN: " << plik_r.tellg() << endl;
	
	// pisanie i czytanie dokładnie jak dla cin cout ...
	plik_w << "Hello World !!!";
	
	// możemy tez przemieszczać się w pliku - do określonej pozycji
	//  plik_w.seekp( pozycja );
	//  plik_r.seekg( pozycja );
	// lub o określoną odległość (także ujemną) od pozycji
	//  plik_w.seekp( przesuniecie, tryb );
	//  plik_r.seekg( przesuniecie, tryb );
	// tryb może mieć postaci:
	//  ios::beg - od początku 
	//  ios::cur - od bieżącej pozycji 
	//  ios::end - od końca
	
	// na koniec zamykamy pliki
	plik_w.close();
	plik_r.close();
}
