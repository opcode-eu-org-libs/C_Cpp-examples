/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody związane z sterowaniem wyświetlaniem na terminalu tekstowym
 * plik ten wykorzystuje metodę opartą w całości na bibliotece ncurses
 *
 * kompilacja: gcc -lncurses 4-ncurses.c
 */

#include <ncurses.h>
/* Wymagane przy linkowaniu */

int main(void) {
	initscr(); /* inicjalizacja */
	noecho(); /* nie będzie widać wpisywanych znaków */
	curs_set(false); /* nie pokazuj kursora */
	
	// ncurses pozwala na łatwe manipulowanie ustawieniami uzyskiwanymi
	// zarówno poprzez konfigurację urządzenia tty (via tcsetattr) – np. no echo
	// jak i tymi związanymi z sekwencjami sterującymi ANSI – np. ukrycie kursora
	
	/* można by jeszcze używać m.in.:
		cbreak(); - wyłącza buforowanie linii
		nonl(); - sterowanie wejściem
		intrflush(stdscr, FALSE); - sterowanie wejściem
		keypad(stdscr, TRUE); - sterowanie wejściem
	*/
	
	/* wypisz znak na zadanej pozycji */
	mvaddch(2,8,'@');
	
	/* oczekiwanie na naciśnięcie q */
	while (getch()!='q');
	
	curs_set(true); /* pokazuj normalnie kursor */
	endwin(); /* zakończ */
	
	return 0;
}
