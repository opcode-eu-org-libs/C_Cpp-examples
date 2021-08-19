/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody numerycznego całkowania funkcji
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

/*                        */
/*   CALKOWANIE - GAUSS   */
/*                        */

// deklaracje dla n = 4
static double wezel4[] = {0.3399810435, -0.3399810435, 0.8611363115, -0.8611363115};
static double waga4[] = {0.6521451548, 0.6521451548, 0.3478548451, 0.3478548451};

// deklaracje dla n = 2
static double wezel2[] = {0.5773502691, -0.5773502691};
static double waga2[2] = {1.0, 1.0};

// deklaracje uniwersalne
static double *wezly[] = {0, 0, wezel2, 0 , wezel4};
static double *wagi[] = {0, 0, waga2, 0, waga4};

double calka_Gauss (double (*f)(double, double), double a, double b, int n, double k) {

	if (n != 2 && n != 4) {
		return 0; // BLAD
	}

	double I=0;

	for (int i=0; i<n; i++) {
		I = I + wagi[n][i] * f( (b-a) * wezly[n][i] / 2 + (a+b)/2 , k);
	}

	I = I * (b-a) / 2;

	return I;
}

// rekurencyjne "precyzjowanie" obliczonej calki (gdy maga dokladnosc na danym odcinku badamy go poprzez podzial na coraz mniejsze)
// korzystajace z powyzszej funkcji: calka_Gauss
double calka_Gauss_rekurencja (double (*f)(double, double), double a, double b, double k, double eps, int stop) {

	double cal = gauss(f, a, b, stop, k), cal_1 = gauss(f, a, (a+b)/2.0, stop, k), cal_2 = gauss(f, (a+b)/2.0, b, stop, k);

        if ( (fabs(cal) < fabs(cal_1 + cal_2) + eps) && (fabs(cal) > fabs(cal_1 + cal_2) - eps) ) {
                return cal;
        } else {
                return calka(f, a, (a+b)/2, k, eps, stop) + calka(f, (a+b)/2, b, k, eps, stop);
        }
}



/*                      */
/*   CALKOWANIE -       */
/*                      */
static int sigma_1[] = {1, 1};
static int sigma_2[] = {1, 4, 1};
static int sigma_3[] = {1, 3, 3, 1};
static int sigma_4[] = {7, 32, 12, 32, 7};
static int sigma_5[] = {19, 75, 50, 50, 75, 19};
static int sigma_6[] = {41, 216, 27, 272, 27, 216, 41};

// deklaracje uniwersalne
static int *sigmy[] = {0, sigma_1, sigma_2, sigma_3, sigma_4, sigma_5, sigma_6};
static int ns[] = {0, 2, 6, 8, 90, 288, 840};

double calkowanie(double f_i[], double a, double b, int n) {
	double suma = 0;
	for (int i=0; i<=n; i++) {
		suma = suma + (double)sigmy[n][i] * f_i[i];
	}

	double wynik;
	wynik = suma * (b-a) / ns[n];

	return wynik;
}

double calka(double (*f)(double, double), double a, double b, double k, double eps) {
	double f_i[2];
	double s = 0, s_old=0, nowe, x;

	for (int i=0; i<=1; i++) {
		f_i[i] = f( a + i * (b-a) , k );
	}
	s = calkowanie (f_i, a, b, 1);

	int iteracja = 0;
	while (fabs(s) < eps || fabs(s) > fabs(s_old) + eps || fabs(s) < fabs(s_old) - eps ) {
		iteracja++;
		s_old = s;

		nowe = 0;
		x = a + (b-a)/(2*iteracja);
		for (int i=0; i<iteracja; i++) {
			nowe = nowe + f(x, k);
			x = x + (b-a) / iteracja;
		}
		s = s / 2  +  (b-a) / (2*iteracja) * nowe;
	}
	return s;
}

// Romberg - ekstrapolacja z kilku iteracji powyzszej funkcji z h -> 0
double romberg(double (*f)(double, double), double a, double b, double k, double eps) {
        double f_i[2];
	double s_tab[5], h_tab[5];
	double s = 0, nowe, x;

	for (int i=0; i<=1; i++) {
		f_i[i] = f( a + i * (b-a) , k );
	}
	s = calkowanie (f_i, a, b, 1);


	for (int iteracja=0; iteracja<5; iteracja++) {
		nowe = 0;
		x = a + (b-a)/(2*iteracja);
		for (int i=0; i<iteracja; i++) {
			nowe = nowe + f(x, k);
			x = x + (b-a) / iteracja;
		}
		s = s / 2  +  (b-a) / (2*iteracja) * nowe;
		s_tab[iteracja] = s;
		h_tab[iteracja] = (b-a) / (2*iteracja);
	}

	// a teraz ekstrapolacja s prz h dazacym do zera
	// korzystamy do tego z algorytmu Nevilla

	return interpolacja_Nevil_2 (0, 5, h_tab, s_tab);
}
