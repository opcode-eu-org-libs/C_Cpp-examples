/*
 * plik ma na celu prezentację podstawowych zagadnień z metod numerycznych (dla C / C++)
 *
 * prezentuje podstawowe metody numerycznego różniczkowania funkcji
 *
 * Plik stanowi część zbioru funkcji prezentujących podstawy metod numerycznych.
 * Jako że programy te napisałem pewien czas temu nie jestem w stanie obecnie zagwarantować,
 * że działają one w pełni poprawnie. Zachęcam do zapoznania się z skryptem z metod
 * numerycznych dostępnym na stronie https://www.fuw.edu.pl/~pjank/mn/
 *
 */

  
/*                    */
/*   RÓŻNICZKOWANIE   */
/*                    */

// funkcja oblicza pochodna (definicyjnie - ekstrapolujac do zera)
double pochodna(double (*f)(double, double), double k, double x) {
	double g=1, h_tab[5], d_tab[5];
	int i=0;

	for (i=0; i<5; i++) {
		h_tab[i] = g;
		//g = g / 2.0;
		g = g / 4.0;
	}

	g = f(x,k);

	for (i=0; i<5; i++) {
		//d_tab[i] = ( f (x + h_tab[i], k) - g ) / h_tab[i];
		d_tab[i] = ( f (x + h_tab[i], k) - g ) / (2 * h_tab[i]);
	}

	//return interpolacja (0, 5, h_tab, d_tab);
	return 2 * interpolacja_Nevil_2 (0, 5, h_tab, d_tab);
}
