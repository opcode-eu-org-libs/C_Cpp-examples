/**

plik odpowiedzialny za obsługę "czystego" OpenGL - ustawienia

UWAGA:
Są to dość stare przykłady korzystające z Fixed Function Pipeline nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. Ogre 3D).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

**/

/// jeżeli chemy skorzystać z shader'ów to włączamy plik z funkcjami obsługującymi ich ładowanie
#ifdef USE_SHADER
#include "2_opengl-load_shader.c"
#endif

/// nagłówki podstawowe OpenGL
// OpenGL (-lGL)
#include <GL/gl.h>
// GL Utility library (-lGLU)
#include <GL/glu.h>

/// funkcja inicjalizująca OpenGL _po_ utworzeniu jego okna
void inicjalizuj_GL() {
	// czyszczenie tła kolorem RGBA
	// kolor lekko czerwony, lekko przezroczysty
	// w GLUT (niekiedy) tło do przezroczystości z tego co za oknem, w SDL czarne
	glClearColor(0.0f, 0.4f, 0.0f, 0.3f);
	// czyszczenie bufora głębi zadaną wartością
	glClearDepth(1.0);
	
	// włączamy porównywanie głębi
	// funkcją tą możemy włączać wiele innych funcjonalności OpenGL (do wyłączania glDisable)
	glEnable(GL_DEPTH_TEST);
	// ustawienie funkcji porównującej głębie
	glDepthFunc(GL_LESS);
	
	// ustawiamy tryb shadingu na "smooth color", czyli:
	// kolor liczony dla każdego wierzchołka i interplolowany na resztę wielokąta
	glShadeModel(GL_SMOOTH);
	
	// ustawienia blendingu, czyli mówimy jak chemy używac kanału alfa
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	
	// włączamy ukryweanie niewidocznych powierzchni
	glEnable(GL_CULL_FACE);
	// ukrywamy tył
	glCullFace(GL_BACK);
	// strona przednia to ta której wierzchołki rysowane są zgodnie z ruchem wskazówek zegara
	glFrontFace(GL_CW);
	
	// włączamy efekty oświetlenia
	//glEnable( GL_LIGHTING );
	
	// ustawiamy mgłę - kolor
	float kolor_mgly[] = {0.3f, 0.35f, 0.3f, 0.0f}; // lekko zielonkawa
	glFogfv( GL_FOG_COLOR, kolor_mgly );
	// aby uzyskać mgłę pozaobiektami warto na ten sam kolor ustawić glClearColor()
	// oraz typ (wykładnicza) i jej gęstość; w przypadku mgły liniowej 
	// zamiast gęstości ustawiamy Z początku i końca mgły
	glFogi( GL_FOG_MODE, GL_EXP );
	glFogf( GL_FOG_DENSITY, 0.25f );
	// ale jej nie włączamy
	glDisable( GL_FOG );
	
	// ustawiamy redenrowanie wypełniające dla obu stron wielokątów
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// ustawienia specyficzne dla implementacji
	// konkretnie ustawiamy najbardziej poprawne korygowanie kolorów i tekstór przy liczeniu perspektywy
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	
	// wymuszamy wykonanie komend OpenGL w tym momencie
	glFlush();
}


/// struktura globalna przechowująca informacje o położeniu kamery
struct parametry_kamery {
	float x, y, z; // punkt z którego patrzymy
	float obr_xz; // obrót w plaszczyźnie podłogi (przeciwnie do wskazówek zegara)
	float obr_yz; // pochylanie góra-dół (dodatnie: patrzymy do góry)
	// ogólnie możliwy jest także obrót kamery wokół własnej osi (z)
	float zoom; // kąt widzenia (w y, bo w x zadany przez ratio okna)
	char is_new;
};
/// wartości domyślne (początkowe) dla kamery
//static struct parametry_kamery kamera = {0,0,0, 0,0, 40.0f};
static struct parametry_kamery kamera = {-1.999999,0.000000,-6.090007,   -38.939995,-2.999999,   40.000000};

/// funcja ustawiająca kamere
void ustaw_kamere() {
	// uzyskujemy informację o wysokości i szerokości view portu
	GLfloat view_port_info[4];
	glGetFloatv(GL_VIEWPORT, view_port_info);
	
	// ustawiamy jako bierzącą macierz rzutowania (gdzie i z kąd patrzy kamera)
	glMatrixMode(GL_PROJECTION);
	// ustawiamy _ją_ na identyczność
	glLoadIdentity();
	
	// ustawiamy perspektywę rzutowania:
	//  * kąt widzenia w Y
	//  * proporcje ekranu
	//  * Z przedniej płaszczyzny ocinania
	//  * Z tylniej płaszczyzny obcinania
	gluPerspective(kamera.zoom, view_port_info[2]/view_port_info[3], 0.1f, 100.0f);
	// warto wspomnieć o gluOrtho2D() która sprawia że wyświetlane na ekranie
	// są obiekty o współrzędnych x,y odpowiadajęce podanemu do niej prostokątowi
	
	// ustawiamy położenie kamery (w DX jest to osobna macierz - VIEW)
	// przesunięcie w prawo, w górę i w głąb ekranu
	glTranslatef(kamera.x, kamera.y, kamera.z);
	
	// obrót kamery w płaszyźnie yz (względem osi x)
	glRotatef(kamera.obr_yz, 1, 0, 0);
	// obrót kamery w płaszyźnie xz (względem osi y)
	glRotatef(kamera.obr_xz, 0, 1, 0);
	
	// oczywiście oprócz wyżej zaprezentowanej metody działań na macierzach
	// możemy operować jawnie na nich przy reprezentacji w postaci tablic jednowymiarowych
	// (wpisujemy ciórkiem kolejne wiersze), do załadowania przygotowanej takiej macierzy
	// do OpenGL (np jako GL_PROJECTION) służy funckja glLoadMatrixf() lub glMultMatrix()
	// wykonana po odpowiednim glMatrixMode
	
	kamera.is_new = 0;
	
	// przełączamy bierzącą macierz na macierz świata (domyślna wartość)
	glMatrixMode(GL_MODELVIEW);
}


/// funcja ustawiająca rozmiar sceny
void ustaw_rozmiar_sceny(int szerokosc, int wysokosc) {
	// ustawiamy rozmiar widoku: lewy gorny róg = (0,0) oraz wysokość i szerokość
	// w ogólności może on być mniejszy od rozmiaru okna
	glViewport(0, 0, szerokosc, wysokosc);
	
	// ustawiamy położenie i inne parametry kamery
	// (musimy przeliczyć bo zależą od rozmiaru view point'a)
	ustaw_kamere();
}
