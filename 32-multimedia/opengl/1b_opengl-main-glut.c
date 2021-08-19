/**

plik odpowiedzialny za obsługę "bibliotek wysokopoziomowych" OpenGL - glut

plik ten demonstruje bardziej zaawansowane korzystanie z GLUTa
(niepokazane w opengl-main-glut_sdl_glx.c dla innych mechanizmów wysokopoziomowych)

WARTO JESZCZE ZAPOZNAĆ SIĘ Z:
* vertex i pixel (fragment) shadery - OpenGL Shading Language (GLSL) - http://www.lighthouse3d.com/opengl/glsl/
* cienie - http://forum.gamedev.pl/index.php/topic,754.0.html

* antyaliasing i multisampling,
* efekty realizowane w oparciu o bufory akumulacyjny (rozmycie ruchu itp) i szablonowy (lustro)
* tryb selekcji obiektów - obsługa strzału w obiekt
  (tu warto wspomnieć o trzecim trybie - sprzężenia zwrotnego - dającym precyzyjniejsze informacje ...)
* efekty cząsteczkowe (Sprajty punktowe), czyli jak zrobić deszcz
* ewaluacja krzywych i powierzchni

UWAGA:
Są to dość stare przykłady korzystające z Fixed Function Pipeline nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. Ogre 3D).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

**/

//#define DEBUG(a) puts(a);
#define DEBUG(a) ;

/// ustawiamy MODE == GLUT
#define GLUT 1
#define MODE 1
// gcc -lGL -lGLU -lglut -lglpng  -D DRAW_FILE=\"3a_opengl-draw-simple.c\" 1b_opengl-main-glut.c && ./a.out

/// określamy plik z funkcją używaną do rysowania
#ifndef DRAW_FILE
# define DRAW_FILE "3b_opengl-draw-light.c"
#endif

/// włączamy plik odpowiedzialny za ustawienia "czystego" OpenGL
#include "2_opengl-init.c"

/// standardowe pliki nagłówkowe
#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h> // sleep, usleep


/// funkcja główna - przygotowanie do funkcji obsługi OpenGL
/// oraz funkcje pomocnicze dla niej
/// GLUT (-lglut)
#include <GL/glut.h>

// określenie funkcji rysującej na ekran (zależne od MODE)
#define DRAW_TO_SCREEN glutSwapBuffers()
// makro określające że wykonana była inicjalizacja glut'a
// i wolno korzystać z jego funkcji
#define GLUT_INIT 1
// włączenie pliku obsługującego rysowanie "czystym" OpenGL
#include DRAW_FILE

// globalna zmienna z identyfikatorem okna GLUT'owego
int okno;

// definicje kroków związanych z modyfikacjami kamery
#define step_xyz 0.05
#define step_obr 0.05
#define step_zoom 0.05
#define max_zoom 20
#define min_zoom 80

// funkcja obsługująca reakcję na zdarzenia klawiatury
void nacisnieto_klawisz(unsigned char klawisz, int x, int y) {
	DEBUG("nacisnieto KLAWISZ");
	// naciśnięcie q lub ESCAPE (kod ASCII)
	// klawisze specjalne zdefiniowane w GL/glut.h (np. GLUT_KEY_DOWN)
	switch (klawisz) {
		case 'q':
		case 0x1b:
			// niszczymy okno GLUT'owe
			glutDestroyWindow(okno);
			// kończymy program
			exit(0);
		case 'F':
			// przejście w tryb pełnoekranowy
			glutFullScreen();
			// w zasadzie przedtem powinniśmy zapamiętać:
			// glutGet(GLUT_WINDOW_WIDTH) glutGet(GLUT_WINDOW_HEIGHT)
			// oraz położenie okna aby muc wrócić do trybu okienkowego przy pomocy:
			// glutReshapeWindow() i glutPositionWindow()
			break;
		case 'w':
			kamera.z += step_xyz;
			kamera.is_new = 1;
			break;
		case 's':
			kamera.z -= step_xyz;
			kamera.is_new = 1;
			break;
		case 'a':
			kamera.x += step_xyz;
			kamera.is_new = 1;
			break;
		case 'd':
			kamera.x -= step_xyz;
			kamera.is_new = 1;
			break;
		case 'f':
			kamera.y += step_xyz;
			kamera.is_new = 1;
			break;
		case 'r':
			kamera.y -= step_xyz;
			kamera.is_new = 1;
			break;
		case 'i':
			printf ("kamera = {%f,%f,%f,   %f,%f,   %f};\n",
				kamera.x, kamera.y, kamera.z,
				kamera.obr_xz, kamera.obr_yz, kamera.zoom);
			kamera.is_new = 1;
			break;
	}
}

