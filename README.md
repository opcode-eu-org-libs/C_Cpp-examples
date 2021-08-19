Repozytorium zawiera zbiór opisanych w komentarzach programów demonstrujących podstawowy programowania w C i C++
	(w sporej części przeznaczonych dla środowisk Unixowych, ze względu na wykorzystanie oferowanych przez nie funkcji systemowych).
Osoby zaczynające dopiero naukę tych języków zachęcam najpierw do zapoznania się z bardziej uporządkowanym opisem podstaw w [http://www.opcode.eu.org/Podstawy_C.pdf](http://www.opcode.eu.org/Podstawy_C.pdf).
Zachęcam także do zapoznania się z wielojęzykowym wprowadzeniem do programowania zawartym w ramach [vademecum informatyki praktycznej](http://vip.opcode.eu.org/).

Przykłady w repozytorium są stworzone z myślą o zapoznaniu się z kodem (oraz komentarzami w nim zawartymi), uruchomieniu go,
	a następnie wykonywania na nim własnych eksperymentów (zmieniania wartości, warunków itd).
Takie eksperymenty pozwolą dużo lepiej zapoznać się z prezentowanymi w przykładach konstrukcjami i funkcjami biblioteki standardowej.
Warto zauważyć że część zagadnień jest jedynie opisana  w komentarzach i wręcz wymaga samodzielnego dodania do kodu, eksperymentowania z nim.

Część programów nie przypadkowo zachowuje się trochę dziwnie po uruchomieniu (np. wypisując dość przypadkowe teksty czy ich fragmenty).
Należy patrzeć w ich kod i starać się zrozumieć dlaczego zachowują się w taki sposób.
Bardzo pomocne będzie w tym eksperymentowanie z kodem.

## spis treści

Repozytorium zawiera kilka katalogów, grupujących przykłady związane z danym typem zagadnienia.
Każdy z nich zawiera plik README.md z opisem plików znajdujących się w danym katalogu w sugerowanej kolejności zapoznawania się z nimi.
Są to:

1. [Podstawy C](01-podstawy_C/README.md) – Przykładowe programy prezentujące podstawowe zagadnienia z programowania w ("czystym") C. Takie jak definiowanie zmiennych, funkcji, podstawowe operacje wejścia wyjścia.
2. [Podstawy C++](02-podstawy_Cpp/README.md) – Podstawy C++ różne od C, czyli tylko rzeczy dodane w C++ (część wspólna jest pominięta – zapoznaj się z "Podstawy C").
3. [Trochę więcej niż podstawy C](05-wiecej_niz_podstawy/README.md) – Troszkę bardziej zaawansowane zagadnienia z C (alokacja pamięci, obsługa błędów, operacje bitowe).
4. [Podstawy programowania Unixa/Linuxa](11-posix/README.md) – Podstawowe tematy związane z funkcjami systemów POSIX (Linux, Unix, ...) takie jak: funkcje daty i czasu, czytanie katalogu, tworzenie nowych procesów, sygnały, deskryptory plików, obsługa opcji linii poleceń.
5. [Sieci TCP/IP](12-network/README.md) – Wysyłanie i odbiór po UDP oraz TCP, zamiana nazw domenowych na adresy.
6. [Trochę więcej niż podstawy programowania Unixa/Linuxa](15-posix2/README.md) – Bardziej zaawansowane zagadnienia związane z funckjami systemów POSIX (wielowątkowość, komunikacja międzyprocesowa, terminale tty).
7. [Różne, inne i trochę bardziej zaawansowane tematy](17-misc/README.md) – używanie języków skryptowych z poziomu C, wątki i wyrażenia regularne w C++, praktyczna komunikacja TCP (wraz z obsługą napisów) w C++, resolver DNS, adresacja IPv6
8. [Bliżej sprzętu i jądra](21-hardware/README.md) – jak alokowana jest pamięć, porty I/O, blokowanie przerwań, precyzyjny pomiar czasu, USB, moduły jądra, ...
9. [(Graficzny) interfejs użytkownika](31-user_interface/README.md) – graficzny (GTK i Qt), pseudograficzny (ncurses) interfejs użytkownika i okolice (sekwencje sterujące terminala, rysowanie po framebufferze, rendering napisów)
10. [Multimedia](32-multimedia/README.md) – audio, grafika, obsługa plików PNG, grafika 3d w OpenGL, ...
11. [Podstawy metod numerycznych](41-numerical_analysis/README.md) – zbiór funkcji prezentujących podstawy metod numerycznych

## kompilacja i uruchomienie przykładów

Większość przykładów można skompilować po pobraniu (sklonowaniu) repozytorium uruchamiając w linii poleceń:

	clang ścieżka/do/przykładu.c

albo dla C++:

	clang++ ścieżka/do/przykładu.cpp

Można także korzystać z narzędzi GCC:

	gcc ścieżka/do/przykładu.c

a dla C++:

	g++ ścieżka/do/przykładu.cpp

W wyniku kompilacji powstanie plik wykonywalny `a.out`, który należy uruchomić poleceniem `./a.out`.
Na przykład:

	clang 1-podstawy_C/podstawy.c
	./a.out

Część bardziej zaawansowanych przykładów może wymagać dodania opcji (np. dodatkowych bibliotek).
Wówczas przykładowe polecenie do ich kompilacji znajduje się w komentarzu na początku takiego przykładu.
