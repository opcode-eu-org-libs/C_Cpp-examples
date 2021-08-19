/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody operowania zmiennymi napisowymi
 *
 */
 
#include <stdio.h>
#include <string.h>

#include <sstream>
#include <string>
#include <iostream>

int main() {
// konwersja C++ <-> C
	// teraz deklaracja strumienia napisowego C++ i wpisanie do niej wartości
	std::ostringstream ostringstream_text;
	ostringstream_text << "cpp\n";
	
	// zamiana na standardowy napis C - np. celem wypisania przez printf()
	printf("1) %s", ostringstream_text.str().c_str());
	
	// można to zrobić rownież tak ... (z wykorzystaniem zmiennej napisowej C++)
	std::string string_text1 = ostringstream_text.str();
	printf("2) %s", string_text1.c_str());
	// warto zaznaczyć że kompilator może optymalizować string przy przekazywaniu
	// do funkcji przez wartość jako "copy-on-write", co może spowodować problemy
	// przy programowaniu wielowątkowym
	// (gdy macierzysty zmodyfikuje zanim wątek zrobi własną kopię)

	// a nawet tak ...
	int dlugosc = string_text1.length()+1;
	//  deklarujemy tablice - można by także użyć malloc
	char tresc1 [dlugosc];
	//  kopiujemy do niej nasz napis
	memcpy(tresc1, string_text1.c_str(), sizeof(char) * dlugosc);
	printf("3) %s", tresc1);

	// albo i tak ...
	const char* tresc2 = NULL;
	tresc2 = ( ostringstream_text.str() ).c_str();
	printf("4) %s", tresc2);
	
	// w powyższych punktach 1 do 4 przedstawiłem metody konwersji
	// ostringstream do napisu standardowego C,
	// zaczynając i kończąc w zasadzie na takiej samej metodzie ...

// strumienie napisowe C++
	// możemy wydłużać tak utworzony napis:
	ostringstream_text << "abecadlo\n";
	std::cout << ostringstream_text.str(); //printf("%s", ostringstream_text.str().c_str());
	
	// możemy także wykasować nasz napis:
	ostringstream_text.str(""); ostringstream_text.clear();
	std::cout << "nic tu _" << ostringstream_text.str() << "_ nie ma ?\n";
	
	// teraz możemy znowu wpisywać
	ostringstream_text << "Wszyscy ludzie rodzą się wolni i równi pod względem swej godności";
	std::cout << ostringstream_text.str() << std::endl;
	
	// można też połączyć te napisy
	std::ostringstream ostringstream_text2;
	string_text1 = "i swych praw.\n";
	ostringstream_text2 << ostringstream_text.str() << " " << 12.5 << string_text1;
	// i wpisać do string_text
	string_text1 = ostringstream_text2.str();
	std::cout << string_text1; //printf("%s", string_text2.c_str());

	/* podsumowanie - ostringstream:
		* wygodnie się łączy i rozszerza
		* aby zamienić na string używamy metody .str()
		* aby utworzyć z czegokolwiek po prostu wpisujemy przez <<
		  zatem wygodne do konwersji liczb na napis
		  (patrz także boost::lexical_cast)
		
		  więcej w in_out.cpp
	*/



// zmienne napisowe C++
	// wyszukiwanie ...
	// pierwsze wystąpienie zadanego ciągu
	int pozycja = string_text1.find(" sw");
	// możemy szukać od tyłu przez .rfind()
	if (pozycja != std::string::npos) {
		printf("znalazłem \" sw\" na pozycji %d\n", pozycja);
			// wyszukujemy od poprzedniej pozycji
		pozycja = string_text1.find(" sw", pozycja+1);
		if (pozycja != std::string::npos)
			printf("znalazłem kolejne \" sw\" na pozycji %d\n", pozycja);
	} else {
		printf("nie znalazłem \" sw\"\n");
	}
	
	// pierwsze wystąpienie zadanego któregoś ze znaku
	pozycja = string_text1.find_first_of(" i.");
	// możemy szukać od tyłu przez .find_last_of()
	// bądź też znaku innego niż podany odpowiednio przez
	//  .find_first_not_of() i .find_last_not_of()
	if (pozycja != std::string::npos)
		printf("znalazłem spację kropkę lub i na pozycji %d\n", pozycja);

	
	// możemy wycinać zadany fragment napisu:
	std::string string_text2; // definicja drugiego napisu
	string_text2 = string_text1.substr(73); // od 74 znaku (włącznie) do końca
	string_text1 = string_text1.substr(0,14); // 14 znaków od początku


	// możemy też łączyć napisy typu string bez pośrednictwa ostringstream
	string_text1 += " " + string_text2;
	// można też przy pomocy metod zamiast przeciążonych operatorów
	//  string_text.append(" ").append(string_text2);
	// co da identyczny rezultat
	std::cout << string_text1;
	
	std::cout << "Wpisz coś długiego\n";
	std::getline( std::cin, string_text2 );
		// wczyta całą linię z standardowego wejścia do string_text
		// jako trzeci argument możemy podać inny ogranicznik niż znak nowej linii
	std::cout << "Wpisałeś: " << string_text2 << std::endl;

	string_text1 = "Ala ma kota";
	string_text2 = "Ala ma psa";
	// możliwe jest także porównywanie fragmentów napisów:
	// string_text1.compare(od_1, ilosc_1, string_text2, od_2, ilosc_2)
	if (string_text1.compare(1, 5, string_text2, 2, 5))
		printf("podnapis 2-7 (numerujemy od zera) jest taki sam\n");
	if (string_text1 != string_text2)
		printf("ale napisy są różne\n");
		
	// wstawianie - insert(pozycja, co)
	string_text2.insert(6, " kota i");
	std::cout << string_text2 << std::endl;
	
	// zastępowanie - replace(pozycja, ile, czym);
	string_text2.replace(4, 2, "miała samochód", 0, 6);
	// mogłoby tez być string_text2.replace(4, 2, "miała"); i parę innych wariantów ...
	std::cout << string_text2 << std::endl;
	
	// usuwanie - erase(pozycja, ile);
	string_text2.erase(9, 1); // 9 zamiast 8 bo UTF-8 i ł ma dwa znaki
	std::cout << string_text2 << std::endl;
	
	// zmiana długości - resize(długość, znak_dopełniający)
	string_text2.resize(14);
	std::cout << string_text2 << std::endl;
	
	string_text2.resize(18, 'x');
	std::cout << string_text2 << std::endl; 
	
	/* podsumowanie - string
		* wygodny do operowania - wyszukiwanie, wycinanie, ...
		* łączenie i rozszerzenie też całkiem dobre ;-)
		* aby zamienić na char* (napis C) używamy metody .c_str()
	*/
	
	// oraz na koniec - podział linii na pola:
	std::string linijka = "12 14 25 ";
	int tmp=-1, old_tmp, x;
	for (int i=0; i<3; i++) {
		// ustalamy granice pola
		old_tmp = tmp + 1;
		tmp = linijka.find(" ", old_tmp);
		// konwertujemy pole na liczbę
		x = atoi(linijka.substr(old_tmp, tmp).c_str());
		// wypisujemy
		std::cout << "Pole numer: " << i << " ma wartość: " << x << std::endl;
	}
}
