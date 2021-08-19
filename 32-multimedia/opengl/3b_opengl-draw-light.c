/**

plik odpowiedzialny za obsługę "czystego" OpenGL - rysowanie
w tym przykładzie ukazane jest korzystanie z świateł 

UWAGA:
Są to dość stare przykłady korzystające z Fixed Function Pipeline nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. Ogre 3D).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

**/


// warto zwrócić uwagę na możliwość wykorzystania tekstur zawierających
// informację o jasności) do uzyskiwania efektów oświetleniowych,
// metoda ta pozwala w łatwy sposób ominąć ograniczenie systemu oświetlenia
// polegające na obliczaniu światła tylko dla wierzchołków, jednak nie pozwala
// na dynamiczne oświetlenie sceny

/// nagłowki ogólne
#include <math.h>
#include <string.h>

/// zmienna globalna przechowująca licznik klatek
int licznik_klatek = 0;

// funkcja wypisująca napis (tylko GLUT)
void umiesc_napis(char *tekst, GLfloat x, GLfloat y) {
#ifdef GLUT_INIT
	glColor3f(1.0f, 0.0f, 1.0f);
	glRasterPos3f(x, y, -5.0f);
	
	int i, dlugosc = strlen(tekst);
	for (i = 0; i < dlugosc; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, tekst[i]);
#endif
}

/// funkcja przygotowująca scenę
#define STOZEK 11 // coś >0

void przygotuj_scene(void) {
	// przygotowujemy obiekt stożka
	// aby w każdej ramce nie musieć przeliczać stożka
	// (funkcje trygonometryczne nie są szybkie)
	// przygotowujemy go wcześniej i traktujemy jako gotowy obiekt
	glNewList(STOZEK, GL_COMPILE);
	// rysować będziemy kółko złożone z trójkątów
	glBegin( GL_TRIANGLE_FAN );
		// środek (wspólny wierzchołek wszystkich trójkątów)
		glColor3f(1.0f,1.0f,0.0f); glVertex3f( 0.0f, 0.0f, 2.0f);
		// punkty na okręgu (pozostałe wierzchołki trójkątów)
		GLint i;
		for (i=0; i<20; i++) {
			glColor3f(sin(i*0.0785f),cos(i*0.0785f),cos(1.57f-i*0.0785f));
			glVertex3f( sin(i*0.157f), cos(i*0.157f), 0.0f);
		}
		for (i=20; i<=41; i++) {
			glColor3f(1-cos(i*0.0785f),1-sin(i*0.0785f),sin(1.57f-i*0.0785f));
			glVertex3f( sin(i*0.157f), cos(i*0.157f), 0.0f);
		}
	glEnd( );
	glEndList();
	
	/// ustawienia świateł
	// reflektor (Spot Light)
	float kolor[] = {0.0f, 1.0f, 0.5f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, kolor);
	float pozycja[] = {0.0f, 0.0f, 4.0f, 1.0f};
	// ostatnia wartość =1.0f oznacza umieszczenie źródła na scenie
	glLightfv(GL_LIGHT0, GL_POSITION, pozycja);
	float kierunek[] = {0.0f, 0.0f, -1.0f};
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, kierunek);
	// kąt świecenia reflektora
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 5.0f);
	// skupienie światła
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10.0f);
	// liniowe zanikanie światła wraz z odległością
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.01f);
	
	// światło kierunkowe o promieniach równoległych
	// czyli źródło w nieskończoności (światło słoneczne)
	// w odróżnieniu od pozostałych daje światło dla odbłysków (GL_SPECULAR)
	kolor[0]=1.0; kolor[1]=0.0; kolor[2]=0.6;
	glLightfv(GL_LIGHT1, GL_SPECULAR, kolor);
	pozycja[3]=0;
	glLightfv(GL_LIGHT1, GL_POSITION, pozycja);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, kierunek);
	
	// światło punktowe (żarówka na scenie)
	kolor[0]=0.0; kolor[1]=0.0; kolor[2]=0.6;
	glLightfv(GL_LIGHT0 + 2, GL_DIFFUSE, kolor);
	pozycja[2]=-3.0f; pozycja[3]=1;
	glLightfv(GL_LIGHT0 + 2, GL_POSITION, pozycja);
	// kwadratowe zanikanie światła wraz z odległością
	glLightf(GL_LIGHT0 + 2, GL_QUADRATIC_ATTENUATION, 0.3f);
	// warto zauważyć inny sposób podawania numeru światła ...
	// jest on przydatny zwłaszcza gdy nasza implementacja obsługuje więcej niż 8 źródeł
	// skoro mówimy o tym ograniczeniu liczby źródeł światła to warto wspomnieć że
	// po narysowaniu kawałka sceny możemy je przesunąć i pozmieniać aby narysować inny)
	
	// oświetlenie ogóle (AMBIENT)
	kolor[0]=0.5; kolor[1]=0.4; kolor[2]=0.0;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, kolor);
	// można jest stosować też przy normalnych światłach,
	// ale nie ma to większego zastosowania gdyż nie ma kierunku z którego ono pada
	
	// funkcje z grupy glLightModel mogą posłużyć także do aktywacji drugorzędnego
	// koloru odbicia (secondary specular color), ponadto poprzez glEnable(GL_COLOR_SUM)
	// i funkcje z rodziny glSecondaryColor możemy uzyskać kontolę nad ustawieniem
	// drugorzędnego koloru wierzchołka (dla tablic wertexów -  glSecondaryColorPointer)
	
	/// aktywacja świateł
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT0 + 2);
};

