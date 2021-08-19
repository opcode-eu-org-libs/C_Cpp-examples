/*
 * plik ma na celu prezentację podstawowych zagadnień z programowania w języku C / C++
 * prezentuje korzystanie z funkcji parsujących ospcje programu
 *
 * zobacz tez: man getopt_long
 *
 */

// dla wypisywania:
#include <stdio.h>

// dla obsługi opcji:
#include <getopt.h>

// dla CHAR_MAX:
#include <limits.h>

// deklaracje numerów wyboru dla opcji długich nie posiadających krótkich odpowiedników
// CHAR_MAX + 1 => numery zaczynają się tam gdzie kończą się numery
// odpowiadające kodom pojedynczych znaków ...
enum {
	OPCJA_PIERWSZA = CHAR_MAX + 1,
	OPCJA_DRUGA,
	OPCJA_TRZECIA
};

// tablica struktura przechowujących informacje o opcjach programu ...
// każdy element składa się z następujących informacji:
//   nazwa_dluga, czy_wymaga_argumentow, zmienna_do_ustawienia, nazwa krótka
static struct option opcje [] =
{
	{"wymaga-argumentu", required_argument, NULL, 'w'},
	{"moze-miec-argument", optional_argument, NULL, OPCJA_PIERWSZA},
	{"kolejna", no_argument, NULL, OPCJA_DRUGA},
	{"i-jeszcze-jedna", no_argument, NULL, OPCJA_TRZECIA},
	{0, no_argument, NULL, 'v'},
	{0, 0, 0, 0}
};
	
int main(int argc, char *argv[]) {
	int opcja;
	while((opcja = getopt_long(argc, argv, "w:vh", opcje, 0)) != -1) {
		switch (opcja) {
			case 'v':
				printf("Podana opcja -v\n");
			break;
			case 'w':
				printf("Podana opcja -w z argumentem %s\n", optarg);
			break;
			case OPCJA_PIERWSZA:
				printf("Podana opcja --moze-miec-argument z argumentem %s\n", optarg);
				if (!optarg)
					printf("  bez argumentu\n");
			break;
			case OPCJA_DRUGA:
				printf("Podana opcja --kolejna\n");
			break;
			case OPCJA_TRZECIA:
				printf("Podana opcja --i-jeszcze-jedna\n");
			break;
			default:
				printf("nie podano opcji ...\n");	
		}
		printf("  zczytałem opcje do argv[%d] włącznie\n", optind );
	}
	
	return 0;
}
