/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody numerycznego rozwiązywania równań różniczkowych zwyczajnych
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

typedef void (*func)(double, double*, double*);

void euler (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double z[n];

	// Euler
	for (j=0; j<ile_krokow; j++) {
		f(x, y, z);
		for (i=0; i<n; i++) y[i] = y[i] + krok * z[i];
		x += krok;
	}
}


void midpoint (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double z[n], z2[n], krok_pol=krok / 2.0;

	// punkt srodkowy
	for (j=0; j<ile_krokow; j++) {
		f(x, y, z);

		for (i=0; i<n; i++) z[i] = y[i] + krok_pol * z[i];
		f(x + krok_pol, z, z2);

		for (i=0; i<n; i++) y[i] = y[i] + krok * z2[i];
		x += krok;
	}
}

void heun (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double z[n], z2[n], z3[n], krok_pol=krok / 2.0;

	// Heun
	for (j=0; j<ile_krokow; j++) {
		f(x, y, z);

		for (i=0; i<n; i++) z3[i] = y[i] + krok * z[i];
		f(x + krok, z3, z2);

		for (i=0; i<n; i++) y[i] = y[i] + krok_pol * (z[i] + z2[i]);
		x += krok;
	}
}

void rk4 (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double k1[n], k2[n], k3[n], k4[n], tmp[n];

	// kolejne kroki
	for (j=0; j<ile_krokow; j++) {
		f(x, y, k1);
		for (i=0; i<n; i++) {
			k1[i] *= krok; // finalny k1

			tmp[i] = y[i] + k1[i] / 2;
		}
		f(x + krok / 2, tmp, k2);
		for (i=0; i<n; i++) {
			k2[i] *= krok; // finalny k2

			tmp[i] = y[i] + k2[i] / 2;
		}
		f(x + krok / 2, tmp, k3);
		for (i=0; i<n; i++) {
			k3[i] *= krok; // finalny k3

			tmp[i] = y[i] + k3[i];
		}
		f(x + krok, tmp, k4);
		for (i=0; i<n; i++) {
			k4[i] *= krok; // finalny k4

			y[i] += k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
		}

		x += krok;
	}
}

#define EPSILON 0.00000001
void rk4_adapt (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double k1[n], k2[n], k3[n], k4[n], tmp[n], y1[n], y2[n];
	double Z, pol_krok, x_max = krok*ile_krokow; // dla zachowania kompatybilnosci wstecznej postaci wywolania funkcji

	// kolejne kroki
	while (x <= x_max) {
	   // dla CALEGO kroku
		f(x, y, k1);
		for (i=0; i<n; i++) {
			k1[i] *= krok; // finalny k1

			tmp[i] = y[i] + k1[i] / 2;
		}
		f(x + krok / 2.0, tmp, k2);
		for (i=0; i<n; i++) {
			k2[i] *= krok; // finalny k2

			tmp[i] = y[i] + k2[i] / 2;
		}
		f(x + krok / 2.0, tmp, k3);
		for (i=0; i<n; i++) {
			k3[i] *= krok; // finalny k3

			tmp[i] = y[i] + k3[i];
		}
		f(x + krok, tmp, k4);
		for (i=0; i<n; i++) {
			k4[i] *= krok; // finalny k4

			y1[i] = y[i] + k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
		}

	   // dla POL kroku
		pol_krok = krok/2.0;
		// pierwszy raz
		f(x, y, k1);
		for (i=0; i<n; i++) {
			k1[i] *= pol_krok; // finalny k1

			tmp[i] = y[i] + k1[i] / 2;
		}
		f(x + pol_krok / 2.0, tmp, k2);
		for (i=0; i<n; i++) {
			k2[i] *= pol_krok; // finalny k2

			tmp[i] = y[i] + k2[i] / 2;
		}
		f(x + pol_krok / 2.0, tmp, k3);
		for (i=0; i<n; i++) {
			k3[i] *= pol_krok; // finalny k3

			tmp[i] = y[i] + k3[i];
		}
		f(x + pol_krok, tmp, k4);
		for (i=0; i<n; i++) {
			k4[i] *= pol_krok; // finalny k4

			y2[i] = y[i] + k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
		}
		// drugi raz
		f(x + pol_krok, y2, k1);
		for (i=0; i<n; i++) {
			k1[i] *= pol_krok; // finalny k1

			tmp[i] = y2[i] + k1[i] / 2;
		}
		f(x + pol_krok + pol_krok / 2.0, tmp, k2);
		for (i=0; i<n; i++) {
			k2[i] *= pol_krok; // finalny k2

			tmp[i] = y2[i] + k2[i] / 2;
		}
		f(x + pol_krok + pol_krok / 2.0, tmp, k3);
		for (i=0; i<n; i++) {
			k3[i] *= pol_krok; // finalny k3

			tmp[i] = y2[i] + k3[i];
		}
		f(x + krok, tmp, k4);
		for (i=0; i<n; i++) {
			k4[i] *= pol_krok; // finalny k4

			y2[i] = y2[i] + k1[i]/6 + k2[i]/3 + k3[i]/3 + k4[i]/6;
		}

/*		for (i=0; i<n; i++) y2[i]=y[i];
		rk4 (krok / 2.0, 2, x, y2, n, f);
		for (i=0; i<n; i++) y1[i]=y[i];
		rk4 (krok, 1, x, y1, n, f);*/

	   // sprawdzam czy krok byl dobry i podejmuje odpowiednie dzialania ...
		// obliczam Z
		Z = 0;
		for(i=0; i<n; i++) Z += pow(y1[i]-y2[i] , 2);
		Z = pow(16.0/15.0 * sqrt(Z) / EPSILON , 1.0/5.0);

		if (Z > 3) {
			krok *= 2 / Z;
		} else {
			for(i=0; i<n; i++) y[i]=y1[i];
			x += krok;
			krok *= 2 / Z;

		#ifdef _WYDRUK_WYNIKOW_POSREDNICH_
			_WYDRUK_WYNIKOW_POSREDNICH_
		#endif

		}
//printf("x=%f\tZ=%f\tkrok(nowy)=%f\ty1=%f\ty2=%f\n", x, Z, krok, y1[0], y2[0]);
	}
}

