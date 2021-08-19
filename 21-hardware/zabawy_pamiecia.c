/*
 * plik ma na celu zobrazowanie sposobu alokacji pamięci w systemie Linux
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * napis = "Ala ma kota";
int a;
unsigned int * wsk;
unsigned char * wsk2;


int main(int argc, char *argv[], char *envp[]) {
	printf("fragment stosu związany z main()\n");
	for (wsk=&argc; wsk<argv[0]; wsk++)
		printf("0x%x : 0x%x\n", wsk, *wsk);
	// tutaj zaczynają się tablice char
	for (wsk2=argv[0]; wsk2<argv[0]+20; wsk2++)
		printf("0x%x : 0x%x (%c)\n", wsk2, *wsk2, *wsk2);
	
	printf("\n\n");
	
	
	printf("adresy zmiennych przekazywane do main()\n");
	printf("\t0x%x 0x%x 0x%x\n", &argc, &argv, &envp);
	printf("zmienne przekazywane do main()\n");
	printf("\t0x%x 0x%x 0x%x\n", argc, argv, envp);
	// warto zauważyć że envp jest kontynuacją argv - nic ich nie oddziela w pamięci ...
	printf("napisy na które wskazują\n");
	printf("\t0x%x 0x%x 0x%x\n", argv[0], envp[0], envp[1]);
	
	printf("\n\n");
	
	
	int b;
	int c;
	printf("alokacja zmiennej globalnej a lokalnej\n");
	printf("\t0x%x 0x%x (b=0x%x) 0x%x (c=0x%x)\n", &a, &b, b, &c, c);
	// warto zwrócić uwagę iż zmienna lokalna jest zaalokowana na stosie przed
	// tablicami będącymi argumentami main(), a zmienna globalna poza stosem
	// kolejne zmienne alokowane są coraz bardziej przed (stos rośnie w dół)
	// jeżeli którąś z tych zmiennych będzie tablica, a następnie przekroczymy jej zakres
	// będziemy zamazywać kolejne obszary pamięci w stronę wyższych adresów (argumenty main, ...)
	// jest to tak zwany błąd przepełnienia bufora - http://pl.wikipedia.org/wiki/Przepełnienie_bufora
	
	// warto także zwrócić uwagę iż taka metoda alokacji tablic narzuca ograniczenie na ich rozmiar
	// związane z ograniczeniem rozmiaru stosu, dlatego lepiej duże tablice alokować poza stosem
	char d[100];
	char e[atoi(argv[1])];
	
	// gdy jako argument podamy np. 20000000 (20MB)
	// program zakończy się naruszeniem ochrony pamięci
	printf("alokacje tablic\n");
	printf("\t0x%x 0x%x\n", d, e);
	
	// natomiast alokacja mallociem ponad 20 razy większej tablicy jest nieszkodliwa
	char * tab = malloc(sizeof(*tab) * 400000001 );
	tab[1] = 20;
	tab[400000000]=30;
	printf("\t0x%x 0x%x %d %d\n", &tab[1], &tab[400000000], tab[1], tab[400000000]);
	
	// a na koniec napis - jest to w zasadzie tablica, ale jakiego rodzaju ?
	char *bo;
	bo = strstr (napis, "kota");
	
	printf("DŁUGOŚĆ NAPISU poprzez wskaźniki w bajtach a nie znakach (istotna różnica dla kodowań typu UTF8)\n");
	printf("\t0x%x 0x%x %i\n", napis, bo, (int)((size_t)bo - (size_t)napis));
	
	printf("\n\n");
	for (a=0; a<100; a=a+4) {
		d[a] = a;
		d[a+1] = 0;
		d[a+2] = 0xdd;
		d[a+3] = 0xdd;
	}
	b=0xbbbbbbbb;
	c=0xcccccccc;
	
	printf("fragment stosu związany z main()\n");
	for (wsk=&argc; wsk<argv[0]; wsk++)
			printf("0x%x : 0x%x\n", wsk, *wsk);
	// tutaj zaczynają się tablice char
	for (wsk2=argv[0]; wsk2<argv[0]+20; wsk2++)
		printf("0x%x : 0x%x (%c)\n", wsk2, *wsk2, *wsk2);
}
