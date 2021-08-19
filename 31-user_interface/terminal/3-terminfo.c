/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody związane z sterowaniem wyświetlaniem na terminalu tekstowym
 * plik ten wykorzystuje metodę opartą w całości na bibliotece terminfo
 * (z tej biblioteki korzysta też ncurses, czyli jest to taka bardziej niskopoziomowa metoda)
 *
 * kompilacja: gcc -lncurses 3-terminfo.c
 */

#include <termios.h>
#include <term.h>

#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h> /* for memset() */

struct termios old_termios, new_termios;
static	int max_x, max_y;
static	char *tig_cup;

/* inicjalizacja terminala */
void inicjalizacja_terminalu() {
	/* oczywiście większość tych funkcji może nie zakończyć się powodzeniem co należałoby sprawdzać ... */
	
	/* otwarcie terminala */
	setupterm(NULL,1,0);
	
	/* zapis starych ustawień terminala */
	tcgetattr(1, &old_termios);
	
	/* wyłączenie wyświetlania wpisywanych znaków */
	// w przykładzie tym modyfikujemy zastane ustawienia ... można jednak zamiast tego wprowadzić zupełnie nowe ...
	memcpy(&new_termios, &old_termios, sizeof(new_termios));
	new_termios.c_iflag &= ~ECHO;
	new_termios.c_oflag &= ~ECHO;
	new_termios.c_cflag &= ~ECHO;
	new_termios.c_lflag &= ~ECHO;
	tcsetattr(1, TCSANOW, &new_termios);
	
	/* zczytanie parametrów terminala */
	max_x=tigetnum("cols");
	max_y=tigetnum("lines");
	
	tig_cup=tigetstr("cup"); /* pozycja kursora (patrz man 5 terminfo),
		inne to m.in. "bold" - przełącznik w tryb gruby, "setf" - ustawianie koloru tła ... */
}

/* wyczyszczenie terminala */
void wyczysc_terminal(void) {
	putp(tparm(tig_cup,0,0)); /* idź na sam początek */
	putp(tparm(tigetstr("ed")));
}

/* wypisanie w zadanym punkcie ekranu,
   warto zwrócić uwagę że jest to sposób tworzenia funkcji opartych na instrukcjach preprocesora
   - przed kompilacją kod funkcji zostanie wstawiony w miejsca jej wywołań */
#define put_s(y,x,s) putp(tparm(tig_cup,y,x)); printf(s);

/* funkcja główna */
int main(void) {
	inicjalizacja_terminalu();
	
	wyczysc_terminal();
	
	put_s(2,8,"@")
	
	/* oczekiwanie na naciśniecie q */
	while (getchar()!='q');
	
	/* przywrócenie ustawień terminala */
	tcsetattr(1, TCSAFLUSH, &old_termios);
	
	return 0;
}
