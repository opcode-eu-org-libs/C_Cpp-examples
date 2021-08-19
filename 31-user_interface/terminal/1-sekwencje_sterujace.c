/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metody związane z sterowaniem wyświetlaniem na terminalu tekstowym
 * plik ten wykorzystuje bezpośrednio sekwencje sterujące terminalem
 * (bez dodatkowych bibliotek i sprawdzania czy są one wspierane)
 *
 * więcej informacji: http://blog.opcode.eu.org/2021/07/25/sekwencje_sterujace_terminala.html
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main() {
	// wyłączenie echo (wyświetlania wpisywanego tekstu)
	// oczywiście można to zrobić bezpośrednio kodem C, bez wywołania zewnętrznego programu ...
	// przykład poniżej (przy włączaniu echo)
	system("stty -echo");
	
	// wyczyszczenie ekranu
	puts("\033[2J");
	// ukrycie kursora
	puts("\033[?25l");
	// wypisanie @ w linii 2 w kolumnie 8
	puts("\033[2;8H@");
	
	while (getchar()!='q');
	
	// pokazywanie kursora
	puts("\033[?25h");
	
	// włączenie echo – zamiast system("stty echo") użyjemy kodu w C:
	system("stty echo");
	struct termios opcje;
	tcgetattr(1, &opcje);  // odczyt ustawień terminala
	opcje.c_cflag |= ECHO;
	opcje.c_lflag |= ECHO;
	tcsetattr(1, TCSANOW, &opcje);
}
