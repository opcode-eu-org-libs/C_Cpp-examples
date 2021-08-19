/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody komunikacji sieciowej (TCP/IP) - program wysyłający dane przez TCP
 *
 * wywołanie: ./program adres.ip port dane
 *
 * warto zobaczyć różnicę przy wywołaniu `./a.out localhost 2346 ppp`
 * gdy słuchamy tylko na IPv4 lub tylko na IPv6, np. z użyciem netcata:
 * `nc -4lp 2346` kontra `nc -6lp 2346`
 */

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	if (argc<4) {
		fprintf(stderr, "USAGE: %s host port message\n", argv[0]);
		return 1;
	}
	
	// 1. zamieniamy nazwę domenową / adres IP oraz port / nazwę usługi na łańcuch struktur addrinfo
	//   (opis tej struktury znajduje się w `man 3 getaddrinfo`)
	struct addrinfo *netInfo;
	int res = getaddrinfo(argv[1], argv[2], NULL, &netInfo);
	if (res != 0) {
		fprintf(stderr, "error in getaddrinfo: %d %s\n", res, strerror(errno));
		return 2;
	}
	
	// 2. iterujemy po otrzymanych wynikach funkcji getaddrinfo
	for (struct addrinfo *netInfo2 = netInfo; netInfo2; netInfo2=netInfo2->ai_next) {
		// jeżeli wynik dotyczy SOCK_STREAM (TCP) to ...
		if (netInfo2->ai_socktype == SOCK_STREAM) {
			// a. otwieramy gniazdo sieciowe odpowiedniej rodziny (IPv4 albo IPv6)
			int sh = socket(netInfo2->ai_family, SOCK_STREAM, 0);
			if (sh < 0) {
				fprintf(stderr, "error in socket: %d %s\n", sh, strerror(errno));
				continue;
			}
			
			// b. nawiązujemy połączenie dane używając struktury sockaddr (dokładniej sockaddr_in / sockaddr_in6)
			//    zawartej w polu ai_addr struktury addrinfo
			//    (opis tych struktur znajduje się w `man 7 ip` / `man 7 ipv6`;
			//     można je także wypełniać ręcznie, np. z pomocą funkcji typu inet_pton, htons)
			res = connect(sh, netInfo2->ai_addr, netInfo2->ai_addrlen);
			if (res) {
				fprintf(stderr, "error in connect: %d %s\n", res, strerror(errno));
				close(sh);
				continue;
			}
			
			// c. wysyłamy dane
			res = send(sh, argv[3], strlen(argv[3]), 0);
			
			// c. zamykamy gniazdo
			close(sh);
			
			// d. jeżeli wysyłanie nie powiodło się to podejmujemy kolejną próbę
			if (res < 0) {
				fprintf(stderr, "error in sendto: %d %s\n", sh, strerror(errno));
				continue;
			}
			
			// e. jeżeli wysłaliśmy jakieś dane to wypisujemy ile i przerywamy pętle
			//    to jest TCP więc wiemy że zostały odebrane
			printf("wysłano %d bajtów\n", res);
			break;
		}
	}
	
	// 3. zwalniamy pamięć zajmowaną przez wyniki getaddrinfo
	if (netInfo) {
		freeaddrinfo(netInfo);
	}
}
