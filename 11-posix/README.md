# Podstawy programowania Unixa/Linuxa

1. [czas.c](czas.c)
	* czas
2. [unix.c](unix.c)
	* sygnały
	* deskryptory
	* łącza nazwane FIFO
	* oczekiwanie na dane (funkcja `select`)
3. [mmap.c](mmap.c)
	* mapowanie plików do pamięci
4. [katalogi.c](katalogi.c)
	* listowanie plików w katalogu, czyli nasze własne ls ;-)
	* operacje na plikach (takie jak np. zmiana nazwy)
5. [opcje.c](opcje.c)
	* obsługa opcji unix'owych (-h) i GNU (--help)
6. [fork.c](fork.c)
	* rozgałęzianie procesu
	* wywołania innych programów
	* strumienie między procesami (pipe)
