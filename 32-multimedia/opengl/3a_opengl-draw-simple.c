/**

plik odpowiedzialny za obsługę "czystego" OpenGL - rysowanie
ukazane są proste metody rysowania oraz korzystanie z list

ponadto pokazuje jak można wykorzystać GLSL
 (w oparciu o "opengl-load_shader.c" includowany w "opengl-init.c")
 aby w tym przykładzie użyć shaderów zamiast stałego potoku
 do opcji kompilacji należy dodać: "-lGLEW -D USE_SHADER"

UWAGA:
Są to dość stare przykłady korzystające z Fixed Function Pipeline nie obsługiwanym w współczesnych wersjach OpenGL i wspierającym je sprzęcie.
Współcześnie także używanie surowego OpenGL jest rzadkością – dużo częściej stosowane są wyżej poziomowe biblioteki związane z obsługą 3D (np. Ogre 3D).
W związku z tym przykłady z tego katalogu należy traktować czysto dydaktycznie (celem zrozumienia jak to może działać, jak to może być robione) niż jako punkt wyjścia do tworzenia czegoś.

**/

/// nagłowki ogólne
#include <math.h>

/// nagłówek biblioreki libglpng odpowiedzialnej za
/// obsługę plików png dla OpenGL
#include <GL/glpng.h>

/// zmienne globalne trzymające informacje o teksturach i jej plikach
GLuint tekstura;
pngInfo tekstura_info;

/// zmienna globalna przechowująca licznik klatek
int licznik_klatek = 0;

