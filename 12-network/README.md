# Sieci TCP/IP

Podstawy działania sieci, adresacji oraz (analogiczne do poniższych) przykłady klienta i serwera UDP i TCP (ale w Pythonie) można znaleźć w [TCP/IP & Ethernet](http://www.opcode.eu.org/Sieci.pdf).

1. [UDP_nadajnik.c](UDP_nadajnik.c)
	* zamiana nazw na adresy
	* wysyłanie danych po UDP
2. [UDP_odbiornik.c](UDP_odbiornik.c)
	* odbiór danych i odpowiadanie po UDP
	* równoczesne słuchanie IPv4 i IPv6
	* konwersja adresów na napisy
3. [TCP_nadajnik.c](TCP_nadajnik.c)
	* zamiana nazw na adresy
	* nawiązywanie połączenia i wysyłanie danych po TCP
4. [TCP_serwer.c](TCP_serwer.c)
	* odbiór danych i odpowiadanie po TCP
	* równoczesne słuchanie IPv4 i IPv6
	* konwersja adresów na napisy
	* obsługa sieci w trybie plikowym (poprzez `fgets`, `fputs`, itp)
