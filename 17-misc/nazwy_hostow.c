/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * demonstruje przekształcanie nazwy hosta na adres IP
 */

#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {
	if (argc<2) {
		puts("podaj nazwę hosta jako argument");
		return 1;
	}
	
	int i=0;
	char buff[40];
	void* adres;
	
	// zamiana nazwy na adresy
	struct addrinfo *info1;
	errno = 0;
	int error = getaddrinfo(argv[1], NULL, NULL, &info1);
	if (error != 0) {
		perror("getaddrinfo: ");
		printf("getaddrinfo: %d %s\n", error, gai_strerror(error));
	}
	
	struct sockaddr_in *socket_v4;
	struct sockaddr_in6 *socket_v6;
	
	printf("Za pomocą getaddrinfo\n");
	do {
		// UWAGA: to co siedzi w info1->ai_addr to sockaddr_in lub sockaddr_in6 a nie sockaddr !!!
		if (info1->ai_family == AF_INET) {
			socket_v4 = (struct sockaddr_in*) info1->ai_addr;
			adres = &socket_v4->sin_addr;
			printf("\tadres ip w postaci HEX: %x\n", socket_v4->sin_addr.s_addr);
		} else if (info1->ai_family == AF_INET6) {
			socket_v6 = (struct sockaddr_in6*) info1->ai_addr;
			adres = &socket_v6->sin6_addr;
			printf("\tadres ip w postaci HEX: ");
			for (i=0; i<16; i++)
				printf("%x ", socket_v6->sin6_addr.s6_addr[i]);
			printf("\n");
		}
		
		// konwersja wartości numerycznej na ludzki zapis adresu
		// funkcja inet_ntop rozpoznaje przyjmowany typ struktury
		// mogą to być n6_addr, sockaddr_in6, in_addr, sockaddr_in
		if (!inet_ntop  (info1->ai_family, adres, buff, 40)) {
			perror( "próba zamiany adresu nieudana" );
			return -1;
		}
		printf("\t%s %s (%d %d)\n\n", info1->ai_canonname, buff, info1->ai_family, info1->ai_addrlen);
		
		// bierzemy następny z uzyskanych adresów
		info1=info1->ai_next;
	} while(info1!=NULL);
}