/// funkcja przygotowująca scenę
#define STORZEK 11 // coś >0
void przygotuj_scene(void) {
	/// przygotowujemy obiekt stożka
	// aby w każdej ramce nie musieć przeliczać stożka
	// (funkcje trygonometryczne nie są szybkie)
	// przygotowujemy go wcześniej i traktujemy jako gotowy obiekt
	glNewList(STORZEK, GL_COMPILE);
	// listy możemy także usuwać - glDeleteLists()
	// możemy też uzyskiwać zbiór zadanej ilości identyfikatorów - glGenLists
	// oraz sprawdzać czy identyfikator jest wolny - glIsList
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
	
	/// wczytanie pliku tekstury
	tekstura = pngBind("tekstura.png", PNG_BUILDMIPMAPS, PNG_ALPHA, &tekstura_info, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	// UWAGA: nie wiem dlaczego ale nie wszystkie PNG wczytują się poprawnie
	
	// korzystamy tutaj z funkcji przypisującej od rzu teksturę do obiektu teksturowego OpenGL,
	// normalnie obiekt taki jest tworzony poprzez wywołanie glGenTextures 
	// obiekty te ustawiane jako aktywne (przełączane) są poprzez wywołanie funkcji
	// glBindTexture(), oprócz samej tekstury przechowują one też informacje ustawiane
	// funkcjami glTexParameter... takie jak powtarzalność itp cechy
	
	// bez zastosowania tego mechanizmu tekstury wraz z wszystkimi ich własnościami
	// muszą być ustawiane dla kolejnych obiektów niezależnie poprzez wywołanie
	// funkcji typu glTexImage2D i glTexParameter, aby wczytać teksturę do użycia
	// w taki sposób można skorzystać z funkcji pngLoad
	
	// ustawiamy powtarzalność tekstury na boki
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// wypełnianie kolorem ramki w górę
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// ustalamy kolor ramki
	GLint kolor[] = {255,255,255,255};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, kolor);
	// przy pomocy glTexParameteri możemy też ustawić filtrowanie tekstur (np. mipmapping)
	// ustawiamy trójliniowe wygładzanie (liniowa interpolacja najbliższych mipmap
	// a następnie filtrowanie liniowe) dla filtra pomniejszającego
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// i liniowe powiększanie dla powiększającego
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// warto zaznaczyć że niektóre powyższe ustawienia mogą być dokonane
	// od razu w funkcji pngBind
	
	// mechanizm obiektów teksturowych pozwala także na ustawienie priorytetu tekstury
	// wpływającego na przechowywanie jej w pamięci karty graficznej glAreTexturesResident()
	// (normalnie wybór tekstur przechowywanych w pamięci karty opiera się na
	// mechanizmach typu LRU lub MFU znanych z mechanizmów swappowania stron pamięci operacyjnej)
	
	// warto także wspomnieć o ficzerach nie omówionych tutaj szczegółowo, takich jak:
	// kompresja tekstur, tekstury kubiczne (teksturyujące sześcian), teksturach 3d
	// (pozwalających na łatwiejsze teksturowanie obiektów z nierównościami gdyż
	// zawierających kolejne warstwy tekstóry dobierane w zażności jak głeboko
	// położony element obiektu teksturujemy), tekstur głębi
	
	#ifdef USE_SHADER
	// jeżeli używamy shaderów to należy przygotować i ustawić odpowiedni program dla GPU ...
	GLenum typy[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	const char* pliki[] = {"shader.vert", "shader.frag"};
	glewInit();
	MakeShadersProgram( typy, pliki, 2, 1 );
	#endif
};

/// funkcja rysująca (przerysowująca) scenę
void narysuj_scene() {
	// jeżeli nastąpiła zmiana ustawień kamery to ją wdrażamy
	if (kamera.is_new) ustaw_kamere();
	
	// czyścimy ekran (ustawionym wcześniej) kolorem oraz czyścimy bufor głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// ustawiamy bieżącą macierz (GL_MODELVIEW) na identyczność
	glLoadIdentity();
	/** sprawdzenie że to rzeczywiście GL_MODELVIEW
	GLint tmp;
	glGetIntegerv (GL_MATRIX_MODE, &tmp);
	printf("%d from %d %d %d\n", tmp, GL_MODELVIEW, GL_PROJECTION, GL_TEXTURE); **/
	
	// mnożymy bieżącą macierz przez macierz przesunięcia (x, y, z)
	glTranslatef(-1.5f,0.0f,-6.0f);
	// możemy także skorzystać z mnożenia przez macierze:
	//  * skalowania glScalef(x, y, z);
	//  * obrotów glRotatef(kat w stopniach, x , y, z); zadany punkt i (0,0,0) określają oś obrotu
	
	// zaczynamy rysować łamaną zamkniętą bez wypełnienia
	// poprzez wskazywanie kolejnych jej punktów wraz z ich kolorami
	// punkty określamy za pomocą współrzędnych:
	//  x (rosnie w prawo), y (rośnie w górę), z (rośnie w przód)
	glBegin(GL_LINE_LOOP);
		glLineWidth(1.0f);
		// zobacz też glLineStipple
		glColor3f(1.0f,0.0f,0.0f);	glVertex3f( 0.0f, 0.0f, 0.0f);
		glColor3f(1.0f,1.0f,0.0f);	glVertex3f( 1.0f, 0.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f);	glVertex3f( 1.0f, 1.0f, 0.0f);
		glColor3f(0.0f,1.0f,1.0f);	glVertex3f(-1.0f, 0.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f);	glVertex3f( 0.0f,-1.0f, 0.0f);
		glColor3f(1.0f,1.0f,1.0f);	glVertex3f( 1.0f,-1.0f, 0.0f);
		// należy wspomnieć że zamiast bezpośredniego określania kolorów,
		// możemy je określić w buforze i posługiwać się tylko numerami
	glEnd(); // kończymy rysować
	
	// kolejny obiekt jest o
	//  licznik_klatek/300.0f na prawo od poprzedniego
	//  1.0 do przodu od poprzedniego
	// OSTRZEŻENIE: prędkość animacji obiektu zależna od fps a nie czasu !!!
	//              nie powinno się tak robić ze względu na różne
	//              zachowanie w zależności od szybkości komputera
	glTranslatef(licznik_klatek/300.0f,0.0f,1.0f);
	// półprzezroczysty prostokąt (prawa ścianka wgłąb sceny) z nałożoną tekturą
	// patrz ustawienia blendingu w funkcji inicjalizującej
	glColor4f(0.5f,0.5f,1.0f,0.5f);
	// jako teksturę 2D ustawiamy wczytaną wcześniej teksturę
	// poprzez wywołania tej funkcji możemy różne obiekty teksturować innymi obrazkami
	glBindTexture(GL_TEXTURE_2D, tekstura);
	// tekstura będzie modulowana kolorem obiektu (jest to ustawienie całego
	// środowiska teksturującego a nie ustawienie przypisane konkretnej teksturze)
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// warto bliżej przyjrzeć się możliwym ustawieniom gdyż pozwalają 
	// na uzyskanie wielu ciekawych efektów
	// włączamy teksturowanie
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.4f);
		glTexCoord2f(2.5f, 0.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(2.5f, 1.5f); glVertex3f( 1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 1.5f); glVertex3f(-1.0f, 0.0f, 0.4f);
	glEnd();
	// wyłaczamy teksturowanie (inne obiekty mają być tylko kolorowane)
	glDisable(GL_TEXTURE_2D);
	
	// warto wspomnieć o uzyskiwaniu przeźroczystości obiektu z wykorzystaniem
	// kanału alfa tekstury, efekt ten możemy uzyskać na dwa sposoby:
	// * poprzez zastosowanie standardowego blendingu (należy uważać na kolejność
	//   rysowania obiektów - obiekty najbliżej kamery muszą być rysowane na końcu)
	// * poprzez testowanie wartości alfa fragmentów (IMHO trochę gorszy efekt -
	//   bardziej ostre przejścia) przy pomocy funkcji glEnable (GL_ALPHA_TEST); i
	//   glAlphaFunc(tryb_porownania, wartosc);
	
	// tym razem pozycjonujemy znowu od początku a nie od ostatniego obiektu
	glLoadIdentity();
	// zamiast tego przed każdą modyfikacja możemy dawać glPushMatrix();
	// a po narysowaniu tak przesuniętych obiektów glPopMatrix();
	glTranslatef(0.5f,0.0f,-7.0f+licznik_klatek/140.0f);
	glCallList(STORZEK);
	
	// licznik FPS
	licznik_klatek++;
	if (licznik_klatek == 1000) {
		licznik_klatek = 0;
#if MODE == SDL
		static czas_ostatniego_wypisania = 0;
		GLint czas1 = SDL_GetTicks();
		GLint czas2 = czas1 - czas_ostatniego_wypisania;
		czas_ostatniego_wypisania = czas1;
		printf("FPS %g\n", 1000000.0 / czas2); // 1000 ms/s * 1000 frame / x s
#endif
	}
	
	// wywołujemy (zależną od wyżej poziomowego mechanizmu) funkcję rysujące scenę do okna
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