// funkcja wywoływana po naciśnięciu przycisku myszy
int x_0=0;
int y_0=0;
void nacisnieto_mysz(int przycisk, int stan, int x, int y) {
	DEBUG("nacisnieto MYSZ");
	if (przycisk == 3) { // kółko w górę
		if (kamera.zoom > max_zoom) {
			kamera.zoom -= step_zoom;
			kamera.is_new = 1;
		}
	} else if (przycisk == 4) { // kółko w dół
		if (kamera.zoom < min_zoom) {
			kamera.zoom += step_zoom;
			kamera.is_new = 1;
		}
	} else {
		x_0=x;
		y_0=y;
	}
	// UWAGA: wartości przycisków odpowiadających ruchowi kółka
	//        nie znalazłem w dokumentacji a odczytałem doświadczalnie
	//         - mogą się różnić pomiędzy implementacjami
}

// funkcja wywoływana przy poruszeniu myszki z wciśniętym przyciskiem
void przesunieto_mysz(int x,int y) {
	kamera.obr_xz -= (x-x_0)*step_obr;
	kamera.obr_yz -= (y-y_0)*step_obr;
	kamera.is_new = 1;
	x_0=x;
	y_0=y;
}

/// UWAGA: aby nie zaciemniać kodu tak rozbudowane sterowanie jak powyżej
///        jest tylko dla GLUTa, oczywiście nie stoi nic na przeszkodzie
///        aby dopisać je poprzez analogię do kodu dla innych bibliotek

// typ wyliczeniowy dla obsługui menu
enum {
	UKRYJ_PRZOD,
	UKRYJ_TYL,
	UKRYJ_NIC,
	RYSUJ_PUNKTY,
	RYSUJ_LINIE,
	RYSUJ_WYPELNIENIA,
	PRZELACZ_SWIATLA,
	PRZELACZ_SWIATLO_0,
	PRZELACZ_SWIATLO_1,
	PRZELACZ_SWIATLO_2,
	MGLA,
	PRZELACZ_TEXTURA_0,
	PRZELACZ_TEXTURA_1,
};
// funkcja wywoływana po wybraniu pozycji menu
void akcja_menu (int opcja) {
	switch (opcja) {
		case UKRYJ_TYL:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case UKRYJ_PRZOD:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case UKRYJ_NIC:
			glDisable(GL_CULL_FACE);
			break;
		case RYSUJ_WYPELNIENIA:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case RYSUJ_LINIE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case RYSUJ_PUNKTY:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case PRZELACZ_SWIATLA:
			if (glIsEnabled(GL_LIGHTING))
				glDisable(GL_LIGHTING);
			else
				glEnable(GL_LIGHTING);
			break;	
		case PRZELACZ_SWIATLO_0:
			if (glIsEnabled(GL_LIGHT0))
				glDisable(GL_LIGHT0);
			else
				glEnable(GL_LIGHT0);
			break;
		case PRZELACZ_SWIATLO_1:
			if (glIsEnabled(GL_LIGHT1))
				glDisable(GL_LIGHT1);
			else
				glEnable(GL_LIGHT1);
			break;
		case PRZELACZ_SWIATLO_2:
			if (glIsEnabled(GL_LIGHT2))
				glDisable(GL_LIGHT2);
			else
				glEnable(GL_LIGHT2);
			break;
		case MGLA:
			if (glIsEnabled(GL_FOG))
				glDisable(GL_FOG);
			else
				glEnable(GL_FOG);
			break;
		case PRZELACZ_TEXTURA_0:
			glActiveTexture(GL_TEXTURE0);
			if (glIsEnabled(GL_TEXTURE_2D))
				glDisable(GL_TEXTURE_2D);
			else
				glEnable(GL_TEXTURE_2D);
			break;
		case PRZELACZ_TEXTURA_1:
			glActiveTexture(GL_TEXTURE1);
			if (glIsEnabled(GL_TEXTURE_2D))
				glDisable(GL_TEXTURE_2D);
			else
				glEnable(GL_TEXTURE_2D);
			break;
	}
}

