/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody komunikacji sieciowej (TCP/IP) - prosty serwer TCP echo
 *
 */

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 255
#define QUERY_SIZE 1

int main(int argc, char *argv[]) {
	if (argc<2) {
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		return 1;
	}
	int numerPortu = strtoll(argv[1],0,0);
	if (numerPortu <= 0 || numerPortu > 65535) {
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		return 1;
	}
	
	// 1. otwieramy gniazdo (IPv6, TCP) ...
	int sh = socket(AF_INET6, SOCK_STREAM, 0);
	if (sh<0) {
		fprintf(stderr, "error in socket: %d %s\n", sh, strerror(errno));
		return 2;
	}
	//    i ustawiamy obsługę IPv4 oraz IPv6 na jednym gnieździe
	int res = 0;
	res = setsockopt(sh, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&res, sizeof(res)); 
	if (res<0) {
		fprintf(stderr, "error in setsockopt: %d %s\n", res, strerror(errno));
		return 2;
	}
	
	// 2. przygotowanie struktury z adresem do słuchania
	struct sockaddr_in6 addr;
	addr.sin6_family=AF_INET6;
	addr.sin6_port=htons( numerPortu ); // zmieniamy porządek bajtów na sieciowy (host to network short);
	                                    // są też takie funkcje dla long oraz odwrotne
	addr.sin6_addr=in6addr_any; // słuchamy na wszystkich adresach (zamiast tego można określić konkretny adres)
	
	// 3. przypisanie adresu do gniazda
	res=bind(sh, (struct sockaddr *) &addr, sizeof(addr));
	if (res<0) {
		fprintf(stderr, "error in bind: %d %s\n", res, strerror(errno));
		return 3;
	}
	
	// 4. ustawienie nasłuchiwania połączeń na gnieździe
	if (listen(sh, QUERY_SIZE) < 0) {
		fprintf(stderr, "error in listen: %s\n", strerror(errno));
		return 4;
	}
	
	// 5. odbiór danych ...
	while(1) {
		// odebranie połączenia
		struct sockaddr_in6 from;
		socklen_t fromlen = sizeof(from);
		int sh2 = accept(sh, (struct sockaddr *) &from, &fromlen);
		if (sh2<0) {
			fprintf(stderr, "error in accept: %d %s\n", sh, strerror(errno));
			continue;
		}
		
		// tutaj moglibyśmy utworzyć potomka zajmującego się tym połączeniem ...
		// wtedy w macierzystym robimy close(sh2) a w potomnym close(sh)
		
		// wypisanie informacji o nawiązaniu połączenia
		char addr_buf[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &(from.sin6_addr), addr_buf, INET6_ADDRSTRLEN);
		printf("połączenie od [%s]:%d\n", addr_buf, ntohs(from.sin6_port));
		
		// obsługa w trybie plikowym ... można też bezpośrednio read/write na sh2
		FILE * net;
		char buf[BUF_SIZE];
		net=fdopen(sh2, "r+");
		while(fgets(buf, BUF_SIZE, net))
			fputs(buf, net);
		fclose(net);
	}
	
	// 6. zamkniecie gniazda
	close(sh);
}