void adams2 (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double z1[n], z2[n], krok_pol=krok / 2.0;

	// z innej metody pozyskujemy 2 startowe z
	heun (krok, 1, x, y, n, f); f(x, y, z2); x += krok;
	heun (krok, 1, x, y, n, f); f(x, y, z1); x += krok;
	ile_krokow -= 2;

	// kolejne kroki
	for (j=0; j<ile_krokow; j++) {
		for (i=0; i<n; i++) {
			y[i] += krok_pol * (3*z1[i] - z2[i]); // z1 = f[n], z2 = f[n-1]
			z2[i] = z1[i];
		}
		f(x, y, z1); // generujemy nowy z1
		x += krok;
	}
}

void adams4 (double krok, int ile_krokow, double x, double y[], int n, func f) {
	int i, j;
	double z1[n], z2[n], z3[n], z4[n], krok_pol=krok / 24.0;

	// z innej metody pozyskujemy 4 startowe z
	rk4 (krok, 1, x, y, n, f); f(x, y, z4); x += krok; 
	rk4 (krok, 1, x, y, n, f); f(x, y, z3); x += krok;
	rk4 (krok, 1, x, y, n, f); f(x, y, z2); x += krok;
	rk4 (krok, 1, x, y, n, f); f(x, y, z1); x += krok;
	ile_krokow -= 4;

	// kolejne kroki
	for (j=0; j<ile_krokow; j++) {
		for (i=0; i<n; i++) {
			y[i] += krok_pol * (55*z1[i] - 59*z2[i] + 37*z3[i] - 9*z4[i]); // z = f[n], z2 = f[n-1], ...
			z4[i] = z3[i];
			z3[i] = z2[i];
			z2[i] = z1[i];
		}
		f(x, y, z1); // generujemy nowy z1
		x += krok;
	}
}

void pk (double krok, int ile_krokow, double x, double y[], int n, func f) { // predyktor korektor
	int i, j, k;
	double y_org[n], z1[n], z2[n], z3[n], krok_pol = krok/12.0;

	// kolejne kroki
	for (j=0; j<ile_krokow; j++) {
		for (i=0; i<n; i++) y_org[i] = y[i];
		//adams2 (krok, 1, x, y_jaw, n, f) - PREDYKTOR

			// z innej metody pozyskujemy 2 startowe z
			heun (krok, 1, x, y, n, f); f(x, y, z2); x += krok;
			heun (krok, 1, x, y, n, f); f(x, y, z1); x += krok;

			for (i=0; i<n; i++) y[i] += krok/2.0 * (3*z1[i] - z2[i]); // z1 = f[n], z2 = f[n-1]

			for (k=0; k<3; k++) {
				f(x, y, z3); // generujemy z3 = f[n+1]
		// A-M rz 3 - KOREKTOR
				for (i=0; i<n; i++) y[i] = y_org[i] + krok_pol * (5*z3[i] + 8*z1[i] - z2[i]);
			}
	}
}
