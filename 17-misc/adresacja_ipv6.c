/**
  * program oblicza adres sieci
  * na podstawie podanego adresu IPv6 hosta z tej sieci i maski
**/

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>

int main (int argc, char *argv[], char *envp[]) {
	if (argc != 3){
		puts("zła liczba argumentów – podaj adres IP i długość prefixu");
		return -1;
	}
	
	// konwertujemy adres IPv6 z napisu na strukturę, zawierającą tablice 16 bajtów
	// odpowiadających kolejnym bajtom adresu
	struct in6_addr adres; 
	if (inet_pton(AF_INET6, argv[1], &adres) <= 0 ) {
		if (errno == 0)
			puts( "podany adres jest nieprawidłowy");
			// inet_pton nie ustawia wtedy errno ale zwraca 0 ...
		else
			perror( "próba zamiany adresu nieudana" );
		return -2;
	}
	
	// tworzymy analogiczna strukturę z tablica odpowiadającą bajtom maski
	int msk = strtol(argv[2], 0, 0);
	struct in6_addr maska;
	for (int i=0; i<16; i++){
		if (i<msk/8) // (1)
			maska.s6_addr[i]=0xff;
		else if (i==msk/8) // (2)
			maska.s6_addr[i]=(0xff << (8-msk%8));
		else // (3)
			maska.s6_addr[i]=0;
	}
	
	// wykonujemy and binarny miedzy adresem ip hosta a maska otrzymując adres sieci
	// w zasadzie można by to zrobić w powyższej pętli - zamiast tworzyć pełny opis maski
	// dla (1) nic nie robić, dla (2) wykonać and dla tego bajtu dla (3) wyzerować bajty adresu
	for (int i=0; i<16; i++)
		adres.s6_addr[i] = maska.s6_addr[i] & adres.s6_addr[i];
	
	// wypisujemy (dokonując konwersji na postać napisową) otrzymany adres sieci
	char dst[40];
	if (NULL == inet_ntop  (AF_INET6, &adres, dst, 40)) {
		perror( "próba zamiany adresu nieudana" );
		return -3;
	}
	printf("Adres sieci to %s/%d\n", dst, msk);
}