/// funkcja rysująca (przerysowująca) scenę
void narysuj_scene() {
	// jeżeli nastąpiła zmiana ustawień kamery to ją wdrażamy
	if (kamera.is_new) ustaw_kamere();
	
	// czyścimy ekran (ustawionym wcześniej) kolorem oraz czyścimy bufor głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	
	// napisy informacyjne
	char info[100];
	sprintf(info, "GL_LIGHTING: %d", glIsEnabled(GL_LIGHTING));
	umiesc_napis(info, -14.0f, -3.3f);
	sprintf(info, "GL_LIGHT0: %d", glIsEnabled(GL_LIGHT0));
	umiesc_napis(info, -14.0f, -4.0f);
	sprintf(info, "GL_LIGHT1: %d", glIsEnabled(GL_LIGHT1));
	umiesc_napis(info, -14.0f, -4.7f);
	sprintf(info, "GL_LIGHT2: %d", glIsEnabled(GL_LIGHT2));
	umiesc_napis(info, -14.0f, -5.4);
	// bardziej porządną implementację warstwy napisowej można znaleźć w "Podstawa silnika 3D"
	
	float mat[] = {0.9f, 0.9f, 0.9f, 1.0f};
	float mat_zero[] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// przelatujący stożek
	glTranslatef(0.0f, 0.0f, -7.0f+licznik_klatek/140.0f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
#ifdef GLUT_INIT
	glutSolidTeapot(1.0f);
#else
	glCallList(STOZEK);
#endif
	mat[0]=1.0f; mat[1]=1.0f; mat[2]=1.0f;
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	
	glLoadIdentity();
	glScalef(0.5, 0.5, 0.5);
	// dodatkowe stożki demonstrujące różne ustawienia materiałów,
	// czyli informacji o tym jak obiekt odbija światło
	
	// stożek oświetlony sam z siebie
	// kolorami takimi jak zdefiniowane (śledzenie koloru)
	glTranslatef(0.0f, 3.0f, -5.0f);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_EMISSION);
	glCallList(STOZEK);
	glDisable(GL_COLOR_MATERIAL);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
	// zamiast śledzenia koloru morzna włączyć GL_EMISSION poprzez
	// glMaterialfv z wskazanym kolorem świecenia
	
	// stożek na wysoki połysk
	mat[0]=0.0f; mat[1]=0.0f; mat[2]=0.8f;
	glTranslatef(0.0f, -2.5f, 0.0f);
	glMateriali(GL_FRONT, GL_SHININESS, 7);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
	glCallList(STOZEK);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_zero);
	glMateriali(GL_FRONT, GL_SHININESS, 0);
	
	// stożek na wysoki oświetlany światłem rozproszonym
	mat[0]=1.0f; mat[1]=1.0f; mat[2]=1.0f; mat[3]=0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	glTranslatef(0.0f, -2.5f, 0.0f);
	glCallList(STOZEK);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_zero);
	
	// prawdziwy efekt materiału (np. złota) uzyskuje się poprzez
	// ustawienie odpowiednich wartości wszystkich parametrów:
	// * GL_AMBIENT (współczynniki odbicia światła otaczającego
	//   - padającego tak samo z wszystkich kierunków)
	// * GL_DIFFUSE (współczynniki odbicia światła rozproszonego
	//   - padającego z określonych kierunków, odbijanego równomiernie)
	// * GL_SPECULAR (współczynniki odbicia światła odbłysków
	//   - padającego z określonych kierunków, odbijanego kierunkowo)
	// * GL_SHININESS (stopnia połysku
	//   - wielkości plamy powstałej z światła odbłysków)
	
	
	// licznik FPS
	licznik_klatek++;
	if (licznik_klatek == 1000) {
		licznik_klatek = 0;
	}
	
	// wywołujemy (zależną od wyżej poziomowego mechanizmu) funkcję rysującą scenę do okna
#ifdef DRAW_TO_SCREEN
	DRAW_TO_SCREEN;
#elif MODE == GLUT
	glutSwapBuffers();
#elif MODE == SDL
	SDL_GL_SwapBuffers();
#elif MODE == GLX
	glXSwapBuffers(oknoGLX.wyswietlacz, oknoGLX.okno);
#else
#	error "Couldn't determine DRAW_TO_SCREEN function"
#endif
}