/// funkcja wywoływana przez timer
void print_fps(int arg) {
	int fps = licznik_klatek - arg;
	if (fps<0)fps +=1000; // gdy licznik się przekręcił
	printf("FPS: %d\n", fps);
	
	// za 1000ms wywołaj ponownie przekazując jako argument 
	// obecny stan licznika
	glutTimerFunc (1000, &print_fps, licznik_klatek);
	
	// warto być także świadomym możliwości użycia glutGet(GLUT_ELAPSED_TIME);
}


// funkcja główna
int main (int argc, char **argv) {
	// inicjalizacja GLUTa i przekazanie do niego argumentów wywołania
	glutInit(&argc, argv);
	// ustawienie trybu wyświetlania
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	// ustawienie początkowego rozmiaru okna GLUTowego
	glutInitWindowSize(500, 300);
	// ustawienie położenia okna GLUTowego (lewy górny róg)
	glutInitWindowPosition(0, 0);
	// utworzenie okna
	okno = glutCreateWindow("tytul okna");
	
	// przejście w tryb pełnoekranowy
	//glutFullScreen();
	
	// określenie funkcji odpowiedzialnej za rysowanie sceny
	glutDisplayFunc(&narysuj_scene);
	// określenie funkcji wykonywanej gdy nie ma zdarzeń do obsłużenia
	glutIdleFunc(&narysuj_scene);
	// określenie funkcji wykonywane przy zmianie rozmiaru okna
	glutReshapeFunc(&ustaw_rozmiar_sceny);
	
	// określenie funkcji wywoływanej po naciśnięciu przycisku na klawiaturze
	glutKeyboardFunc(&nacisnieto_klawisz);
	// określenie funkcji wywoływanej po naciśnięciu przycisku myszy
	glutMouseFunc(&nacisnieto_mysz);
	// określenie funkcji wywoływanej przy poruszeniu myszki z wciśniętym przyciskiem
	glutMotionFunc(&przesunieto_mysz);
	// gdy chcemy przechwytywać ruch bez naciskania przycisku to glutPassiveMotionFunc()
	
	// tworzymy pierwsze submenu
	int submenu1 = glutCreateMenu(akcja_menu);
	glutAddMenuEntry("przod", UKRYJ_PRZOD);
	glutAddMenuEntry("tyl", UKRYJ_TYL);
	glutAddMenuEntry("nic", UKRYJ_NIC);
	// tworzymy kolejne submenu
	int submenu2 = glutCreateMenu(akcja_menu);
	glutAddMenuEntry("punkty", RYSUJ_PUNKTY);
	glutAddMenuEntry("linie", RYSUJ_LINIE);
	glutAddMenuEntry("wypelnienia", RYSUJ_WYPELNIENIA);
	// tworzymy kolejne submenu
	int submenu3 = glutCreateMenu(akcja_menu);
	glutAddMenuEntry("all", PRZELACZ_SWIATLA);
	glutAddMenuEntry("0", PRZELACZ_SWIATLO_0);
	glutAddMenuEntry("1", PRZELACZ_SWIATLO_1);
	glutAddMenuEntry("2", PRZELACZ_SWIATLO_2);
	// tworzymy kolejne submenu
	int submenu4 = glutCreateMenu(akcja_menu);
	glutAddMenuEntry("0", PRZELACZ_TEXTURA_0);
	glutAddMenuEntry("1", PRZELACZ_TEXTURA_1);
	// tworzymy menu glowne
	glutCreateMenu(akcja_menu);
	glutAddSubMenu("Ukryj: ", submenu1);
	glutAddSubMenu("Rysuj jako: ", submenu2);
	glutAddSubMenu("Przelacz swiatlo: ", submenu3);
	glutAddSubMenu("Przelacz jedn. teksturujaca: ", submenu4);
	glutAddMenuEntry("Mgla", MGLA);
	
	// menu przypięte do prawego klawisza myszy
	glutAttachMenu (GLUT_RIGHT_BUTTON);
	
	// po 1000ms wywołujemy funkcję print_fps
	glutTimerFunc(1000, &print_fps, 0);
	
	// inicjalizacja OpenGL
	inicjalizuj_GL();
	
	// przygotowanie obiektów dla funkcji rysującej
	przygotuj_scene();
	
	// uruchomienie pętli przetwarzającej zdarzenia GLUT 
	glutMainLoop();
}
