/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody rozwiązywania układów równań typu A * x = b,
 * gdzie A - macierz n x n, x i b - wektory o długości n
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

// zamiana macierzy na macierz trojkatna ...
void gauss(double A[], int n) {
	double dzielnik;
	int tmp;
	double wiersz_tmp;
	for (int odejmowany=0; odejmowany<n; odejmowany++) { 
		for (int i=odejmowany+1; i<n; i++) { // po wierszach bez aktualnie odejmowanego
			if (A[i*n + odejmowany] == 0) {
				tmp = i + 1;
				while (A[n * (tmp++) + odejmowany] == 0);
				if (tmp-- == n) continue;
				//zamieniamy
				for (int j=odejmowany; j<n; j++) {
					wiersz_tmp = A[i*n + j];
					A[i*n + j] = A[tmp*n + j];
					A[tmp*n + j] = wiersz_tmp;
				}
			}
			dzielnik = A[odejmowany*n + odejmowany] / A[i*n + odejmowany];
			for (int j=odejmowany; j<n; j++) { // po kolumnach w aktualnym wierszu, od aktualnie zerowanej
				A[i*n + j] = A[odejmowany*n +j] - A[i*n + j]*dzielnik;
			}
		}
	}
}

// rozwiazywanie ukladu z macierza trojkatna
void trojkatny(double A[], double b[], double x[], int n) { // jezeli b nie bedzie nam potrzebne to mozna wywolywac z x=b
	double suma;

	for (int i = n-1; i >= 0; i--) {
		/* x[n-1] = (b[n-1] - a[n-1][n] x[n])/a[n-1][n-1] */

		suma = 0;
		for (int j = i+1; j < n; j++) {
			suma += A[ i*n + j ] * x[j];
//			printf("%d -> %d : %f\n", i, j, A[ i*n + j ]);
		}

		x[i] = (b[i] - suma ) / A[ i*n + i ];
	}
}

// funkcja rozwiazujaca uklad rownan o macierzy trujdiagonalnej (bez zer na diagonali) metoda Thomasa
// UWAGA: funkcja niszczy dane wejsciowe
void thomas(double a[], double b[], double c[], double r[], int m) { // liczba elementow tablicy b - 1
	int j;

	c[0] = c[0] / b[0];
	r[0] = r[0] / b[0];
	for (j=1; j<m; j++) {
		b[j] = b[j] - a[j-1] * c[j-1];
		c[j] = c[j] / b[j];
		r[j] = (r[j] - a[j-1] * r[j-1]) / b[j];
	}
	b[m] = b[m] - a[m-1] * c[m-1];
	r[m] = (r[m] - a[m-1] * r[m-1]) / b[m];
	for (j=m-1; j>=0; j--) {
		r[j] = r[j] - c[j] * r[j+1];
	}
}

// metoda LU
void lu(double A[], int n) {
	int i, j, k;
	double suma;

	for (j=0; j<n; j++)
	for (i=0; i<n; i++)
	{
		suma = 0;
		if (i<=j) {
			for (k=0; k<i; k++)
				suma += A [i*n + k] * A [k*n + j];
			A [i*n + j] = A [i*n + j] - suma;
		} else {
			for (k=0; k<j; k++)
				suma += A [i*n + k] * A [k*n + j];
			A [i*n + j] = (A [i*n + j] - suma) / A [j*n + j];
		}
//		printf("obliczylem element %d %d\n", i, j);
	}
}

// iteracyjna metoda rozwiazywania układów równań
int jacobi(double A[], double b[], double x[], int n, double eps, int limit) {
	int k, j, iteracja = 0;
	bool koniec = false;
	double suma;
	double x_tmp[n];

	while (!koniec && iteracja++ < limit) {
		for (j=0; j<n; j++)
			x_tmp[j] = x[j];

		for (j=0; j<n; j++) {
			koniec = true;
			suma = 0;

			for (k=0; k<n; k++)
				if (k != j) suma += x_tmp[k] * A [j*n + k];

			x[j] = ( b[j] - suma ) / A [j*n + j];
			if ( fabs(x_tmp[j] - x[j]) > eps ) koniec = false;
		}
	}
	if (koniec)
		return iteracja;
	else
		return -1;
}

int nadrelaksacja(double A[], double b[], double x[], double omega, int n, double eps, int limit) {
	int k, j, iteracja = 0;
	bool koniec = false;
	double suma;
	double x_tmp[n];

	while (!koniec && iteracja++ < limit) {
		for (j=0; j<n; j++)
			x_tmp[j] = x[j];

		for (j=0; j<n; j++) {
			koniec = true;
			suma = 0;

			for (k=0; k<j; k++)
				 suma += x[k] * A [j*n + k];

			for (k=j; k<n; k++)
				 suma += x_tmp[k] * A [j*n + k];

			x[j] = x_tmp[j] + omega * ( b[j] - suma ) / A [j*n + j];
			if ( fabs(x_tmp[j] - x[j]) > eps ) koniec = false;
		}
	}
	if (koniec)
		return iteracja;
	else
		return -1;
}

int gauss(double A[], double b[], double x[], int n, double eps, int limit) {
	int k, j, iteracja = 0;
	bool koniec = false;
	double suma;
	double x_tmp[n];

	while (!koniec && iteracja++ < limit) {
		for (j=0; j<n; j++)
			x_tmp[j] = x[j];

		for (j=0; j<n; j++) {
			koniec = true;
			suma = 0;

			for (k=0; k<j; k++)
				 suma += x[k] * A [j*n + k];

			for (k=j+1; k<n; k++)
				 suma += x_tmp[k] * A [j*n + k];

			x[j] = ( b[j] - suma ) / A [j*n + j];
			if ( fabs(x_tmp[j] - x[j]) > eps ) koniec = false;
		}
	}
	if (koniec)
		return iteracja;
	else
		return -1;
}
