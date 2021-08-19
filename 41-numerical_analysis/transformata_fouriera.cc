/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje transformatę Fouriera ...
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

#include <complex>
#include <cmath>

void ft(complex<double> h[], complex<double> H[], int N) {
	int n,k;
	
	complex<double> w(0, 2*M_PI/N), wn;
	w = exp(w);

	for (n=0; n<N; n++) {
		wn = pow(w,n);
		H[n] = 0;
		for (k=0; k<N; k++) {
			H[n] += pow(wn,k) * h[k];
		}
	}
}


void oft(complex<double> H[], complex<double> h[], int N) { // odwrotna ...
	int n,k;
	
	complex<double> w(0, -2*M_PI/N), wn;
	w = exp(w);

	for (n=0; n<N; n++) {
		wn = pow(w,n);
		h[n] = 0;
		for (k=0; k<N; k++) {
			h[n] += pow(wn,k) * H[k];
		}
		h[n] /= N;
	}
}

