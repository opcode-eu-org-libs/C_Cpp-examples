/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody poszukiwania wartości własnych macierzy
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */
 
  
void pomnoz (double A[], double v[], double wynik[], int wymiar) {
// mnozy macierz [nxn] razy wektor [n]
	double suma;

	for (int wiersz=0; wiersz<wymiar; wiersz++) {
		suma = 0;
		for (int kolumna=0; kolumna<wymiar; kolumna++) {
			suma += A[wiersz*wymiar + kolumna] * v[kolumna];
		}
		wynik[wiersz] = suma;
	}
}

int maxww(double A[], double v[], int n, double eps, int limit, double *x) {
	double v_new[n], norma_new, norma;
	int liczba_iteracji=1, i;

	// rozklad (A - stala * Id) na gorno i dolno trojkatna -- wykorzystac program przykladowy LU

	while(true) {
		pomnoz (A, v, v_new, n);
		norma_new = 0;
		for (i=0; i<n; i++) {
			norma_new += v_new[i]*v_new[i];
		}

		norma = 0;
		for (i=0; i<n; i++) {
			v_new[i] = v_new[i] / sqrt(norma_new);
			norma += (v[i]-v_new[i])*(v[i]-v_new[i]);
		}
		if (norma > eps) {
			// sprawdzamy czy nie za dlugo
			if (liczba_iteracji++ > limit) return -1;
		} else {
			// mamy wynik !!!!
			for (i=0; i<n; i++) {
				norma = 0; // norma to teraz suma
				for (int j=0; j<n; j++) {
					norma += v_new[j] * A[j*n + i];
				}
				v[i] = norma; // v to teraz wynik mnozenia v_new^T * A
			}
			*x=0;
			for (i=0; i<n; i++)
				*x += v[i]*v_new[i];
			norma=0;
			for (i=0; i<n; i++)
				norma += v_new[i]*v_new[i]; // norma to teraz dzielnik

			*x = *x / sqrt(norma);
			return liczba_iteracji;
		}

		// jezeli nadal kontynuujemy to przygotowywujemy nastepny krok
		norma = norma_new;
		for (i=0; i<n; i++)
			v[i] = v_new[i];
	}
}
