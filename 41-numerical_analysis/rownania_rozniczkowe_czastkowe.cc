/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody numerycznego rozwiązywania równań różniczkowych cząstkowych
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

#include "alg_thomasa.c"

void ftcs(double T[], int ilosc_x, int ilosc_t, double krok_x, double krok_t) {
	int i, j;

	double r = krok_t/(krok_x * krok_x);

	//pierwszy wiersz mamy z warunku brzegowego
	for (i=1; i<=ilosc_t; i++) {
		//piersza i ostatnia kolumne mamy z war. brzegowego
		for (j=1; j<ilosc_x; j++) {

			T [ i*ilosc_x + j ] = \
				r * T [ (i-1)*ilosc_x + j-1 ] + \
				(1-2*r) * T [ (i-1)*ilosc_x + j ] + \
				r * T [ (i-1)*ilosc_x + j+1 ];

		}
	}
}

void ftcs_mem(double T[], int ilosc_x, int ilosc_t, double krok_x, double krok_t) {
	int i, j;

	double r = krok_t/(krok_x * krok_x);
	double T_poprzedni, T_poprzedni_tmp;

	//pierwszy wiersz mamy z warunku brzegowego
	for (i=1; i<=ilosc_t; i++) {
		//piersza i ostatnia kolumne mamy z war. brzegowego
		T_poprzedni=T[0];
		for (j=1; j<ilosc_x; j++) {
			T_poprzedni_tmp=T[j];
		
			T [ j ] = \
				r * T_poprzedni + \
				(1-2*r) * T [ j ] + \
				r * T [ j+1 ];

			T_poprzedni=T_poprzedni_tmp;
		}
	}
}

void ftcs_mem2(double T[], double L[], double P[],  int ilosc_x, int ilosc_t, double krok_x, double krok_t) {
	int i, j;

	double r = krok_t/(krok_x * krok_x);
	double T_poprzedni, T_poprzedni_tmp;

	//pierwszy wiersz mamy z warunku brzegowego
	for (i=1; i<=ilosc_t; i++) {
		//piersza i ostatnia kolumne mamy z war. brzegowego
		T_poprzedni = L[i-1];
		T [ ilosc_x ] = P[i-1];

		for (j=1; j<ilosc_x; j++) {
			T_poprzedni_tmp=T[j];
		
			T [ j ] = \
				r * T_poprzedni + \
				(1-2*r) * T [ j ] + \
				r * T [ j+1 ];

			T_poprzedni=T_poprzedni_tmp;
		}
	}
}

void cn(double T[], double L[], double P[],  int ilosc_x, int ilosc_t, double krok_x, double krok_t) {
	int i, j;

	double A, B, A2, B2, T_poprz, T_poprz_tmp, r[ ilosc_x ], b [ ilosc_x ];

	A = krok_t/(krok_x * krok_x);
	B = 1+A;
	A2 = A/2.0;
	B2 = 1-A;
	A = -A/2.0;

	for (i=1; i<=ilosc_t; i++) {
		T_poprz=T[0];
		for (j=1; j<ilosc_x; j++) {
			T_poprz_tmp = T[j];
			T[j] = A2 * (T_poprz + T[j+1]) + B2 * T[j];
			T_poprz=T_poprz_tmp;
			b[j] = B;
		}
		T[1] -= A*L[j-1];
		T[ilosc_x-1] -= A*P[j-1];

		T[0]=L[j-1]; T[ilosc_x]=P[j-1]; // dla nastepnego kroku

//TEST
//b[1]=2; b[2]=2; A=4; T[1]=2; T[2]=10; ilosc_x=3;
for (j=0;j<=ilosc_x;j++) printf(" %f * ", T[j]);
printf("\n\n");
		// rozwiazujemy uklad rownan
		T[1] /= b[1];
		for (j=2; j<ilosc_x; j++) {
			b[j] -= A * A / b[j-1];
			T[j] = (T[j] - A*T[j-1]) / b[j];
		}
		for (j=ilosc_x-1; j>0; j--) {
			T[j] = T[j] - A*T[j+1]/b[j];
		}
	}
}
