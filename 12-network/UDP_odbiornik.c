/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody komunikacji sieciowej (TCP/IP) - program odbierający dane przez UDP
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

#define _BUF_SIZE_ 255


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
	
	// 1. otwieramy gniazdo (IPv6, UDP) ...
	int sh = socket(AF_INET6, SOCK_DGRAM, 0);
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
	
	// 4. odbiór danych ...
	while(1) {
		char buf[_BUF_SIZE_];
		struct sockaddr_in6 from;
		socklen_t fromlen = sizeof(from);
		
		// a. czekamy na porcję danych i ją odbieramy
		res=recvfrom(sh, buf, _BUF_SIZE_, 0, (struct sockaddr *) &from, &fromlen);
		if (res<0) {
			fprintf(stderr, "error in recvfrom: %d %s\n", sh, strerror(errno));
			continue;
		}
		
		// b. konwertujemy adres nadawcy na napis
		char addr_buf[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &(from.sin6_addr), addr_buf, INET6_ADDRSTRLEN);
		
		// c. konwertujemy otrzymane dane (zakłądamy że to napis) na NULL-end string
		buf[res]=0;
		
		// d. wypisujemy
		printf("odebrano od [%s]:%d : %s\n", addr_buf, ntohs(from.sin6_port), buf);
		
		// e. można coś wysłać do "klienta" z użyciem:
		sendto(sh, "OK\n", 3, 0, (struct sockaddr *) &from, fromlen);
	}
	
	// 5. zamkniecie gniazda
	close(sh);
}
