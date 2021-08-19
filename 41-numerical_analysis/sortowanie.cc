/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody sortowania
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

  
/*                */
/*   SORTOWANIE   */
/*                */

// dane są zdefiniowanym typem struktury zawierającej numeryczne pole klucz oraz pole na dane ...

void sort_wybor(dane a[], int m) {
	int i, j, min, min_index;
	dane tmp;

	for (j=0; j<m; j++) {
		// znajdujemny najmniejszy w pozostalej czesci tablicy
		min = a[j].klucz;
		min_index = j;
		for (i=j+1; i<m; i++) {
			if (a[i].klucz < min) {
				min = a[i].klucz;
				min_index = i;
			}
		}
		// wpisujemy najmniejszy na j-ta pozycje
		if (j != min_index) {
			tmp = a[j];
			a[j] = a[min_index];
			a[min_index] = tmp;
		}
	}
}

void sort_wstaw(dane a[], int m) {
	int i, j=1;
	dane tmp;

	        // znajdujemny najmniejszy i wstawiamy na poczatek
                int min_index = 0;

                for (i=1; i<m; i++) {
                        if (a[i].klucz < a[min_index].klucz) {
                                min_index = i;
                        }
                }
                // wpisujemy najmniejszy na j-ta pozycje
                if (0 != min_index) {
                        tmp = a[0];
                        a[0] = a[min_index];
                        a[min_index] = tmp;
                }
		j=2;


	for (j; j<m; j++){
		i = j-1;
		tmp = a[j];
		while (tmp.klucz < a[i].klucz) {
			a[i+1] = a[i];
			i--;
		}
		a[i+1] = tmp;
	}
}

void sort_Shell(dane a[], int m) {
	int i, j, h;
        dane tmp;

	for (h=1; h<m/9; h=3*h+1);
	for (; h>0; h/=3) {
		// sortowanie przez wstawianie
	        for (j=h; j<m; j++){
        	        i = j-h;
                	tmp = a[j];
	                while (i>=0 && tmp.klucz < a[i].klucz) {
				a[i+h] = a[i];
                	        i-=h;
	                }
        	        if(i!=j)
				a[i+h] = tmp;
	        }
	}
}
