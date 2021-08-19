/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody poszukiwania miejsc zerowych
 * (czyli numerycznego rozwiązywania równań ...)
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */


/*                                 */
/*   ZNAJDOWANIE MIEJSC ZEROWYCH   */
/*                                 */

// funkcja znajduje miejsca zerowe metodą bisekcji (szukania punktu zmiany znaku)
double miejsca_zerowe_bisekcja(double (*f)(double, double), double k, double x1, double x2, double eps) {
	double x, f1, f2, fx;

	if( f(x1, k) * f(x2, k) > 0 ) {
		printf("Blaedne dane\n");
		return sqrt(-2);
	}

	while ( fabs(f(x, k)) > 0 ) {
		f1 = f(x1, k);
		f2 = f(x2, k);
		x = (x1 + x2) / 2.0;
		fx = f(x, k);

		if (fabs(fx) < eps) {
			return x;
		} else if (fx*f1 > 0) {
			x1 = x;
		} else if (fx*f2 > 0) {
			x2 = x;
		}
	}
}

// funkcja znajduje miejsca zerowe metodą pochodnej (Newtona)
double miejsca_zerowe_Newton(double (*f)(double, double), double (*df)(double, double), double k, double x, double eps) {
	while( fabs(f(x, k)) > eps ) {
		x = x - f(x,k) / df(x,k);

	}

	return x;
}

// funkcja znajduje miejsca zerowe metodą siecznych
double miejsca_zerowe_sieczne(double (*f)(double, double), double k, double x1, double x2, double eps) {
	double x, fx, f1, f2;

	f1 = f(x1, k);
	f2 = f(x2, k);

	do {
		x = x1 - f1 * (x2 - x1) / (f2 - f1);
		fx = f(x, k);

		if (f1*fx>0) {
			x2 = x;
			f2 = fx;
		} else {
			x1 = x;
			f1 = fx;
		}
	} while( fabs(f(x, k)) > eps );

	return x;
}
 
