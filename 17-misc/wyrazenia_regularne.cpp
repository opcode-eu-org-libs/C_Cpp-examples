/** plik prezentuje podstawy programowania obiektowego w C++ :
  *  korzystanie z wyrażeń regularnych
  *
  *  kompilacja: g++ -lboost_regex wyrazenia_regularne.cpp
  * 
  * (wymagany pakiet: libboost-regex-dev)
  **/

#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main() {
	static const boost::regex wyrazenie1("(.*)(ko*t).*");
	static const boost::regex wyrazenie2("(ko*t)");
	static const boost::regex wyrazenie3("(ko.*?) ");
	// dodanie ? w tym wyrażeniu powoduje jego niezachłanne działanie
	// (dopasowuje najmniej jak się da)
	static const boost::regex wyrazenie4(" +");
	
	boost::match_results<std::string::const_iterator> dopasowania;
	std::string napis1 = "Ala ma koooota";
	std::string napis2 = "Kot ma Ale ...";
	std::string napis3 = "kot psu kotem a kotowi psem";
	
	/* sprawdzanie czy pasuje */
	if (regex_match(napis1, dopasowania, wyrazenie1)) {
		std::cout << "napis1 pasuje" << std::endl;
		std::cout << " dopasowany napis to \"" << dopasowania[0];
		// w kolejnych elementach dopasowania znajdują się napisy odpowiadające dopasowanym podwyrażeniem
		std::cout << "\" a pierwsze podwyrazenie ma wartość \""  << dopasowania[1] << "\"" << std::endl;
	} else
		std::cout << "napis1 nie pasuje" << std::endl;
	
	if (regex_match(napis2, wyrazenie1))
		std::cout << "napis2 pasuje" << std::endl;
	else
		std::cout << "napis2 nie pasuje" << std::endl;
	
	/* wyszukiwanie */
	std::string::const_iterator poczatek = napis3.begin();
	std::string::const_iterator koniec = napis3.end();
	while (boost::regex_search(poczatek, koniec, dopasowania, wyrazenie3)) {
		std::cout << "Znalazlem: " << dopasowania[1] << "X" << std::endl;
		// tutaj podobnie jak wcześniej w 0 mamy cale dopasowane wyrażenie
		// a w kolejnych [] odpowiednie podwyrażenia
		
		// ustawiamy początek szukania na jeden znak dalej niż koniec znalezionego tekstu
		poczatek = dopasowania[0].second;
	}
	
	// możemy także użyć iteratorow sregx
	boost::sregex_iterator begin(napis3.begin(), napis3.end(), wyrazenie3);
	boost::sregex_iterator end;
	while (begin != end){
		dopasowania = *begin;
		std::cout << "Znalazłem: " << dopasowania[1] << " X " << (*begin)[1] << std::endl;
		begin++;
	}
	
	/* zastępowanie */
	napis1=regex_replace(napis1, wyrazenie2, "psa i \\1");
	std::cout << "Teraz: " << napis1 << std::endl;
	
	/* dzielenie */
	boost::sregex_token_iterator i(napis3.begin(), napis3.end(), wyrazenie4, -1);
	boost::sregex_token_iterator j;
	while(i != j) {
		std::cout << "Wydzielilem: " << *i++ << std::endl;
	}
	
	// w Boost są także inne implementacje wyrażeń regularnych - w tym umożliwiające
	// tworzenie wyrażeń na etapie kompilacji oraz operowanie bardziej złożonymi gramatykami
}
