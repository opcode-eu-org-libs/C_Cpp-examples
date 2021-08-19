/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody związane z sterowaniem wyświetlaniem na terminalu tekstowym
 * plik ten wykorzystuje metodę opartą w całości na bibliotece termcap
 * (przy tej metodzie mamy pewność że zawsze powinna zadziałać)
 *
 * komplacja: gcc -ltermcap 2-termcap.c
 */

#include <termcap.h>

#include <stdio.h>

#include <string.h> /* for memset() */
#include <stdlib.h> /* for getenv() */

static	int max_x, max_y;
static	char *tg_cm;

/* inicjalizacja terminala */
void inicjalizacja_terminalu() {
	/* oczywiście większość tych funkcji może nie zakończyć się powodzeniem co należałoby sprawdzać ... */
	/* pobranie informacji o terminalu */
	tgetent (NULL, getenv ("TERM"));
	
	/* zczytanie parametrów terminala */
	max_x=tgetnum("co");
	max_y=tgetnum("li");
	
	tg_cm=tgetstr("cm", 0); /* string używany do ustawiania pozycji kursor */
}

/* wyczyszczenie terminala */
void wyczysc_terminal(void) {
	tputs(tgetstr("cl",0), 1, putchar);
}

/* wypisanie w zadanym punkcie ekranu,
   warto zwrócić uwagę że jest to sposób tworzenia funkcji opartych na instrukcjach preprocesora
   - przed kompilacją kod funkcji zostanie wstawiony w miejsca jej wywołań */
#define put_s(x,y,s) tputs ( tgoto(tg_cm,y,x) , 1, putchar); printf(s);

/* funkcja glowna */
int main(void) {
	inicjalizacja_terminalu();
	
	wyczysc_terminal();
	
	put_s(2,8,"@")
	
	/* oczekiwanie na naciśniecie q */
	while (getchar()!='q');
	
	return 0;
}
