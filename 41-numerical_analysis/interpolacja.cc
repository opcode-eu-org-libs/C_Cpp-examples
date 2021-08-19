/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody interpolacji
 * (obliczania wartosci dla zadanego punktu gdy mamy wartosci w innych punktach)
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */


/*                              */
/*   INTERPOLACJA WIELOMIANEM   */
/*                              */

double interpolacja (double x, int STOPIEN, double tab_x[], double tab_f[]) {
	double wynik = 0, L;
	for (int i=0; i<STOPIEN; i++) {
		L = 1;
	        for (int j=0; j<STOPIEN; j++) {
        	        if (j != i)
                	        L = L * (x - tab_x[j]) / (tab_x[i] - tab_x[j]);
	        }
		wynik = wynik + tab_f[i]*L;
	}
	
	return wynik;
}

// Nevil (wersja 1)
// algorytm Nevila (niezaleznie od wersji implementacji) jest bardzo dobry do ekstrapolacji w zerze
double interpolacja_Nevil_1 (double x, int STOPIEN, double tab_x[], double tab_f[]) {
	double P [STOPIEN] [STOPIEN]; //P[i][j]
	int i,j;

	for (i=0; i < STOPIEN; i++){
		P[i][0] = tab_f[i];
	}

	for (j=1; j < STOPIEN; j++){
		for (i=j; i < STOPIEN; i++){
			//P[i][j] = f ( P[i-1][j-1], P[i][j-1] );
			P[i][j] = ( (x - tab_x[i]) * P[i-1][j-1] + (tab_x[i-j] - x) *  P[i][j-1] ) / (tab_x[i-j] - tab_x[i]);
		}
	}

	return P [STOPIEN-1] [STOPIEN-1];
}

// Nevil (wersja 2 - udoskonalona operująca na mniejszej tablicy)
double interpolacja_Nevil_2 (double x, int STOPIEN, double tab_x[], double tab_f[]) {
	double P[STOPIEN];

	for (int i=0; i < STOPIEN; i++) {
		P[i] = tab_f[i];
		for (int j=i-1; j>=0; j--) {
			// printf("MAMY i = %d\t oraz j = %d a nasze x[i] = %e\t x[i-j] = %e\n",i,j, tab_x[i], tab_x[j]);
			P[j] = ( (x - tab_x[i]) * P[j] + (tab_x[j] - x) *  P[j+1] ) / (tab_x[j] - tab_x[i]);
		}
	}

	return P [0];
}

// double tab_f[STOPIEN]; double tab_x[STOPIEN]; double f[STOPIEN*STOPIEN];
// wynik - wspolczynniki (tab_a dla poniższego algorytmu Hornera) to pierwsze STOPIEN elementów z (pseudo dwuwymiarowej) tablicy f
void interpolacja_Newton (int STOPIEN, double tab_x[], double tab_f[], double *f) {
	int i,j;

	for (i=0; i < STOPIEN; i++){
		f[i*STOPIEN] = tab_f[i];
	}

	for (j=1; j < STOPIEN; j++){
		for (i=0; i < STOPIEN-j; i++){
			f[i*STOPIEN+j] = ( f[(i+1)*STOPIEN+j-1] - f[i*STOPIEN+j-1] ) / ( tab_x[i+j] - tab_x[i] );
		}
	}
}

// funkcja zwraca wartosc wielomianu obliczoną metodą Hornera: a_0 + (x - x_1) * (a_1 + (x - x_2) * (a_2 + (x - x_3) * (a_4 + ... )))
double Horner (int STOPIEN, double x, double tab_x[], double tab_a[]) {
	double wynik=0;

	for (int i=STOPIEN-1; i>0; i--){
		wynik = (tab_a[i] + wynik)*(x-tab_x[i-1]);
	}
	wynik = wynik + tab_a[0];

	return wynik;
}



/*                                  */
/*   DOPASOWANIE GLADKIE - SPLINE   */
/*                                  */

// plik z funkcja rozwiazujaca uklad rownan o macierzy trujdiagonalnej (bez zer na diagonali) metoda Thomasa
// UWAGA: program niszczy dane wejsciowe
void Thomas(double a[], double b[], double c[], double r[], int m) { // m = liczba elementow tablicy b - 1
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

// wymaga wczesniejszego zdeklarowania i przekazania tablic: double a[n-1], b[n], c[n-1];
// gdzie n - ilosc danych wejsciowych
void wspolczynniki_spline (double x_tab[], double y_tab[], double a[], double b[], double c[], int n) {
	// przygotowanie dla alg. Thomasa
	double r[n], h[n];
	int i;

	h[0] = 0;
	for (i=1; i<n; i++) h[i] = x_tab[i] - x_tab[i-1];

	for (i=0; i<n-2; i++) a[i] = h[i+1] / (h[i+1] + h[i+2]);

	for (i=0; i<n; i++) b[i] = 2;

	c[0] = 0;
	for (i=1; i<n-2; i++) c[i] = h[i+1] / (h[i] + h[i+1]);

	r[0]=0;	r[n-1]=0;
	for (i=1; i<n-1; i++) r[i]=((y_tab[i+1]-y_tab[i]) / h[i+1] - (y_tab[i]-y_tab[i-1]) / h[i]) * 6 / (h[i] + h[i+1]);

	// alg. Thomasa
	Thomas(a, b, c, r, n-1);

	// wyznaczenie parametrow
	for (i=0; i<n-1; i++) {
		a[i] = (r[i+1] - r[i]) / (6 * h[i+1]);
		b[i] = r[i] / 2;
		c[i] = (y_tab[i+1] - y_tab[i]) / h[i+1]  -  h[i+1] * (2*r[i] + r[i+1]) / 6;
	}
	
}

// obliczamy wartosci wielomianu z wspolczynnikami a[i], b[i], c[i] , d[i]
// dla zadanego x (bierzemy stowane wspolczynniki, gdy x nalezy do i-tego odcinka
// uzyskujemy aproksymowaną wartosc y(x)
void spline (double x_tab[], double y_tab[], double a[], double b[], double c[], double x) {
	if (x >= x_tab[0] && x <= x_tab[n-1]) {
		i = 1;
		while (x > x_tab[i]) i++;
		// x nalezy do przedzialu x[i-1] do x[i]
		i--;

		// wynik dla zadanego x
		return y_tab[i] + ( c[i] + ( b[i] + a[i] * (x-x_tab[i]) ) * (x-x_tab[i]) ) * (x-x_tab[i]);		
	}
} 
