/*
 * plik ma na celu prezentację ciekawych zagadnień z programowania w języku C / C++
 * prezentuje metodę wyłączania przerwań - przydaje się to gdy na krótko potrzebujemy
 * działania w mocnym trybie czasu rzeczywistego (jest to rozwiązanie dużo mocniejsze
 * niż priorytet -20, gdzie program nawet o dużo niższym (np. dodatnim) priorytecie
 * ma prawo dorwać się do głosu, tu nie może tego zrobić nawet jądro
 * - wszystkie przerwania poza niemaskowalnymi są zablokowane
 */

#include <sys/io.h> // w tym siedzi funkcja iopl();
#include <stdio.h>

int main (int argv, char *args[]) {
	int a=999999999;
	
	iopl (3); // musimy przejąć władzę nad portami ;-),
	          // inaczej mamy błąd ochrony pamięci i nasz wspaniały program z komenda "cli"
	          // nie będzie się uruchamiał, ale do tego to potrzeba nam root'a ...
	
	asm("cli"); // - wyłącza przerwania
	
	while (1) {
		// niestety jeżeli chcielibyśmy coś czytać lub pisać to przerwania bedą włączone :(((
		//scanf("%d",&a);
		//printf("%d", a);
		
		// a to taki bezpiecznik, który zapewni ze komputer się odwiesi ;-)
		a--;
		if (a == 0) break;
	}
	
	asm("sti"); // - włącza przerwania
	
	return 0;
}
